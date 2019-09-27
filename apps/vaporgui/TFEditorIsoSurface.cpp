#include "TFEditorIsoSurface.h"
#include "TFColorWidget.h"
#include "TFHistogramWidget.h"
#include "TFIsoValueWidget.h"
#include "TFMapGroupWidget.h"
#include "TFMappingRangeSelector.h"
#include "TFOpacityWidget.h"

TFEditorIsoSurface::TFEditorIsoSurface() : VSection("Transfer Function") {
    _maps = new TFMapGroupWidget;
    _histogramMap = new TFHistogramMap;
    _isoMap = new TFIsoValueMap;
    _isoMap->SetEquidistantIsoValues(false);

    _maps->Add(_histogramMap);
    _maps->Add(_isoMap);

    layout()->addWidget(_maps);
    layout()->addWidget(_mapsInfo = _maps->CreateInfoGroup());
    layout()->addWidget(range = new TFMappingRangeSelector);
    connect(range, SIGNAL(ValueChangedIntermediate(float, float)), _histogramMap, SLOT(update()));

    _maps2 = new TFMapGroupWidget;
    _opacityMap2 = new TFOpacityMap;
    _histogramMap2 = new TFHistogramMap;
    _colorMap2 = new TFColorMap;

    _opacityMap2->UsingColormapVariable = true;
    _histogramMap2->UsingColormapVariable = true;
    _colorMap2->UsingColormapVariable = true;

    _maps2->Add({_opacityMap2, _histogramMap2});
    _maps2->Add(_colorMap2);

    layout()->addWidget(_maps2);
    layout()->addWidget(_mapsInfo2 = _maps2->CreateInfoGroup());
    layout()->addWidget(range2 = new TFMappingRangeSelector);
    connect(range2, SIGNAL(ValueChangedIntermediate(float, float)), _histogramMap2, SLOT(update()));
    range2->UsingColormapVariable = true;

    QMenu *menu = new QMenu;
    _colorMap2->PopulateSettingsMenu(menu);
    menu->addSeparator();
    _opacityMap2->PopulateSettingsMenu(menu);
    menu->addSeparator();
    _histogramMap->PopulateSettingsMenu(menu);
    setMenu(menu);

    //    this->setStyleSheet(R"(QWidget:hover:!pressed {border: 1px solid red;})");
}

void TFEditorIsoSurface::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                                VAPoR::RenderParams *rParams) {
    _maps->Update(dataMgr, paramsMgr, rParams);
    _mapsInfo->Update(rParams);
    range->Update(dataMgr, paramsMgr, rParams);

    _maps2->Update(dataMgr, paramsMgr, rParams);
    _mapsInfo2->Update(rParams);
    range2->Update(dataMgr, paramsMgr, rParams);
}
