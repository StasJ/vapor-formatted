#include "vapor/VaporField.h"

using namespace flow;

VaporField::VaporField() {
    _datamgr = nullptr;
    _params = nullptr;
}

// Destructor
VaporField::~VaporField() {}

bool VaporField::InsideVolume(float time, const glm::vec3 &pos) const {
    const std::vector<double> coords{pos.x, pos.y, pos.z};
    VAPoR::Grid *grid = nullptr;
    if (!_params)
        return false;
    for (auto &e : VelocityNames)
        if (e.empty())
            return false;

    // In case of steady field, we only check a specific time step
    if (IsSteady) {
        size_t currentTS = _params->GetCurrentTimestep();
        for (auto v : VelocityNames) // cannot use reference here...
        {
            int rv = _getAGrid(currentTS, v, &grid);
            assert(rv == 0);
            if (!grid->InsideGrid(coords)) {
                delete grid;
                return false;
            }
            delete grid;
            grid = nullptr;
        }
    } else // we check two time steps
    {
        // First check if the query time is within range
        if (time < _timestamps.front() || time > _timestamps.back())
            return false;

        // Then locate the 2 time steps
        size_t floor;
        int rv = LocateTimestamp(time, floor);
        if (rv != 0)
            return false;

        // Second test if pos is inside of time step "floor"
        for (auto v : VelocityNames) // cannot use references...
        {
            rv = _getAGrid(floor, v, &grid);
            assert(rv == 0);
            if (!grid->InsideGrid(coords)) {
                delete grid;
                return false;
            }
            delete grid;
            grid = nullptr;
        }

        // If time is larger than _timestamps[floor], we also need to test _timestamps[floor+1]
        if (time > _timestamps[floor]) {
            for (auto v : VelocityNames) // cannot use references
            {
                rv = _getAGrid(floor + 1, v, &grid);
                assert(rv == 0);
                if (!grid->InsideGrid(coords)) {
                    delete grid;
                    return false;
                }
                delete grid;
                grid = nullptr;
            }
        }
    }

    return true;
}

void VaporField::AssignDataManager(VAPoR::DataMgr *dmgr) {
    _datamgr = dmgr;

    // Make a copy of the timestamps from the new data manager
    const auto &timeCoords = dmgr->GetTimeCoordinates();
    _timestamps.resize(timeCoords.size());
    for (size_t i = 0; i < timeCoords.size(); i++)
        _timestamps[i] = timeCoords[i];
}

void VaporField::UpdateParams(const VAPoR::FlowParams *p) { _params = p; }

int VaporField::LocateTimestamp(float time, size_t &floor) const {
    if (_timestamps.size() == 0)
        return TIME_ERROR;
    if (_timestamps.size() == 1) {
        if (_timestamps[0] != time)
            return TIME_ERROR;
        else {
            floor = 0;
            return 0;
        }
    }

    if (time < _timestamps.front() || time > _timestamps.back())
        return TIME_ERROR;
    else {
        floor = _binarySearch(_timestamps, time, 0, _timestamps.size() - 1);
        return 0;
    }
}

template <typename T>
size_t VaporField::_binarySearch(const std::vector<T> &vec, T val, size_t begin, size_t end) const {
    if (begin + 1 == end) {
        if (val == vec[end])
            return end;
        else
            return begin;
    }

    size_t middle = begin + (end - begin) / 2;
    if (val == vec[middle])
        return middle;
    else if (val < vec[middle])
        return _binarySearch(vec, val, begin, middle);
    else
        return _binarySearch(vec, val, middle, end);
}

int VaporField::GetNumberOfTimesteps() const { return _timestamps.size(); }

int VaporField::_getAGrid(int timestep, std::string &varName, VAPoR::Grid **gridpp) const {
    std::vector<double> extMin, extMax;
    _params->GetBox()->GetExtents(extMin, extMax);
    VAPoR::Grid *grid = _datamgr->GetVariable(timestep, varName, _params->GetRefinementLevel(),
                                              _params->GetCompressionLevel(), extMin, extMax);
    if (grid == nullptr) {
        Wasp::MyBase::SetErrMsg("Not able to get a grid!");
        return GRID_ERROR;
    } else {
        *gridpp = grid;
        return 0;
    }
}
