#pragma once

#include "Flags.h"
#include "VLineItem.h"
#include "VSection.h"
#include "vapor/MyBase.h"
#include <QObject>

QT_USE_NAMESPACE

class QGroupBox;
class VLineComboBox;
class VFidelityButtons;

//! class VFidelitySection
//! A VSection that encapsulates VWidgets (VFidelityButtons, and a
//! VLineComboBox for the LOD and Refinement selectors.

class VFidelitySection : public VSection {
    Q_OBJECT

  public:
    VFidelitySection();

    void Reinit(VariableFlags variableFlags);

    void Update(VAPoR::RenderParams *rParams, VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr);

    std::string GetCurrentLodString() const;
    std::string GetCurrentMultiresString() const;

  private:
    VAPoR::DataMgr *_dataMgr;
    VAPoR::ParamsMgr *_paramsMgr;
    VAPoR::RenderParams *_rParams;

    static const std::string _sectionTitle;

    VariableFlags _variableFlags;

    VFidelityButtons *_fidelityButtons;
    VLineComboBox *_lodCombo;
    VLineComboBox *_refCombo;

    std::vector<string> _fidelityLodStrs;
    std::vector<string> _fidelityMultiresStrs;

  private slots:
    void setNumRefinements(int num);
    void setCompRatio(int num);
};

//! class VFidelityButtons
//! A VLineItem that holds a label on the left, and a QButtonGroup
//! on the right.  The QButtonGroup holds a set of radio buttons that
//! each map to a pair of LOD and Refinement values, allowing the user
//! to linearly increase the quality of their rendering.
class VFidelityButtons : public VLineItem {

    Q_OBJECT

  public:
    VFidelityButtons();

    void Reinit(VariableFlags variableFlags);

    virtual void Update(VAPoR::RenderParams *params, VAPoR::ParamsMgr *paramsMgr,
                        VAPoR::DataMgr *dataMgr);

  protected slots:
    void setFidelity(int buttonID);

  private:
    VAPoR::DataMgr *_dataMgr;
    VAPoR::ParamsMgr *_paramsMgr;
    VAPoR::RenderParams *_rParams;

    VariableFlags _variableFlags;

    QButtonGroup *_fidelityButtons;
    QGroupBox *_fidelityBox;
    QFrame *_fidelityFrame;

    VLineComboBox *_lodCombo;
    VLineComboBox *_refCombo;

    void setupFidelity(VAPoR::RenderParams params);
    void uncheckFidelity();

    std::vector<std::string> _fidelityLodStrs;
    std::vector<std::string> _fidelityMultiresStrs;
    std::vector<int> _fidelityLodIdx;
    std::vector<int> _fidelityMultiresIdx;
};
