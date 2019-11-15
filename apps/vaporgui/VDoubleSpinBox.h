#pragma once

#include <string>

#include <QDoubleSpinBox>
#include <QWidget>

#include "VContainer.h"

class VDoubleSpinBox : public VContainer {
    Q_OBJECT

  public:
    VDoubleSpinBox(double min, double max);

    void SetValue(double value);
    void SetRange(double min, double max);

    double GetValue() const;

  private:
    QSpinBox *_spinBox;

  public slots:
    void emitSpinBoxChanged();

  signals:
    void ValueChanged(double value);
};
