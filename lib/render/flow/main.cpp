#include <cstdlib>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <queue>
#include <stack>
#include <vector>

#include "Advection.h"
#include "OceanField.h"
#include "Particle.h"

using namespace flow;

int main(int argc, char **argv) {
    int numOfSeeds = 5;
    std::vector<Particle> seeds(numOfSeeds);

    seeds[0].location = glm::vec3(0.65f, 0.65f, 0.1f);
    seeds[1].location = glm::vec3(0.3f, 0.3f, 0.1f);
    for (int i = 2; i < numOfSeeds; i++)
        seeds[i].location = glm::vec3(float(i + 1) / float(numOfSeeds + 1), 0.0f, 0.0f);

    OceanField f;
    Advection a;

    a.UseVelocityField(&f);
    a.UseSeedParticles(seeds);

    int steps = std::atoi(argv[1]);
    for (int i = 0; i < steps; i++)
        a.Advect(0.02f, flow::Advection::RK4);

    std::string filename("streams.dat");
    a.OutputStreamsGnuplot(filename);
}
