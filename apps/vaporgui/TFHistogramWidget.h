#pragma once

#include "Histo.h"
#include "ParamsMenuItems.h"
#include "TFMapWidget.h"
#include <QFrame>
#include <QWidget>
#include <vapor/VAssert.h>

class TFHistogramMap : public TFMap {
    Q_OBJECT

    enum ScalingType { Linear = 0, Logarithmic, Boolean, ScalingTypeCount };

  public:
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
    ParamsDropdownMenuItem *_scalingMenu;
    bool _dynamicScaling = true;

    ScalingType _getScalingType() const;

  private slots:
    void _menuDynamicScalingToggled(bool on);

  signals:
    void InfoDeselected();
    void UpdateInfo(float value);
};

class TFHistogramWidget : public TFMapWidget {
  public:
    TFHistogramWidget() : TFMapWidget(new TFHistogramMap(this)) {}
};
