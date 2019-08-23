#pragma once

#include "ParamsWidgets.h"
#include <QStackedWidget>
#include <QTabWidget>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>
#include <vector>

class TFOpacityWidget;
class TFColorWidget;
class TFHistogramWidget;
class QRangeSlider;
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
    QRangeSlider *range;
    ParamsWidgetDropdown *colorMapTypeDropdown;
    TFMapsGroup *_maps;
    TFMapsInfoGroup *_mapsInfo;
    QWidget *_tab() const;
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
