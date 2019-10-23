#include "TFIsoValueInfoWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <vapor/RenderParams.h>

void TFIsoValueInfoWidget::controlPointChanged() {
    emit ControlPointChanged(this->getValueFromEdit());
}

void TFIsoValueInfoWidget::SetControlPoint(float value) {
    this->setEnabled(true);
    SetNormalizedValue(value);
}

void TFIsoValueInfoWidget::Deselect() {
    this->setEnabled(false);
    _valueEdit->clear();
}
