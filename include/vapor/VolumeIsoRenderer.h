#pragma once

#include <glm/fwd.hpp>
#include <vapor/VolumeAlgorithm.h>
#include <vapor/VolumeRenderer.h>

using std::string;
using std::vector;

namespace VAPoR {

class RENDER_API VolumeIsoRenderer : public VolumeRenderer {
  public:
    VolumeIsoRenderer(const ParamsMgr *pm, std::string &winName, std::string &dataSetName,
                      std::string &instName, DataMgr *dataMgr);
    ~VolumeIsoRenderer();

    static std::string GetClassType() { return ("NEW_IsoSurface"); }

    bool _usingColorMapData() const;
};

}; // namespace VAPoR
