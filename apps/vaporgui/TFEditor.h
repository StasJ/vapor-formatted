#pragma once

#include <QTabWidget>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>

class TFFunctionEditor;
class TFColorWidget;
class QRangeSlider;

class TFEditor : public QTabWidget {
    Q_OBJECT

  public:
    TFEditor();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    TFFunctionEditor *tff;
    TFColorWidget *colorWidget;
    QRangeSlider *range;
    QWidget *_tab() const;
};
