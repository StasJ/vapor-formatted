
#include <string>
#include <vapor/STLUtils.h>
#include <vapor/VolumeIsoParams.h>

using namespace Wasp;
using namespace VAPoR;

//
// Register class with object factory!!!
//
static RenParamsRegistrar<VolumeIsoParams> registrar(VolumeIsoParams::GetClassType());

VolumeIsoParams::VolumeIsoParams(DataMgr *dataMgr, ParamsBase::StateSave *ssave)
    : VolumeParams(dataMgr, ssave, VolumeIsoParams::GetClassType()) {
    SetDiagMsg("VolumeIsoParams::VolumeIsoParams() this=%p", this);
    _init();
}

VolumeIsoParams::VolumeIsoParams(DataMgr *dataMgr, ParamsBase::StateSave *ssave, XmlNode *node)
    : VolumeParams(dataMgr, ssave, node) {
    SetDiagMsg("VolumeIsoParams::VolumeIsoParams() this=%p", this);

    // If node isn't tagged correctly we correct the tag and reinitialize
    // from scratch;
    //
    if (node->GetTag() != VolumeIsoParams::GetClassType()) {
        node->SetTag(VolumeIsoParams::GetClassType());
        _init();
    }
}

VolumeIsoParams::~VolumeIsoParams() {
    SetDiagMsg("VolumeIsoParams::~VolumeIsoParams() this=%p", this);
}

string VolumeIsoParams::GetDefaultAlgorithmName() const { return "Iso Regular"; }

// Set everything to default values
void VolumeIsoParams::_init() {
    SetDiagMsg("VolumeParams::_init()");
    SetValueLong(UseColormapVariableTag, UseColormapVariableTag, false);

    SetFieldVariableNames(vector<string>());
}
