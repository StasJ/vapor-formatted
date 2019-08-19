#pragma once

#include "ParamsWidgets.h"
#include <QTabWidget>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>

class TFFunctionEditor;
class TFColorWidget;
class TFHistogramWidget;
class QRangeSlider;

class TFEditor : public QTabWidget {
    Q_OBJECT

  public:
    TFEditor();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    TFFunctionEditor *tff;
    TFHistogramWidget *tfh;
    TFColorWidget *colorWidget;
    QRangeSlider *range;
    ParamsWidgetDropdown *colorMapTypeDropdown;
    QWidget *_tab() const;
};
