#ifndef FLOWPARAMS_H
#define FLOWPARAMS_H

#include <vapor/DataMgr.h>
#include <vapor/RenderParams.h>

namespace VAPoR {

class PARAMS_API FlowParams : public RenderParams {
  public:
    FlowParams(DataMgr *dataManager, ParamsBase::StateSave *stateSave);
    FlowParams(DataMgr *dataManager, ParamsBase::StateSave *stateSave, XmlNode *xmlNode);

    virtual ~FlowParams();

    //
    // (Pure virtual methods from RenderParams)
    //
    virtual bool IsOpaque() const override { return false; }
    virtual bool usingVariable(const std::string &varname) override { return false; }

    static std::string GetClassType() { return ("FlowParams"); }

    void SetVelocityVarNameU(std::string &);
    void SetVelocityVarNameV(std::string &);
    void SetVelocityVarNameW(std::string &);

    std::string GetVelocityVarNameU() const;
    std::string GetVelocityVarNameV() const;
    std::string GetVelocityVarNameW() const;

  protected:
    static const std::string _velocityUTag;
    static const std::string _velocityVTag;
    static const std::string _velocityWTag;
};

} // namespace VAPoR

#endif
