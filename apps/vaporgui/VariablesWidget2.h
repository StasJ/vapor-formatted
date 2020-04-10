#pragma once

#include "Flags.h"
#include "PVariableSelector.h"
#include "PVariableSelectorHLI.h"
#include "VSection.h"

class PStringDowndownHLI;

namespace VAPoR {
class DataMgr;
class ParamsMgr;
class RenderParams;
} // namespace VAPoR

class VariablesWidget2 : public VSection {
    Q_OBJECT

  public:
    VariablesWidget2();

    void Reinit(VariableFlags varFlags, DimFlags dimFlags);

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    PStringDropdown *_dimSelector;

    PVariableSelector *_scalarVar;
    PFieldVariableSelector _fieldVars;
    PVariableSelector *_colorVar;
    PVariableSelector *_heightVar;
};
