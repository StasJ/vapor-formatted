#include <sstream>

#include "vapor/RenderParams.h"

// I don't understand why we need to include PWidget.h to use HLI widgets.
// Comment the line below, and many errors arise that are not clear to me.
// -Scott
//#include "PWidget.h"

#include "PGroup.h"

#include "VFidelitySection.h" // This is where VFidelityButtons is defined
#include "VFidelitySection_PW.h"

const std::string VFidelitySection_PW::_sectionTitle = "Data Fidelity";

VFidelitySection_PW::VFidelitySection_PW() : VSection(_sectionTitle) {
    _fidelityButtons = new VFidelityButtons();
    layout()->addWidget(_fidelityButtons);

    _pGroup = new PGroup();
    layout()->addWidget(_pGroup);

    _plodHLI = new PLODSelectorHLI<VAPoR::RenderParams>(&VAPoR::RenderParams::GetCompressionLevel,
                                                        &VAPoR::RenderParams::SetCompressionLevel);
    // layout()->addWidget( _plodHLI );
    _pGroup->Add(_plodHLI);

    _pRefHLI = new PRefinementSelectorHLI<VAPoR::RenderParams>(
        &VAPoR::RenderParams::GetRefinementLevel, &VAPoR::RenderParams::SetRefinementLevel);
    // layout()->addWidget( _pRefHLI );
    _pGroup->Add(_pRefHLI);
}

void VFidelitySection_PW::Reinit(VariableFlags varFlags) {
    _variableFlags = varFlags;

    _fidelityButtons->Reinit(_variableFlags);
    _plodHLI->Reinit(_variableFlags);
    _pRefHLI->Reinit(_variableFlags);
}

void VFidelitySection_PW::Update(VAPoR::RenderParams *rParams, VAPoR::ParamsMgr *paramsMgr,
                                 VAPoR::DataMgr *dataMgr) {
    VAssert(dataMgr);
    VAssert(paramsMgr);
    VAssert(rParams);

    _dataMgr = dataMgr;
    _paramsMgr = paramsMgr;
    _rParams = rParams;

    _pGroup->Update(_rParams, _paramsMgr, _dataMgr);
    //_plodHLI->Update( _rParams, _paramsMgr, _dataMgr );
    //_pRefHLI->Update( _rParams, _paramsMgr, _dataMgr );

    _fidelityButtons->Update(_rParams, _paramsMgr, _dataMgr);
}

void VFidelitySection_PW::setNumRefinements(int num) { _rParams->SetRefinementLevel(num); }

void VFidelitySection_PW::setCompRatio(int num) { _rParams->SetCompressionLevel(num); }

std::string VFidelitySection_PW::GetCurrentLodString() const { return _currentLodStr; }

std::string VFidelitySection_PW::GetCurrentMultiresString() const { return _currentMultiresStr; }
