#pragma once

#include <string>

#include <QSpinBox>
#include <QWidget>

#include "VContainer.h"

//! class VIntSpinBox
//!
//! Wraps a QSpinBox and provides vaporgui's standard setter/getter functions
//! and signals.

class VIntSpinBox : public VContainer {
    Q_OBJECT

  public:
    VIntSpinBox(int min, int max);

    void SetValue(int value);
    void SetRange(int min, int max);

    int GetValue() const;

  private:
    QSpinBox *_spinBox;

  public slots:
    void emitSpinBoxChanged();

  signals:
    void ValueChanged(int value);
};