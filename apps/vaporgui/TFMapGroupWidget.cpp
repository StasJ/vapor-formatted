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

TFMapsGroup::TFMapsGroup() {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);

    TFMapWidget *o;
    add(o = new TFOpacityWidget);
    histo = new TFHistogramMap(o);
    o->AddMap(histo);
    add(new TFIsoValueWidget);
    add(new TFColorWidget);
}

void TFMapsGroup::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                         VAPoR::RenderParams *rParams) {
    for (auto map : _maps)
        map->Update(dataMgr, paramsMgr, rParams);
}

TFMapsInfoGroup *TFMapsGroup::CreateInfoGroup() {
    TFMapsInfoGroup *infos = new TFMapsInfoGroup;

    for (auto map : _maps) {
        infos->add(map);
    }

    return infos;
}

void TFMapsGroup::add(TFMapWidget *map) {
    _maps.push_back(map);
    layout()->addWidget(map);
    connect(map, SIGNAL(Activated(TFMapWidget *)), this, SLOT(mapActivated(TFMapWidget *)));
}

void TFMapsGroup::mapActivated(TFMapWidget *activatedMap) {
    for (auto map : _maps)
        if (map != activatedMap)
            map->Deactivate();
}

TFMapsInfoGroup::TFMapsInfoGroup() {}

void TFMapsInfoGroup::Update(VAPoR::RenderParams *rParams) {
    for (auto info : _infos)
        info->Update(rParams);
}

void TFMapsInfoGroup::add(TFMapWidget *mapWidget) {
    for (TFMap *map : mapWidget->GetMaps()) {
        TFInfoWidget *info = map->GetInfoWidget();
        if (!info)
            continue;
        _infos.push_back(info);
        addWidget(info);
        connect(map, SIGNAL(Activated(TFMap *)), this, SLOT(mapActivated(TFMap *)));
    }
}

void TFMapsInfoGroup::mapActivated(TFMap *activatedMap) {
    setCurrentWidget(activatedMap->GetInfoWidget());
}
