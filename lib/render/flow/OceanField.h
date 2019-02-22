/*
 * A drived VelocityField that simulates the ocean flow.
 */

#ifndef OCEANFIELD_H
#define OCEANFIELD_H

#include "VelocityField.h"

namespace flow {
class OceanField : public VelocityField {
  public:
    OceanField();
    ~OceanField();

    int Get(float time, const glm::vec3 &pos, glm::vec3 &vel) const;
    bool insideField(const glm::vec3 &pos) const;
};

}; // namespace flow

#endif
