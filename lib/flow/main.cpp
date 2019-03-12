#include <cstdlib>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <queue>
#include <stack>
#include <vector>

#include "vapor/Advection.h"
#include "vapor/OceanField.h"
#include "vapor/Particle.h"

using namespace flow;

int main(int argc, char **argv) {
    /*
        int numOfSeeds = 5;
        std::vector<Particle> seeds( numOfSeeds );

        seeds[0].location = glm::vec3( 0.65f, 0.65f, 0.1f );
        seeds[1].location = glm::vec3( 0.3f, 0.3f, 0.1f );
        for( int i = 2; i < numOfSeeds; i++ )
            seeds[i].location = glm::vec3( float(i + 1) / float(numOfSeeds + 1), 0.0f, 0.0f );

        OceanField    f;
        Advection     a;
        a.SetBaseStepSize( 0.5f );

        a.UseField( &f );
        a.UseSeedParticles( seeds );

        int steps = std::atoi( argv[1] );
        for( int i = 0; i < steps; i++ )
            a.Advect( flow::Advection::RK4 );

        std::string filename( "streams.dat" );
        a.OutputStreamsGnuplot( filename );
    */

    Particle p;
    int numOfProp = 10;
    for (int i = 0; i < numOfProp; i++)
        p.AttachProperty(float(i));

    for (int i = 0; i < p.GetNumOfProperties(); i++) {
        std::cout << p.RetrieveProperty(i) << std::endl;
    }
}
