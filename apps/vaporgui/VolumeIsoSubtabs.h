#pragma once

#include "ui_VolumeIsoAnnotationGUI.h"
#include "ui_VolumeIsoAppearanceGUI.h"
#include "ui_VolumeIsoGeometryGUI.h"

#include "TFEditorIsoSurface.h"
#include "vapor/VolumeIsoParams.h"

namespace VAPoR {
class ControlExec;
class RenderParams;
class ParamsMgr;
class DataMgr;
} // namespace VAPoR

class PGroup;

class VolumeIsoAppearanceSubtab : public QWidget, public Ui_VolumeIsoAppearanceGUI {
    Q_OBJECT

  public:
    VolumeIsoAppearanceSubtab(QWidget *parent);

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *params);

  private:
    PGroup *_pg;
};

class VolumeIsoGeometrySubtab : public QWidget, public Ui_VolumeIsoGeometryGUI {
    Q_OBJECT

  public:
    VolumeIsoGeometrySubtab(QWidget *parent) {
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

class VolumeIsoAnnotationSubtab : public QWidget, public Ui_VolumeIsoAnnotationGUI {
  public:
    VolumeIsoAnnotationSubtab(QWidget *parent) { setupUi(this); }

    void Update(VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr,
                VAPoR::RenderParams *rParams) {
        _colorbarWidget->Update(dataMgr, paramsMgr, rParams);
    }
};
