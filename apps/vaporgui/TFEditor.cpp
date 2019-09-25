#include "TFEditor.h"
#include "TFColorWidget.h"
#include "TFHistogramWidget.h"
#include "TFMapGroupWidget.h"
#include "TFMappingRangeSelector.h"
#include "TFOpacityWidget.h"

TFEditor::TFEditor() : VSection("Transfer Function") {
    _maps = new TFMapGroupWidget;
    _opacityMap = new TFOpacityMap(nullptr);
    _histogramMap = new TFHistogramMap(nullptr);
    _colorMap = new TFColorMap(nullptr);

    _maps->Add({_opacityMap, _histogramMap});
    _maps->Add(_colorMap);

    layout()->addWidget(_maps);
    layout()->addWidget(_mapsInfo = _maps->CreateInfoGroup());
    layout()->addWidget(range = new TFMappingRangeSelector);
    connect(range, SIGNAL(ValueChangedIntermediate(float, float)), _histogramMap, SLOT(update()));

    QMenu *menu = new QMenu;
    _colorMap->PopulateSettingsMenu(menu);
    menu->addSeparator();
    _opacityMap->PopulateSettingsMenu(menu);
    menu->addSeparator();
    _histogramMap->PopulateSettingsMenu(menu);
    setMenu(menu);

    //    this->setStyleSheet(R"(QWidget:hover:!pressed {border: 1px solid red;})");
}

void TFEditor::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                      VAPoR::RenderParams *rParams) {
    _maps->Update(dataMgr, paramsMgr, rParams);
    _mapsInfo->Update(rParams);
    range->Update(dataMgr, paramsMgr, rParams);
}
