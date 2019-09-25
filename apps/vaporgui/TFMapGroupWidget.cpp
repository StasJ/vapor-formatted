#include "TFMapGroupWidget.h"
#include "ErrorReporter.h"
#include "SettingsParams.h"
#include "TFColorInfoWidget.h"
#include "TFColorWidget.h"
#include "TFEditor.h"
#include "TFHistogramWidget.h"
#include "TFIsoValueWidget.h"
#include "TFMappingRangeSelector.h"
#include "TFOpacityInfoWidget.h"
#include "TFOpacityWidget.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <vapor/ColorMap.h>
#include <vapor/FileUtils.h>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>
#include <vapor/ResourcePath.h>

using namespace Wasp;
using namespace VAPoR;

TFMapGroupWidget::TFMapGroupWidget() {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
}

void TFMapGroupWidget::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                              VAPoR::RenderParams *rParams) {
    for (auto map : _maps)
        map->Update(dataMgr, paramsMgr, rParams);
}

TFMapInfoGroupWidget *TFMapGroupWidget::CreateInfoGroup() {
    TFMapInfoGroupWidget *infos = new TFMapInfoGroupWidget;

    for (auto map : _maps) {
        infos->add(map);
    }

    return infos;
}

void TFMapGroupWidget::Add(TFMapWidget *mapWidget) {
    _maps.push_back(mapWidget);
    layout()->addWidget(mapWidget);
    connect(mapWidget, SIGNAL(Activated(TFMapWidget *)), this, SLOT(mapActivated(TFMapWidget *)));
}

void TFMapGroupWidget::Add(TFMap *map) { Add(new TFMapWidget(map)); }

void TFMapGroupWidget::Add(const std::initializer_list<TFMap *> &layeredMaps) {
    assert(layeredMaps.size() > 0);
    if (layeredMaps.size() == 0)
        return;

    TFMapWidget *mapWidget = new TFMapWidget(*layeredMaps.begin());

    for (auto it = layeredMaps.begin() + 1; it != layeredMaps.end(); ++it)
        mapWidget->AddMap(*it);

    Add(mapWidget);
}

void TFMapGroupWidget::mapActivated(TFMapWidget *activatedMap) {
    for (auto map : _maps)
        if (map != activatedMap)
            map->Deactivate();
}

TFMapInfoGroupWidget::TFMapInfoGroupWidget() {}

void TFMapInfoGroupWidget::Update(VAPoR::RenderParams *rParams) {
    for (auto info : _infos)
        info->Update(rParams);
}

void TFMapInfoGroupWidget::add(TFMapWidget *mapWidget) {
    for (TFMap *map : mapWidget->GetMaps()) {
        TFInfoWidget *info = map->GetInfoWidget();
        if (!info)
            continue;
        _infos.push_back(info);
        addWidget(info);
        connect(map, SIGNAL(Activated(TFMap *)), this, SLOT(mapActivated(TFMap *)));
    }
}

void TFMapInfoGroupWidget::mapActivated(TFMap *activatedMap) {
    setCurrentWidget(activatedMap->GetInfoWidget());
}
