#ifndef ISOSURFACERENDERER_H
#define ISOSURFACERENDERER_H

#include "vapor/IsoSurfaceParams.h"
#include "vapor/RayCaster.h"

namespace VAPoR {

class RENDER_API IsoSurfaceRenderer : public RayCaster {
  public:
    IsoSurfaceRenderer(const ParamsMgr *pm, std::string &winName, std::string &dataSetName,
                       std::string &instName, DataMgr *dataMgr);

    static std::string GetClassType() { return ("IsoSurface"); }

  protected:
    int _load3rdPassShaders();
    void _3rdPassSpecialHandling(bool fast, int castMode);
    void _colormapSpecialHandling(RayCasterParams *params);
    bool _use2ndVariable(const RayCasterParams *params) const;
    void _update2ndVarTextures();
};

}; // namespace VAPoR

#endif
