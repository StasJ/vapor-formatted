#include "TFEditor.h"
#include "ErrorReporter.h"
#include "SettingsParams.h"
#include "TFColorInfoWidget.h"
#include "TFColorWidget.h"
#include "TFHistogramWidget.h"
#include "TFIsoValueWidget.h"
#include "TFMapGroupWidget.h"
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
#include <algorithm>
TFEditor::TFEditor() : VSection("Transfer Function") {
    layout()->addWidget(_maps = new TFMapsGroup);
    layout()->addWidget(_mapsInfo = _maps->CreateInfoGroup());
    layout()->addWidget(range = new TFMappingRangeSelector);
    connect(range, SIGNAL(ValueChangedIntermediate(float, float)), _maps->histo, SLOT(update()));

    QMenu *menu = new QMenu;
    _maps->histo->PopulateSettingsMenu(menu);
    setMenu(menu);

    //    this->setStyleSheet(R"(QWidget:hover:!pressed {border: 1px solid red;})");
}

void TFEditor::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                      VAPoR::RenderParams *rParams) {
    _rParams = rParams;
    _paramsMgr = paramsMgr;
    _maps->Update(dataMgr, paramsMgr, rParams);
    _mapsInfo->Update(rParams);
    range->Update(dataMgr, paramsMgr, rParams);
}
