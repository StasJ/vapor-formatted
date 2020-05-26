#pragma once

#include "Flags.h"
#include "PWidget.h"
#include "VSection.h"

namespace VAPoR {
class DataMgr;
class ParamsMgr;
class RenderParams;
} // namespace VAPoR

class VLineComboBox;
class VContainer;
class VFidelitySection;
class PSection;
// class VLineItem;
// class FidelityWidget2;

class VVariablesSection : public VSection {

    Q_OBJECT

  public:
    VVariablesSection();

    void Reinit(VariableFlags varFlags, DimFlags dimFlags);

    int GetActiveDimension() const;
    DimFlags GetDimFlags() const;

    void Configure2DFieldVars();
    void Configure3DFieldVars();

    void Update(VAPoR::RenderParams *rParams, VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr);

  protected:
    VAPoR::RenderParams *_rParams;
    VAPoR::ParamsMgr *_paramsMgr;
    VAPoR::DataMgr *_dataMgr;

  private:
    size_t _activeDim;
    bool _initialized;

    VariableFlags _variableFlags;
    DimFlags _dimFlags;

    VSection *_vSection;
    VLineComboBox *_dimCombo;

    VContainer *_container;

    // We cannot hide PVariableSelector, so use
    // VLineComboBox and signal/slot connections
    VLineComboBox *_scalarCombo;
    VLineComboBox *_xFieldCombo;
    VLineComboBox *_yFieldCombo;
    VLineComboBox *_zFieldCombo;
    VLineComboBox *_colorCombo;
    VLineComboBox *_heightCombo;

    VFidelitySection *_fidelitySection;

    static const std::string _sectionTitle;

  private slots:
    void _dimChanged();
    void _scalarVarChanged(std::string var);
    void _xFieldVarChanged(std::string var);
    void _yFieldVarChanged(std::string var);
    void _zFieldVarChanged(std::string var);
    void _colorVarChanged(std::string var);
    void _heightVarChanged(std::string var);
};
