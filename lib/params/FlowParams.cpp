#include "vapor/FlowParams.h"

using namespace VAPoR;

const std::string FlowParams::_isSteadyTag = "isSteadyTag";
const std::string FlowParams::_velocityMultiplierTag = "velocityMultiplierTag";
const std::string FlowParams::_steadyNumOfStepsTag = "steadyNumOfStepsTag";
const std::string FlowParams::_seedGenModeTag = "seedGenModeTag";
const std::string FlowParams::_seedInputFilenameTag = "seedInputFilenameTag";
const std::string FlowParams::_flowlineOutputFilenameTag = "flowlineOutputFilenameTag";
const std::string FlowParams::_flowDirectionTag = "flowDirectionTag";
const std::string FlowParams::_needFlowlineOutputTag = "needFlowlineOutputTag";
const std::string FlowParams::_periodicTag = "periodicTag";
const std::string FlowParams::_rakeTag = "rakeTag";

static RenParamsRegistrar<FlowParams> registrar(FlowParams::GetClassType());

// Constructor
FlowParams::FlowParams(DataMgr *dataManager, ParamsBase::StateSave *stateSave)
    : RenderParams(dataManager, stateSave, FlowParams::GetClassType(), 3 /* max dim */) {
    SetDiagMsg("FlowParams::FlowParams() this=%p", this);
}

FlowParams::FlowParams(DataMgr *dataManager, ParamsBase::StateSave *stateSave, XmlNode *node)
    : RenderParams(dataManager, stateSave, node, 3 /* max dim */) {
    SetDiagMsg("FlowParams::FlowParams() this=%p", this);
}

// Destructor
FlowParams::~FlowParams() { SetDiagMsg("FlowParams::~FlowParams() this=%p", this); }

void FlowParams::SetIsSteady(bool steady) {
    SetValueLong(_isSteadyTag, "are we using steady advection", long(steady));
}

bool FlowParams::GetIsSteady() const {
    long rv = GetValueLong(_isSteadyTag, long(true));
    return bool(rv);
}

void FlowParams::SetNeedFlowlineOutput(bool need) {
    SetValueLong(_needFlowlineOutputTag, "need to do an output of the flow lines", long(need));
}

bool FlowParams::GetNeedFlowlineOutput() const {
    long rv = GetValueLong(_needFlowlineOutputTag, long(false));
    return bool(rv);
}

double FlowParams::GetVelocityMultiplier() const {
    return GetValueDouble(_velocityMultiplierTag, 0.01);
}

void FlowParams::SetVelocityMultiplier(double coeff) {
    SetValueDouble(_velocityMultiplierTag, "velocity multiplier", coeff);
}

long FlowParams::GetSteadyNumOfSteps() const { return GetValueLong(_steadyNumOfStepsTag, 100); }

void FlowParams::SetSteadyNumOfSteps(long i) {
    SetValueLong(_steadyNumOfStepsTag, "num of steps for a steady integration", i);
}

long FlowParams::GetSeedGenMode() const { return GetValueLong(_seedGenModeTag, 0); }

void FlowParams::SetSeedGenMode(long i) {
    SetValueLong(_seedGenModeTag, "which mode do we use to generate seeds", i);
}

std::string FlowParams::GetSeedInputFilename() const {
    return GetValueString(_seedInputFilenameTag, "");
}

void FlowParams::SetSeedInputFilename(const std::string &name) {
    SetValueString(_seedInputFilenameTag, "filename for input seeding list", name);
}

std::string FlowParams::GetFlowlineOutputFilename() const {
    return GetValueString(_flowlineOutputFilenameTag, "");
}

void FlowParams::SetFlowlineOutputFilename(const std::string &name) {
    SetValueString(_flowlineOutputFilenameTag, "filename for output flow lines", name);
}

long FlowParams::GetFlowDirection() const { return GetValueLong(_flowDirectionTag, 0); }

void FlowParams::SetFlowDirection(long i) {
    SetValueLong(_flowDirectionTag, "does flow integration go forward, backward, or bi-directional",
                 i);
}

std::vector<bool> FlowParams::GetPeriodic() const {
    std::vector<long> tmp(3, 0);
    auto longs = GetValueLongVec(_periodicTag, tmp);
    std::vector<bool> bools(3, false);
    for (int i = 0; i < 3; i++)
        if (longs[i] != 0)
            bools[i] = true;

    return bools;
}

void FlowParams::SetPeriodic(const std::vector<bool> &bools) {
    std::vector<long> longs(3, 0);
    for (int i = 0; i < 3; i++)
        if (bools[i])
            longs[i] = 1;

    SetValueLongVec(_periodicTag, "any axis is periodic", longs);
}

std::vector<float> FlowParams::GetRake() const {
    const long rakeSize = 6;
    std::vector<double> tmp(6, std::nan("1"));
    auto doubles = GetValueDoubleVec(_rakeTag, tmp);
    VAssert(doubles.size() == rakeSize);

    std::vector<float> floats(rakeSize, 0.0f);
    if (std::isnan(doubles[0]))
        floats[0] = std::nan("1");
    else {
        for (int i = 0; i < rakeSize; i++)
            floats[i] = float(doubles[i]);
    }
    return floats;
}

void FlowParams::SetRake(const std::vector<float> &rake) {
    const long rakeSize = 6;
    VAssert(rake.size() == rakeSize);
    std::vector<double> doubles(rakeSize, 0.0);
    for (int i = 0; i < rakeSize; i++)
        doubles[i] = rake[i];

    SetValueDoubleVec(_rakeTag, "rake boundaries", doubles);
}
