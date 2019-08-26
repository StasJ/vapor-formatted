#include "TFHistogramInfoWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <vapor/RenderParams.h>

TFHistogramInfoWidget::TFHistogramInfoWidget() {
    ((QBoxLayout *)layout())->addWidget(new QLabel, 30, Qt::AlignRight);
    _valueEdit->setReadOnly(true);
}

void TFHistogramInfoWidget::Update(VAPoR::RenderParams *rParams) {
    TFInfoWidget::Update(rParams);
    if (!rParams)
        return;
}

void TFHistogramInfoWidget::SetControlPoint(float value) {
    this->setEnabled(true);
    SetNormalizedValue(value);
}

void TFHistogramInfoWidget::Deselect() {
    this->setEnabled(false);
    _valueEdit->clear();
}
