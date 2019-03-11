#include "vapor/OceanField.h"
#include "vapor/Particle.h"
#include <cmath>

using namespace flow;

OceanField::OceanField() {}

OceanField::~OceanField() {}

bool OceanField::InsideVolume(float t, const glm::vec3 &pos) const {
    if (glm::length(pos) > 1.0f)
        return false;
    else
        return true;
}

int OceanField::GetVelocity(float t, const glm::vec3 &pos, glm::vec3 &vel) const {
    if (!InsideVolume(t, pos))
        return OUT_OF_FIELD;

    // First calculate the direction of the velocity
    if (std::fabs(pos.y) > 0.001) // For numerical stability reasons
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

#if 0
int
OceanField::GetScalar( float t, const glm::vec3& pos, float& val ) const
{
    if( !InsideVolume( t, pos ) )
        return OUT_OF_FIELD;
    else
    {
        val = glm::length( pos );
        return 0;
    }
}
#endif

int OceanField::GetExtents(float time, glm::vec3 &minExt, glm::vec3 &maxExt) const {
    minExt = glm::vec3(-1.0, -1.0, -1.0);
    maxExt = glm::vec3(1.0, 1.0, 1.0);
    return 0;
}
