#include "TFEditor.h"
#include "ErrorReporter.h"
#include "QRangeSlider.h"
#include "QRangeSliderTextCombo.h"
#include "SettingsParams.h"
#include "TFColorInfoWidget.h"
#include "TFColorWidget.h"
#include "TFHistogramWidget.h"
#include "TFIsoValueWidget.h"
#include "TFOpacityInfoWidget.h"
#include "TFOpacityWidget.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <vapor/ColorMap.h>
#include <vapor/FileUtils.h>
#include <vapor/ResourcePath.h>

using namespace Wasp;
using namespace VAPoR;
#include <algorithm>
TFEditor::TFEditor() : VSection("Transfer Function") {
    layout()->addWidget(_maps = new TFMapsGroup);
    layout()->addWidget(_mapsInfo = _maps->CreateInfoGroup());
    layout()->addWidget(range = new QRangeSliderTextCombo);
    layout()->addWidget(
        colorMapTypeDropdown = new ParamsWidgetDropdown(
            VAPoR::ColorMap::_interpTypeTag, {"Linear", "Discrete", "Diverging"},
            {TFInterpolator::linear, TFInterpolator::discrete, TFInterpolator::diverging},
            "Color Interpolation"));

    QMenu *menu = new QMenu;
    _maps->histo->PopulateSettingsMenu(menu);
    setMenu(menu);

    connect(range, SIGNAL(ValueChanged(float, float)), this, SLOT(_rangeChanged(float, float)));
    connect(range, SIGNAL(ValueChangedBegin()), this, SLOT(_rangeChangedBegin()));
    connect(range, SIGNAL(ValueChangedIntermediate(float, float)), this,
            SLOT(_rangeChangedIntermediate(float, float)));

    //    this->setStyleSheet(R"(QWidget:hover:!pressed {border: 1px solid red;})");
}

void TFEditor::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                      VAPoR::RenderParams *rParams) {
    _rParams = rParams;
    _paramsMgr = paramsMgr;
    colorMapTypeDropdown->Update(rParams->GetMapperFunc(rParams->GetVariableName())->GetColorMap());
    _maps->Update(dataMgr, paramsMgr, rParams);
    _mapsInfo->Update(rParams);
    _updateMappingRangeControl(dataMgr, paramsMgr, rParams);
}

void TFEditor::_updateMappingRangeControl(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                                          VAPoR::RenderParams *rParams) {
    float min, max;
    _getDataRange(dataMgr, rParams, &min, &max);
    range->SetRange(min, max);
    vector<double> mapperRange =
        rParams->GetMapperFunc(rParams->GetVariableName())->getMinMaxMapValue();
    range->SetValue(mapperRange[0], mapperRange[1]);
}

void TFEditor::_getDataRange(VAPoR::DataMgr *d, VAPoR::RenderParams *r, float *min,
                             float *max) const {
    std::vector<double> range;
    d->GetDataRange(r->GetCurrentTimestep(), r->GetVariableName(), r->GetRefinementLevel(),
                    r->GetCompressionLevel(),
                    d->GetDefaultMetaInfoStride(r->GetVariableName(), r->GetRefinementLevel()),
                    range);
    *min = range[0];
    *max = range[1];
}

void TFEditor::_rangeChangedBegin() { _paramsMgr->BeginSaveStateGroup("Change tf mapping range"); }

void TFEditor::_rangeChangedIntermediate(float left, float right) {
    _rParams->GetMapperFunc(_rParams->GetVariableName())->setMinMaxMapValue(left, right);
    _maps->histo->update();
    _paramsMgr->IntermediateChange();
}

void TFEditor::_rangeChanged(float left, float right) {
    _rParams->GetMapperFunc(_rParams->GetVariableName())->setMinMaxMapValue(left, right);
    _paramsMgr->EndSaveStateGroup();
}

std::map<std::string, QIcon> ColorMapMenuItem::icons;

QIcon ColorMapMenuItem::getCachedIcon(const std::string &path) {
    auto it = icons.find(path);
    if (it != icons.end())
        return it->second;

    ParamsBase::StateSave stateSave;
    MapperFunction mf(&stateSave);

    mf.LoadColormapFromFile(path);
    ColorMap *cm = mf.GetColorMap();

    QSize size = getIconSize();
    int nSamples = size.width();
    unsigned char buf[nSamples * 3];
    float rgb[3];
    for (int i = 0; i < nSamples; i++) {
        cm->colorNormalized(i / (float)nSamples).toRGB(rgb);
        buf[i * 3 + 0] = rgb[0] * 255;
        buf[i * 3 + 1] = rgb[1] * 255;
        buf[i * 3 + 2] = rgb[2] * 255;
    }
    QImage image(buf, nSamples, 1, QImage::Format::Format_RGB888);

    icons[path] = QIcon(QPixmap::fromImage(image).scaled(size.width(), size.height()));
    return icons[path];
}

QSize ColorMapMenuItem::getIconSize() { return QSize(50, 15); }

QSize ColorMapMenuItem::getIconPadding() { return QSize(10, 10); }

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
