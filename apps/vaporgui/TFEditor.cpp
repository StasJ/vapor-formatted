#include "TFEditor.h"
#include "QRangeSlider.h"
#include "QRangeSliderTextCombo.h"
#include "TFColorInfoWidget.h"
#include "TFColorWidget.h"
#include "TFHistogramWidget.h"
#include "TFOpacityInfoWidget.h"
#include "TFOpacityWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <vapor/ColorMap.h>

TFEditor::TFEditor() {
    addTab(new QWidget(this), "Transfer Function");

    QVBoxLayout *layout = new QVBoxLayout;
    //    layout->setSpacing(0);
    layout->setMargin(12);
    _tab()->setLayout(layout);
    layout->addWidget(_maps = new TFMapsGroup);
    layout->addWidget(_mapsInfo = _maps->CreateInfoGroup());
    layout->addWidget(range = new QRangeSliderTextCombo);
    layout->addWidget(colorMapTypeDropdown = new ParamsWidgetDropdown(
                          VAPoR::ColorMap::_interpTypeTag, {"Linear", "Discrete", "Diverging"},
                          "Color Interpolation"));

    connect(range, SIGNAL(ValueChanged(float, float)), this, SLOT(_rangeChanged(float, float)));

    //    this->setStyleSheet(R"(QWidget:hover:!pressed {border: 1px solid red;})");
}

void TFEditor::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                      VAPoR::RenderParams *rParams) {
    _rParams = rParams;
    colorMapTypeDropdown->Update(rParams->GetMapperFunc(rParams->GetVariableName())->GetColorMap());
    _maps->Update(dataMgr, paramsMgr, rParams);
    _mapsInfo->Update(rParams);
    _updateMappingRangeControl(dataMgr, paramsMgr, rParams);
}

QWidget *TFEditor::_tab() const { return this->widget(0); }

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

void TFEditor::_rangeChanged(float left, float right) {
    _rParams->GetMapperFunc(_rParams->GetVariableName())->setMinMaxMapValue(left, right);
}

TFMapsGroup::TFMapsGroup() {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);

    add(new TFOpacityWidget);
    add(new TFHistogramWidget);
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

void TFMapsInfoGroup::add(TFMapWidget *map) {
    TFInfoWidget *info = map->GetInfoWidget();
    _infos.push_back(info);
    addWidget(info);
    connect(map, SIGNAL(Activated(TFMapWidget *)), this, SLOT(mapActivated(TFMapWidget *)));
}

void TFMapsInfoGroup::mapActivated(TFMapWidget *activatedMap) {
    setCurrentWidget(activatedMap->GetInfoWidget());
}
