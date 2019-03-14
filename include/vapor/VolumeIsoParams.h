#pragma once

#include <vapor/DataMgr.h>
#include <vapor/VolumeParams.h>

namespace VAPoR {

class PARAMS_API VolumeIsoParams : public VolumeParams {
  public:
    VolumeIsoParams(DataMgr *dataMgr, ParamsBase::StateSave *ssave);
    VolumeIsoParams(DataMgr *dataMgr, ParamsBase::StateSave *ssave, XmlNode *node);
    virtual ~VolumeIsoParams();

    static string GetClassType() { return ("VolumeIsoParams"); }

    virtual string GetDefaultAlgorithmName() const;

  private:
    void _init();
};

}; // namespace VAPoR
