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

    // True  == Steady; False == Unteady
    void SetIsSteady(bool steady);
    bool GetIsSteady() const;

    double GetVelocityMultiplier() const;
    void SetVelocityMultiplier(double);

    long GetSteadyNumOfSteps() const;
    void SetSteadyNumOfSteps(long);

    long GetSeedGenMode() const;
    void SetSeedGenMode(long);

    std::string GetSeedInputFilename() const;
    void SetSeedInputFilename(std::string &);

    std::string GetFlowlineOutputFilename() const;
    void SetFlowlineOutputFilename(std::string &);

    long GetFlowDirection() const;
    void SetFlowDirection(long);

  protected:
    static const std::string _isSteadyTag;
    static const std::string _velocityMultiplierTag;
    static const std::string _steadyNumOfStepsTag;
    static const std::string _seedGenModeTag;
    static const std::string _seedInputFilenameTag;
    static const std::string _flowlineOutputFilenameTag;
    static const std::string _flowDirectionTag;
};

} // namespace VAPoR

#endif
