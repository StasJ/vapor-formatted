#include "TFEditor.h"
#include "QRangeSlider.h"
#include "TFColorWidget.h"
#include "TFControlPointWidget.h"
#include "TFHistogramWidget.h"
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
    layout->addWidget(controlPointWidget = new TFControlPointWidget);
    layout->addWidget(tff = new TFOpacityWidget);
    layout->addWidget(tfh = new TFHistogramWidget);
    layout->addWidget(colorWidget = new TFColorWidget);
    layout->addWidget(range = new QRangeSlider);
    layout->addWidget(colorMapTypeDropdown = new ParamsWidgetDropdown(
                          VAPoR::ColorMap::_interpTypeTag, {"Linear", "Discrete", "Diverging"},
                          "Color Interpolation"));

    //    this->setStyleSheet(R"(QWidget:hover:!pressed {border: 1px solid red;})");

    connect(tff, SIGNAL(SelectControlPoint(int)), controlPointWidget,
            SLOT(SelectOpacityControlPoint(int)));
    connect(tff, SIGNAL(DeselectControlPoint()), controlPointWidget, SLOT(DeselectControlPoint()));
}

void TFEditor::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                      VAPoR::RenderParams *rParams) {
    tff->Update(dataMgr, paramsMgr, rParams);
    tfh->Update(dataMgr, paramsMgr, rParams);
    colorWidget->Update(dataMgr, paramsMgr, rParams);
    colorMapTypeDropdown->Update(rParams->GetMapperFunc(rParams->GetVariableName())->GetColorMap());
    controlPointWidget->Update(dataMgr, paramsMgr, rParams);
}

QWidget *TFEditor::_tab() const { return this->widget(0); }
