#pragma once

#include "ParamsWidgets.h"
#include <QTabWidget>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>

class TFOpacityWidget;
class TFColorWidget;
class TFHistogramWidget;
class QRangeSlider;
class TFInfoWidget;

class TFEditor : public QTabWidget {
    Q_OBJECT

  public:
    TFEditor();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    TFOpacityWidget *tff;
    TFHistogramWidget *tfh;
    TFColorWidget *colorWidget;
    QRangeSlider *range;
    ParamsWidgetDropdown *colorMapTypeDropdown;
    TFInfoWidget *controlPointWidget;
    QWidget *_tab() const;
};
