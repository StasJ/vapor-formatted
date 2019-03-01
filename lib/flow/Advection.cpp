#include <cstdio>

#include "vapor/Advection.h"

using namespace flow;

Advection::Advection() {
    _vField = nullptr;
    _baseDeltaT = 0.01f;
    _lowerAngle = 3.0f;
    _upperAngle = 15.0f;
    _lowerAngleCos = glm::cos(glm::radians(_lowerAngle));
    _upperAngleCos = glm::cos(glm::radians(_upperAngle));
}

Advection::~Advection() { _vField = nullptr; }

void Advection::SetBaseStepSize(float f) { _baseDeltaT = f; }

void Advection::UseVelocityField(const VelocityField *p) { _vField = p; }

void Advection::UseSeedParticles(std::vector<Particle> &seeds) {
    _streams.clear();
    _streams.resize(seeds.size());
    for (size_t i = 0; i < seeds.size(); i++)
        _streams[i].push_back(seeds[i]);
}

int Advection::_readyToAdvect() const {
    if (_vField == nullptr)
        return NO_VECTOR_FIELD_YET;

    for (const auto &s : _streams) {
        if (s.size() < 1)
            return NO_SEED_PARTICLE_YET;
    }

    return 0;
}

int Advection::Advect(ADVECTION_METHOD method) {
    int ready = _readyToAdvect();
    if (ready != 0)
        return ready;

    for (auto &s : _streams) {
        const auto &p0 = s.back();
        if (!_vField->InsideVelocityField(p0.time, p0.location))
            continue;

        float dt = _baseDeltaT;
        // If there are at least 3 particles in the stream, we also adjust dt
        if (s.size() > 2) {
            const auto &past1 = s[s.size() - 2];
            const auto &past2 = s[s.size() - 3];
            dt = p0.time - past1.time; // step size used by last integration
            dt *= _calcAdjustFactor(past2, past1, p0);
        }

        Particle p1;
        int rv;
        switch (method) {
        case EULER:
            rv = _advectEuler(p0, dt, p1);
            break;
        case RK4:
            rv = _advectRK4(p0, dt, p1);
            break;
        }

        if (rv == 0)
            s.push_back(p1);
    }

    return 0;
}

int Advection::_advectEuler(const Particle &p0, float dt, Particle &p1) const {
    glm::vec3 v0;
    int rv = _vField->GetVelocity(p0.time, p0.location, v0);
    assert(rv == 0);
    p1.location = p0.location + dt * v0;
    p1.time = p0.time + dt;
    return 0;
}

int Advection::_advectRK4(const Particle &p0, float dt, Particle &p1) const {
    glm::vec3 k1, k2, k3, k4;
    float dt2 = dt * 0.5f;
    int rv;
    rv = _vField->GetVelocity(p0.time, p0.location, k1);
    assert(rv == 0);
    rv = _vField->GetVelocity(p0.time + dt2, p0.location + dt2 * k1, k2);
    if (rv != 0)
        return rv;
    rv = _vField->GetVelocity(p0.time + dt2, p0.location + dt2 * k2, k3);
    if (rv != 0)
        return rv;
    rv = _vField->GetVelocity(p0.time + dt, p0.location + dt * k3, k4);
    if (rv != 0)
        return rv;
    p1.location = p0.location + dt / 6.0f * (k1 + 2.0f * (k2 + k3) + k4);
    p1.time = p0.time + dt;
    return 0;
}

float Advection::_calcAdjustFactor(const Particle &p2, const Particle &p1,
                                   const Particle &p0) const {
    glm::vec3 p2p1 = p1.location - p2.location;
    glm::vec3 p1p0 = p0.location - p1.location;
    float denominator = glm::length(p2p1) * glm::length(p1p0);
    float cosine;
    if (denominator < 1e-7)
        return 1.0f;
    else
        cosine = glm::dot(p2p1, p1p0) / denominator;

    if (cosine > _lowerAngleCos) // Less than "_lowerAngle" degrees
        return 1.25f;
    else if (cosine < _upperAngleCos) // More than "_upperAngle" degrees
        return 0.5f;
    else
        return 1.0f;
}

int Advection::OutputStreamsGnuplot(const std::string &filename) const {
    FILE *f = std::fopen(filename.c_str(), "w");
    if (f == nullptr)
        return FILE_ERROR;

    std::fprintf(f, "%s\n", "# X      Y      Z");
    for (const auto &s : _streams) {
        for (const auto &p : s)
            std::fprintf(f, "%f, %f, %f\n", p.location.x, p.location.y, p.location.z);
        std::fprintf(f, "\n\n");
    }
    std::fclose(f);

    return 0;
}
