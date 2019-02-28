
#include <string>
#include <vapor/STLUtils.h>
#include <vapor/VolumeParams.h>

using namespace Wasp;
using namespace VAPoR;

//
// Register class with object factory!!!
//
static RenParamsRegistrar<VolumeParams> registrar(VolumeParams::GetClassType());

std::vector<std::string> VolumeParams::_algorithmNames;

const std::string VolumeParams::_algorithmTag = "AlgorithmTag";
const std::string VolumeParams::_isoValueTag = "IsoValueTag";

VolumeParams::VolumeParams(DataMgr *dataMgr, ParamsBase::StateSave *ssave)
    : RenderParams(dataMgr, ssave, VolumeParams::GetClassType(), 3) {
    SetDiagMsg("VolumeParams::VolumeParams() this=%p", this);

    _init();
}

VolumeParams::VolumeParams(DataMgr *dataMgr, ParamsBase::StateSave *ssave, XmlNode *node)
    : RenderParams(dataMgr, ssave, node, 3) {
    SetDiagMsg("VolumeParams::VolumeParams() this=%p", this);

    // If node isn't tagged correctly we correct the tag and reinitialize
    // from scratch;
    //
    if (node->GetTag() != VolumeParams::GetClassType()) {
        node->SetTag(VolumeParams::GetClassType());
        _init();
    }
}

VolumeParams::~VolumeParams() { SetDiagMsg("VolumeParams::~VolumeParams() this=%p", this); }

bool VolumeParams::IsOpaque() const { return true; }

bool VolumeParams::usingVariable(const std::string &varname) {
    return (varname.compare(GetVariableName()) == 0);
}

std::string VolumeParams::GetAlgorithm() const { return GetValueString(_algorithmTag, "Regular"); }

void VolumeParams::SetAlgorithm(std::string algorithm) {
    assert(STLUtils::Contains(GetAlgorithmNames(), algorithm));
    SetValueString(_algorithmTag, "Volume rendering algorithm", algorithm);
}

double VolumeParams::GetIsoValue() const { return GetValueDouble(_isoValueTag, 0); }

void VolumeParams::SetIsoValue(double isoValue) {
    SetValueDouble(_isoValueTag, "Iso surface value", isoValue);
}

const std::vector<std::string> VolumeParams::GetAlgorithmNames() { return _algorithmNames; }

void VolumeParams::Register(const std::string &name) {
    assert(!STLUtils::Contains(_algorithmNames, name));
    _algorithmNames.push_back(name);
}

// Set everything to default values
void VolumeParams::_init() { SetDiagMsg("VolumeParams::_init()"); }
