#pragma once

#include "Flags.h"
#include "ui_VolumeAnnotationGUI.h"
#include "ui_VolumeAppearanceGUI.h"
#include "ui_VolumeGeometryGUI.h"
#include "ui_VolumeVariablesGUI.h"
#include <vapor/MapperFunction.h>
#include <vapor/VolumeParams.h>

namespace VAPoR {
class ControlExec;
class RenderParams;
class ParamsMgr;
class DataMgr;
} // namespace VAPoR

class TFEditorVolume;
class QSliderEdit;
class PGroup;

#include "PCheckbox.h"
#include "PDoubleInput.h"
#include "PIntegerInput.h"
#include "PSection.h"
#include "PSliderEdit.h"

class VolumeVariablesSubtab : public QWidget, public Ui_VolumeVariablesGUI {

    Q_OBJECT

  public:
    VolumeVariablesSubtab(QWidget *parent) {
        setupUi(this);
        _variablesWidget->Reinit((VariableFlags)(SCALAR | COLOR), (DimFlags)(THREED));

        _ps = new PSection("OSPray");
        ((QVBoxLayout *)layout())->insertWidget(1, _ps);
    }

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private slots:

  private:
    VAPoR::VolumeParams *_volumeParams;
    PSection *_ps;
    PWidget *_fov;
};

class VolumeAppearanceSubtab : public QWidget, public Ui_VolumeAppearanceGUI {
    Q_OBJECT

  public:
    VolumeAppearanceSubtab(QWidget *parent);
    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    PGroup *_pg;
};

class VolumeGeometrySubtab : public QWidget, public Ui_VolumeGeometryGUI {

    Q_OBJECT

  public:
    VolumeGeometrySubtab(QWidget *parent) {
        setupUi(this);
        _geometryWidget->Reinit((DimFlags)THREED, (VariableFlags)SCALAR);
    }

    void Update(VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr,
                VAPoR::RenderParams *rParams) {
        _geometryWidget->Update(paramsMgr, dataMgr, rParams);
        _copyRegionWidget->Update(paramsMgr, rParams);
        _transformTable->Update(rParams->GetTransform());
    }
};

class VolumeAnnotationSubtab : public QWidget, public Ui_VolumeAnnotationGUI {

    Q_OBJECT

  public:
    VolumeAnnotationSubtab(QWidget *parent) { setupUi(this); }

    void Update(VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr,
                VAPoR::RenderParams *rParams) {
        _colorbarWidget->Update(dataMgr, paramsMgr, rParams);
    }
};
