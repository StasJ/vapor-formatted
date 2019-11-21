#pragma once

#include <string>

#include <QSlider>
#include <QWidget>

#include "VContainer.h"

class VSlider : public VContainer {
    Q_OBJECT

  public:
    VSlider(double min = 0, double max = 1);

    void SetValue(double value);
    void SetRange(double min, double max);

    double GetValue() const;

  private:
    QSlider *_slider;
    double _minValid;
    double _maxValid;
    double _stepSize;

  private slots:
    void _sliderChanged();
    void _sliderChangedIntermediate(int position);

  signals:
    void ValueChanged(double value);
    void ValueChangedIntermediate(double value);
};
