#include "vapor/VaporField.h"

using namespace flow;

VaporField::VaporField() : _recentGridLimit(12) {
    _datamgr = nullptr;
    _params = nullptr;
}

// Destructor
VaporField::~VaporField() {}

bool VaporField::InsideVolume(float time, const glm::vec3 &pos) {
    const std::vector<double> coords{pos.x, pos.y, pos.z};
    const VAPoR::Grid *grid = nullptr;
    assert(_isReady());

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

int VaporField::GetVelocity(float time, const glm::vec3 &pos, glm::vec3 &velocity) {
    const std::vector<double> coords{pos.x, pos.y, pos.z};
    const VAPoR::Grid *grid = nullptr;

    // First make sure the query positions are inside of the volume
    if (!InsideVolume(time, pos))
        return OUT_OF_FIELD;

    if (IsSteady) {
        size_t currentTS = _params->GetCurrentTimestep();
        for (int i = 0; i < 3; i++) {
            auto varname = VelocityNames[i];
            int rv = _getAGrid(currentTS, varname, &grid);
            assert(rv == 0);
            velocity[i] = grid->GetValue(coords);
            delete grid;
            grid = nullptr;
        }
        // Need to do: examine if velocity contains missing value.
    } else {
        // First check if the query time is within range
        if (time < _timestamps.front() || time > _timestamps.back())
            return TIME_ERROR;

        // Then we locate the floor time step
        size_t floorTS;
        int rv = LocateTimestamp(time, floorTS);
        assert(rv == 0);

        // Find the velocity values at floor time step
        glm::vec3 floorVelocity;
        for (int i = 0; i < 3; i++) {
            auto varname = VelocityNames[i];
            int rv = _getAGrid(floorTS, varname, &grid);
            assert(rv == 0);
            floorVelocity[i] = grid->GetValue(coords);
            delete grid;
            grid = nullptr;
        }
        // Need to do: examine if velocity contains missing value.

        // Find the velocity values at the ceiling time step
        if (time == _timestamps[floorTS])
            velocity = floorVelocity;
        else {
            glm::vec3 ceilVelocity;
            for (int i = 0; i < 3; i++) {
                auto varname = VelocityNames[i];
                int rv = _getAGrid(floorTS + 1, varname, &grid);
                assert(rv == 0);
                ceilVelocity[i] = grid->GetValue(coords);
                delete grid;
                grid = nullptr;
            }
            // Need to do: examine if velocity contains missing value.
            float weight =
                (time - _timestamps[floorTS]) / (_timestamps[floorTS + 1] - _timestamps[floorTS]);
            velocity = glm::mix(floorVelocity, ceilVelocity, weight);
        }
    }

    return 0;
}

int VaporField::GetScalar(float time, const glm::vec3 &pos, float &scalar) {
    if (!InsideVolume(time, pos))
        return OUT_OF_FIELD;

    if (ScalarName.empty())
        return NO_FIELD_YET;
    std::string scalarname = ScalarName; // const requirement...

    const std::vector<double> coords{pos.x, pos.y, pos.z};
    const VAPoR::Grid *grid = nullptr;

    if (IsSteady) {
        size_t currentTS = _params->GetCurrentTimestep();
        int rv = _getAGrid(currentTS, scalarname, &grid);
        assert(rv == 0);
        scalar = grid->GetValue(coords);
        delete grid;
        grid = nullptr;
        // Need to do: examine if velocity contains missing value.
    } else {
        // First check if the query time is within range
        if (time < _timestamps.front() || time > _timestamps.back())
            return TIME_ERROR;

        // Then we locate the floor time step
        size_t floorTS;
        int rv = LocateTimestamp(time, floorTS);
        assert(rv == 0);
        rv = _getAGrid(floorTS, scalarname, &grid);
        assert(rv == 0);
        float floorScalar = grid->GetValue(coords);
        delete grid;
        grid = nullptr;

        if (time == _timestamps[floorTS])
            scalar = floorScalar;
        else {
            rv = _getAGrid(floorTS + 1, scalarname, &grid);
            assert(rv == 0);
            float ceilScalar = grid->GetValue(coords);
            delete grid;
            grid = nullptr;
            float weight =
                (time - _timestamps[floorTS]) / (_timestamps[floorTS + 1] - _timestamps[floorTS]);
            scalar = glm::mix(floorScalar, ceilScalar, weight);
        }
    }

    return 0;
}

bool VaporField::_isReady() const {
    if (!_datamgr)
        return false;
    if (!_params)
        return false;
    for (auto &e : VelocityNames)
        if (e.empty())
            return false;

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

void VaporField::UpdateParams(const VAPoR::FlowParams *p) {
    _params = p;
    // Update properties of this Field
    IsSteady = p->GetIsSteady();
    ScalarName = p->GetColorMapVariableName();
    auto velNames = p->GetFieldVariableNames();
    for (int i = 0; i < 3; i++)
        VelocityNames[i] = velNames.at(i);
}

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

int VaporField::GetNumberOfTimesteps() { return _timestamps.size(); }

int VaporField::_getAGrid(size_t timestep, std::string &varName, const VAPoR::Grid **gridpp) {
    // First check if we have the requested grid in our cache
    std::vector<double> extMin, extMax;
    _params->GetBox()->GetExtents(extMin, extMax);
    int refLevel = _params->GetRefinementLevel();
    int compLevel = _params->GetCompressionLevel();

    for (auto it = _recentGrids.cbegin(); it != _recentGrids.cend(); ++it)
        if (it->equals(timestep, varName, refLevel, compLevel, extMin, extMax)) {
            // Output the underlying grid;
            *gridpp = it->realGrid;
            // Move this node to the front of the list
            _recentGrids.splice(_recentGrids.cbegin(), _recentGrids, it);

            return 0;
        }

    // There's no such grid in our cache! Let's ask for it from the data manager,
    // and also keep it in our cache!

    VAPoR::Grid *grid =
        _datamgr->GetVariable(timestep, varName, refLevel, compLevel, extMin, extMax);
    if (grid == nullptr) {
        Wasp::MyBase::SetErrMsg("Not able to get a grid!");
        return GRID_ERROR;
    }
    // Outout the grid:
    *gridpp = grid;
    // Put it in our cache
    _recentGrids.emplace_front(grid, timestep, varName, refLevel, compLevel, extMin, extMax,
                               _datamgr);
    if (_recentGrids.size() > _recentGridLimit)
        _recentGrids.pop_back();

    return 0;
}

VaporField::RichGrid::RichGrid()
    : realGrid(nullptr), TS(0), varName(), refinementLevel(0), compressionLevel(0), extMin(),
      extMax(), mgr(nullptr) {}

VaporField::RichGrid::RichGrid(const VAPoR::Grid *g, size_t currentTS, const std::string &var,
                               int refLevel, int compLevel, const std::vector<double> &min,
                               const std::vector<double> &max, VAPoR::DataMgr *dm)
    : realGrid(g), TS(currentTS), varName(var), refinementLevel(refLevel),
      compressionLevel(compLevel), extMin(min), extMax(max), mgr(dm) {}

VaporField::RichGrid::~RichGrid() {
    if (mgr && realGrid) {
        mgr->UnlockGrid(realGrid);
        delete realGrid;
    }
}

bool VaporField::RichGrid::equals(size_t currentTS, const std::string &var, int refLevel,
                                  int compLevel, const std::vector<double> &min,
                                  const std::vector<double> &max) const {
    if (currentTS == TS && var == varName && refLevel == refinementLevel &&
        compLevel == compressionLevel && min == extMin && max == extMax)
        return true;
    else
        return false;
}
