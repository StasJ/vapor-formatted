#ifndef RAYCASTERPARAMS_H
#define RAYCASTERPARAMS_H

#include <vapor/DataMgr.h>
#include <vapor/RenderParams.h>

namespace VAPoR {

class PARAMS_API RayCasterParams : public RenderParams {
  public:
    RayCasterParams(DataMgr *dataManager, ParamsBase::StateSave *stateSave, std::string classType);
    RayCasterParams(DataMgr *dataManager, ParamsBase::StateSave *stateSave, XmlNode *xmlNode);

    virtual ~RayCasterParams();

    //
    // (Pure virtual methods from RenderParams)
    //
    virtual bool IsOpaque() const override { return false; }
    virtual bool usingVariable(const std::string &varname) override {
        return false; // since this class is for an image, not rendering a variable.
    }

    //
    //! Obtain current MapperFunction for the primary variable.
    //
    MapperFunction *GetMapperFunc();

    bool GetLighting() const;
    void SetLighting(bool);
    std::vector<double> GetLightingCoeffs() const;
    void SetLightingCoeffs(const std::vector<double> &coeffs);
    //
    // Different ray casting methods: 1 == fixed step casting
    //                                2 == prism intersection casting
    //
    long GetCastingMode() const;
    void SetCastingMode(long);
    long GetSampleMultiplier() const;   // ComboBox index is held here. Need to translate
    void SetSampleRateMultiplier(long); //   to real multipliers in RayCaster.cpp

  protected:
    static const std::string _lightingTag;
    static const std::string _lightingCoeffsTag;
    static const std::string _castingModeTag;
    static const std::string _sampleMultiplierTag;
};

} // namespace VAPoR

#endif
