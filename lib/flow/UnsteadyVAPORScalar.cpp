#include "vapor/UnsteadyVAPORScalar.h"
#include "vapor/Particle.h"

using namespace flow;

// Constructor
UnsteadyVAPORScalar::UnsteadyVAPORScalar() {
    IsSteady = true;
    _grid = nullptr;
}

// Destructor
UnsteadyVAPORScalar::~UnsteadyVAPORScalar() {
    if (_grid)
        delete _grid;
}

int UnsteadyVAPORScalar::GetScalar(float t, const glm::vec3 &pos, float &val) const {
    if (!_grid)
        return NO_FIELD_YET;

    if (!InsideVolume(t, pos))
        return OUT_OF_FIELD;

    const std::vector<double> coords{pos.x, pos.y, pos.z};
    float scalar = _grid->GetValue(coords);
    // Need to do: examine scalar is not missing value
    val = scalar;

    return 0;
}

bool UnsteadyVAPORScalar::InsideVolume(float time, const glm::vec3 &pos) const {
    std::vector<double> coords{pos.x, pos.y, pos.z};
    if (!_grid->InsideGrid(coords))
        return false;
    else
        return true;
}

void UnsteadyVAPORScalar::UseGrid(const VGrid *g) { _grid = g; }

int UnsteadyVAPORScalar::GetExtents(float time, glm::vec3 &minExt, glm::vec3 &maxExt) const {
    if (!_grid)
        return NO_FIELD_YET;

    std::vector<double> gridMin, gridMax;
    _grid->GetUserExtents(gridMin, gridMax);
    glm::vec3 uMin(gridMin.at(0), gridMin.at(1), gridMin.at(2));
    glm::vec3 uMax(gridMax.at(0), gridMax.at(1), gridMax.at(2));

    return 0;
}

int UnsteadyVAPORScalar::GetNumberOfTimesteps() const { return 1; }
