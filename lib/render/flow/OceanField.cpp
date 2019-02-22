#include "OceanField.h"
#include <cmath>

using namespace flow;

OceanField::OceanField() {
    // Ocean field has a radius of one.
    _fieldMin = glm::vec3(-1.0f);
    _fieldMax = glm::vec3(1.0f);
}

bool OceanField::insideField(const glm::vec3 &pos) const {
    if (glm::length(pos) > 1.0f)
        return false;
    else
        return true;
}

int OceanField::Get(float t, const glm::vec3 &pos, glm::vec3 &vel) const {
    if (!insideField(pos))
        return OUT_OF_FIELD;

    // First calculate the direction of the velocity
    if (std::fabs(pos.y) > 0.01) // For numerical stability reasons
    {
        // Depending on the sign of pos.y, make sure give vel a counter-clockwise direction.
        if (pos.y > 0) {
            vel.x = -1.0f;
            vel.y = pos.x / pos.y;
        } else {
            vel.x = 1.0f;
            vel.y = -pos.x / pos.y;
        }
    } else {
        if (pos.x > 0) {
            vel.y = 1.0f;
            vel.x = -pos.y / pos.x;
        } else {
            vel.y = -1.0f;
            vel.x = pos.y / pos.x;
        }
    }
    vel.z = 0.0f;

    // Second, the magnitude of vel equals to the distance pos to the origin.
    float len = glm::length(pos);
    vel = len * glm::normalize(vel);

    return 0;
}
