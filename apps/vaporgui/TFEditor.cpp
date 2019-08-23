#include "TFEditor.h"
#include "QRangeSlider.h"
#include "TFColorInfoWidget.h"
#include "TFColorWidget.h"
#include "TFHistogramWidget.h"
#include "TFOpacityInfoWidget.h"
#include "TFOpacityWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <vapor/ColorMap.h>
//#include "QColorWidget.h"

static ParamsWidgetColor *c;

TFEditor::TFEditor() {
    addTab(new QWidget(this), "Transfer Function");

    QVBoxLayout *layout = new QVBoxLayout;
    //    layout->setSpacing(0);
    layout->setMargin(12);
    _tab()->setLayout(layout);
    layout->addWidget(tff = new TFOpacityWidget);
    layout->addWidget(tfh = new TFHistogramWidget);
    layout->addWidget(colorWidget = new TFColorWidget);
    layout->addWidget(controlPointWidget = tff->CreateInfoWidget());
    layout->addWidget(colorInfo = colorWidget->CreateInfoWidget());
    layout->addWidget(range = new QRangeSlider);
    layout->addWidget(colorMapTypeDropdown = new ParamsWidgetDropdown(
                          VAPoR::ColorMap::_interpTypeTag, {"Linear", "Discrete", "Diverging"},
                          "Color Interpolation"));
    layout->addWidget(c = new ParamsWidgetColor("test"));

    //    this->setStyleSheet(R"(QWidget:hover:!pressed {border: 1px solid red;})");
}

void TFEditor::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                      VAPoR::RenderParams *rParams) {
    tff->Update(dataMgr, paramsMgr, rParams);
    tfh->Update(dataMgr, paramsMgr, rParams);
    colorWidget->Update(dataMgr, paramsMgr, rParams);
    colorMapTypeDropdown->Update(rParams->GetMapperFunc(rParams->GetVariableName())->GetColorMap());
    controlPointWidget->Update(rParams);
    colorInfo->Update(rParams);
    c->Update(rParams);
}

QWidget *TFEditor::_tab() const { return this->widget(0); }
