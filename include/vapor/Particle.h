/*
 * Defines a particle used in flow integration.
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include <forward_list>
#include <glm/glm.hpp>

namespace flow {
enum ERROR_CODE {
    ADVECT_HAPPENED = 1,
    SUCCESS = 0,
    OUT_OF_FIELD = -1,
    NO_FIELD_YET = -2,
    NO_SEED_PARTICLE_YET = -3,
    FILE_ERROR = -4,
    TIME_ERROR = -5,
    GRID_ERROR = -6,
    SIZE_MISMATCH = -7
};

class Particle {
  public:
    glm::vec3 location{0.0f, 0.0f, 0.0f};
    float time = 0.0f;
    float value = 0.0f;

    // Constructors.
    // This class complies with rule of zero.
    Particle() = default;
    Particle(const glm::vec3 &loc, float t, float val = 0.0f);
    Particle(const float *loc, float t, float val = 0.0f);
    Particle(float x, float y, float z, float t, float val = 0.0f);

    void AttachProperty(float v);
    // This function will throw an exception when idx is out of bound
    float RetrieveProperty(int idx) const;
    void ClearProperties();
    int GetNumOfProperties() const;

    // A particle could be set to be at a special state.
    void SetSpecial(bool isSpecial);
    bool IsSpecial() const;

  private:
    std::forward_list<float> _properties;
    // Note on the choice of using a forward_list:
    // Forward_list takes 8 bytes, whereas a vector or list take 24 bytes!
};

}; // namespace flow

#endif
