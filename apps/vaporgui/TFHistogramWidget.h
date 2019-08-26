#pragma once

#include "Histo.h"
#include "TFMapWidget.h"
#include <QFrame>
#include <QWidget>
#include <vapor/VAssert.h>

class TFHistogramWidget : public TFMapWidget {
    Q_OBJECT

  public:
    TFHistogramWidget();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

    QSize minimumSizeHint() const;
    void Deactivate() {}

  protected:
    TFInfoWidget *createInfoWidget();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    //    void mouseDoubleClickEvent(QMouseEvent *event);

  private:
    VAPoR::DataMgr *_dataMgr = nullptr;
    VAPoR::RenderParams *_renderParams = nullptr;
    Histo _histo;

  signals:
    void InfoDeselected();
    void UpdateInfo(float value);
};
