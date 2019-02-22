/*
 * Defines a particle used in flow integration.
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>
#include <vector>

namespace flow {
class Particle {
  public:
    glm::vec3 location;
    float time;
    std::vector<float> properties;

    // Constructor and destructor
    Particle();
    Particle(const glm::vec3 &loc, float t);
    Particle(const float *loc, float t);
    Particle(float x, float y, float z, float t);
    ~Particle();
};

}; // namespace flow

#endif
