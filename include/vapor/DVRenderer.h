#ifndef DVRENDERER_H
#define DVRENDERER_H

#include "vapor/DVRParams.h"
#include "vapor/RayCaster.h"

namespace VAPoR {

class RENDER_API DVRenderer : public RayCaster {
  public:
    DVRenderer(const ParamsMgr *pm, std::string &winName, std::string &dataSetName,
               std::string &instName, DataMgr *dataMgr);

    static std::string GetClassType() { return ("VolumeRenderer"); }

  protected:
    void _loadShaders();
    void _3rdPassSpecialHandling(bool, long);
};

}; // namespace VAPoR

#endif
