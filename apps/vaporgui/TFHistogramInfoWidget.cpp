#include "TFHistogramInfoWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <vapor/RenderParams.h>

TFHistogramInfoWidget::TFHistogramInfoWidget() {
    layout()->addWidget(new QLabel);
    _valueEdit->setReadOnly(true);
}

void TFHistogramInfoWidget::SetControlPoint(float value) {
    this->setEnabled(true);
    SetNormalizedValue(value);
}

void TFHistogramInfoWidget::Deselect() {
    this->setEnabled(false);
    _valueEdit->clear();
}
