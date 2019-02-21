/*
 * A stream that consists of many Particles.
 */

#ifndef STREAM_H
#define STREAM_H

#include "Particle.h"
#include <map>
#include <string>

namespace VAPoR {
namespace flow {
class Stream {
  public:
    std::vector<flow::Particle> particles;

    // A Stream could optionally have multiple properties
    std::map<std::string, std::vector<float>> properties;

    // Constructors and destructor
    Stream();
    ~Stream();
};

}; // namespace flow
}; // namespace VAPoR

#endif
