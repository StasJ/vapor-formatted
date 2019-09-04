#pragma once

#include "ParamsWidgets.h"
#include <QStackedWidget>
#include <QTabWidget>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>
#include <vector>

#include <QToolButton>

class TFOpacityWidget;
class TFColorWidget;
class TFHistogramMap;
class QRangeSlider;
class QRangeSliderTextCombo;
class TFInfoWidget;
class TFMapsGroup;
class TFMapWidget;
class TFMapsInfoGroup;

class TFEditor : public QTabWidget {
    Q_OBJECT

  public:
    TFEditor();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    VAPoR::RenderParams *_rParams;
    QRangeSliderTextCombo *range;
    ParamsWidgetDropdown *colorMapTypeDropdown;
    TFMapsGroup *_maps;
    TFMapsInfoGroup *_mapsInfo;
    QToolButton *_tool;

    QWidget *_tab() const;
    void _updateMappingRangeControl(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                                    VAPoR::RenderParams *rParams);
    void _getDataRange(VAPoR::DataMgr *dataMgr, VAPoR::RenderParams *rParams, float *min,
                       float *max) const;
    QString _createStylesheet() const;

  private slots:
    void _rangeChanged(float left, float right);
};

class SettingsMenu : public QToolButton {
    Q_OBJECT

  public:
    SettingsMenu();

  protected:
    void paintEvent(QPaintEvent *event);
};

class TFMapsGroup : public QWidget {
    Q_OBJECT

    std::vector<TFMapWidget *> _maps;

  public:
    TFMapsGroup();
    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

    TFMapsInfoGroup *CreateInfoGroup();

  private:
    void add(TFMapWidget *map);

  private slots:
    void mapActivated(TFMapWidget *map);
};

class TFMapsInfoGroup : public QStackedWidget {
    Q_OBJECT

    std::vector<TFInfoWidget *> _infos;

  public:
    TFMapsInfoGroup();
    void Update(VAPoR::RenderParams *rParams);

    friend class TFMapsGroup;

  private:
    void add(TFMapWidget *map);

  private slots:
    void mapActivated(TFMapWidget *map);
};
