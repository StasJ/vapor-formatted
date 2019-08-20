#pragma once

#include "ParamsWidgets.h"
#include <QComboBox>
#include <QLineEdit>
#include <QWidget>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>

class TFOpacityWidget;
class TFColorWidget;
class TFHistogramWidget;
class QRangeSlider;

class TFControlPointWidget : public QWidget {
    Q_OBJECT

  public:
    TFControlPointWidget();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  public slots:
    void SelectOpacityControlPoint(int index);
    void SelectColorControlPoint(int index);
    void DeselectControlPoint();

  protected:
    void paintEvent(QPaintEvent *event);
    bool isUsingNormalizedValue() const;

  private:
    QLineEdit *_locationEdit;
    QComboBox *_locationEditType;
    QLineEdit *_valueEdit;

    int _opacityId = -1;
    int _colorId = -1;
};
