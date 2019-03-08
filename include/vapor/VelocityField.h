/*
 * The base class of all possible velocity fields for flow integration.
 */

#ifndef VELOCITYFIELD_H
#define VELOCITYFIELD_H

#include "vapor/Particle.h"
#include <glm/glm.hpp>
#include <string>

namespace flow {
class VelocityField {
  public:
    // Constructor and destructor
    VelocityField();
    virtual ~VelocityField();

    //
    // Get the velocity value at a certain position, at a certain time.
    //
    virtual int GetVelocity(float time, const glm::vec3 &pos, // input
                            glm::vec3 &vel) const = 0;        // output
    //
    // Get the field value at a certain position, at a certain time.
    //
    virtual int GetScalar(float time, const glm::vec3 &pos, // input
                          float &val) const = 0;            // output

    //
    // If a given position at a given time is inside of this field
    //
    virtual bool InsideVolume(float time, const glm::vec3 &pos) const = 0;

    //
    // Retrieve the extents of this field.
    //
    virtual int GetExtents(float time, glm::vec3 &minExt, glm::vec3 &maxExt) const = 0;

    // Class members
    bool IsSteady;
    bool IsPeriodic;
    bool HasFieldValue;
    std::string VarNameU, VarNameV, VarNameW; // Varuable names for 3 velocity components
    std::string VarNameField;                 // Varuable names for the value field
};
}; // namespace flow

#endif
