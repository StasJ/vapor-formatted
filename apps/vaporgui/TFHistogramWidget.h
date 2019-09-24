#pragma once

#include "Histo.h"
#include "TFMapWidget.h"
#include <QFrame>
#include <QWidget>
#include <vapor/VAssert.h>

class TFHistogramMap : public TFMap {
    Q_OBJECT

    enum ScalingType { Linear = 0, Logarithmic, Boolean, ScalingTypeCount };

  public:
    bool DynamicScaling = true;

    TFHistogramMap(TFMapWidget *parent);
    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

    QSize minimumSizeHint() const;
    void Deactivate() {}
    void PopulateSettingsMenu(QMenu *menu) const;

  protected:
    TFInfoWidget *createInfoWidget();
    void paintEvent(QPainter &p);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    //    void mouseDoubleClickEvent(QMouseEvent *event);

  private:
    VAPoR::DataMgr *_dataMgr = nullptr;
    VAPoR::RenderParams *_renderParams = nullptr;
    Histo _histo;
    QAction *_scalingActions[ScalingTypeCount];

    ScalingType _getScalingType() const;

  private slots:
    void _setScalingTypeAction();

  signals:
    void InfoDeselected();
    void UpdateInfo(float value);
};

class TFHistogramWidget : public TFMapWidget {
  public:
    TFHistogramWidget() : TFMapWidget(new TFHistogramMap(this)) {}
};
