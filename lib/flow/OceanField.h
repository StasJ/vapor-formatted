/*
 * A derived VelocityField that simulates the ocean flow.
 */

#ifndef OCEANFIELD_H
#define OCEANFIELD_H

#include "VelocityField.h"

namespace flow {
class OceanField : public VelocityField {
  public:
    OceanField();
    ~OceanField();

    int GetVelocity(float time, const glm::vec3 &pos, glm::vec3 &vel) const;
    bool InsideVelocityField(float time, const glm::vec3 &pos) const;
};

}; // namespace flow

#endif