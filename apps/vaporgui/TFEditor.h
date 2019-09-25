#pragma once

#include "ParamsWidgets.h"
#include "VSection.h"
#include <QStackedWidget>
#include <QTabWidget>
#include <vector>

#include <QToolButton>

class TFOpacityWidget;
class TFColorWidget;
class TFHistogramMap;
class TFHistogramWidget;
class QRangeSlider;
class QRangeSliderTextCombo;
class TFInfoWidget;
class TFMapsGroup;
class TFMapWidget;
class TFMap;
class TFMapsInfoGroup;
class TFIsoValueWidget;
class TFMappingRangeSelector;

namespace VAPoR {
class DataMgr;
class ParamsMgr;
class RenderParams;
} // namespace VAPoR

class TFEditor : public VSection {
    Q_OBJECT

  public:
    TFEditor();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    VAPoR::RenderParams *_rParams = nullptr;
    VAPoR::ParamsMgr *_paramsMgr = nullptr;
    TFMappingRangeSelector *range;
    TFMapsGroup *_maps;
    TFMapsInfoGroup *_mapsInfo;
};
