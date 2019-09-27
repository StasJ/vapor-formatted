#ifndef VAPORFIELD_H
#define VAPORFIELD_H

#include "vapor/DataMgr.h"
#include "vapor/Field.h"
#include "vapor/FlowParams.h"
#include "vapor/Grid.h"
#include "vapor/Particle.h"
#include <list>

namespace flow {
class VaporField : public Field {
  public:
    VaporField();
    VaporField(int recentGridLimit);
    virtual ~VaporField();

    //
    // Functions from class Field
    //
    virtual bool InsideVolumeVelocity(float time, const glm::vec3 &pos);
    virtual bool InsideVolumeScalar(float time, const glm::vec3 &pos);
    virtual int GetVelocity(float time, const glm::vec3 &pos, // input
                            glm::vec3 &vel,                   // output
                            bool checkInsideVolume = true);
    virtual int GetScalar(float time, const glm::vec3 &pos, // input
                          float &val,                       // output
                          bool checkInsideVolume = true);
    virtual int GetNumberOfTimesteps();

    //
    // Functions for interaction with VAPOR components
    //
    void AssignDataManager(VAPoR::DataMgr *dmgr);
    void UpdateParams(const VAPoR::FlowParams *p);

    //
    // Find one index whose timestamp is just below a given time
    // I.e., _timestamps[floor] <= time
    //
    int LocateTimestamp(float time,           // Input
                        size_t &floor) const; // Output

    //
    // This class keeps a pointer to a vapor grid, and all the metadata
    // associated with the grid.
    //
    struct RichGrid {
        const VAPoR::Grid *realGrid;
        const size_t TS;
        const std::string varName;
        const int refinementLevel, compressionLevel;
        const std::vector<double> extMin, extMax;
        VAPoR::DataMgr *mgr; // for unlocking realGrid

        // RichGrid();
        RichGrid(const VAPoR::Grid *g, size_t currentTS, const std::string &var, int refLevel,
                 int compLevel, const std::vector<double> &min, const std::vector<double> &max,
                 VAPoR::DataMgr *dm);
        ~RichGrid();
        bool equals(size_t currentTS, const std::string &var, int refLevel, int compLevel,
                    const std::vector<double> &min, const std::vector<double> &max) const;
    };

    //
    // This wrapper class wraps a grid and a data manager pointer to ensure
    // the grid is properly destroyed.
    //
    class GridWrapper {
      private:
        const VAPoR::Grid *gridPtr;
        VAPoR::DataMgr *mgr;

      public:
        GridWrapper(const VAPoR::Grid *gp, VAPoR::DataMgr *mp) : gridPtr(gp), mgr(mp) {}
        // Rule of 3
        GridWrapper(const GridWrapper &) = delete;
        GridWrapper &operator=(const GridWrapper &) = delete;
        ~GridWrapper() {
            if (mgr && gridPtr) {
                mgr->UnlockGrid(gridPtr);
                delete gridPtr;
            }
        }
    };

    //
    // Returns the intersection domain of 3 velocity variables
    //
    void GetFirstStepVelocityIntersection(glm::vec3 &minxyz, glm::vec3 &maxxyz);

  protected:
    // Member variables
    std::vector<float> _timestamps; // in ascending order
    VAPoR::DataMgr *_datamgr;
    const VAPoR::FlowParams *_params;

    // Keep copies of recent grids.
    const int _recentGridLimit;
    std::list<RichGrid> _recentGrids;

    // Member functions
    template <typename T>
    size_t _binarySearch(const std::vector<T> &vec, T val, size_t begin, size_t end) const;

    std::string _paramsToString(size_t currentTS, const std::string &var, int refLevel,
                                int compLevel, const std::vector<double> &min,
                                const std::vector<double> &max) const;

    // Are the following member variables pointers set?
    //  1) _datamgr and 2) _params
    bool _isReady() const;

    // _getAGrid will use _params to retrieve/generate grids.
    int _getAGrid(size_t timestep,             // Input
                  std::string &varName,        // Input
                  const VAPoR::Grid **gridpp); // Output
};
}; // namespace flow

#endif
