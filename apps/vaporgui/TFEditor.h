#pragma once

#include "ParamsWidgets.h"
#include "VSection.h"
#include <QStackedWidget>
#include <QTabWidget>
#include <vector>

#include <QToolButton>

class TFOpacityWidget;
class TFColorWidget;
class TFOpacityMap;
class TFColorMap;
class TFHistogramMap;
class TFHistogramWidget;
class QRangeSlider;
class QRangeSliderTextCombo;
class TFInfoWidget;
class TFMapGroupWidget;
class TFMapWidget;
class TFMap;
class TFMapInfoGroupWidget;
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
    TFMapGroupWidget *_maps;
    TFMapInfoGroupWidget *_mapsInfo;

    TFOpacityMap *_opacityMap;
    TFHistogramMap *_histogramMap;
    TFColorMap *_colorMap;
};
