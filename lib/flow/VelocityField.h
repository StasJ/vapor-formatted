/*
 * The base class of all possible velocity fields for flow integration.
 */

#ifndef VELOCITYFIELD_H
#define VELOCITYFIELD_H

#include "Particle.h"
#include <glm/glm.hpp>

namespace flow {
class VelocityField {
  public:
    // Constructor and destructor
    VelocityField();
    ~VelocityField();

    //
    // Get the velocity value at a certain position, at a certain time.
    //
    virtual int GetVelocity(float time, const glm::vec3 &pos, // input
                            glm::vec3 &vel) const = 0;        // output

    //
    // If a given position at a given time is inside of this field
    //
    virtual bool InsideVelocityField(float time, const glm::vec3 &pos) const = 0;

    // Class members
    bool IsSteady;
    bool IsPeriodic;

  protected:
    template <class T> T lerp(const T &v1, const T &v2, float a) const {
        return glm::mix(v1, v2, a);
    }
};
}; // namespace flow

#endif
