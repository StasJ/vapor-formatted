#include <QLabel>
#include <QVBoxLayout>

#include <vapor/VAssert.h>

#include "VGeometry2.h"

//              Flow params ordering      Usual "Vapor" ordering
#define XMIN 0 // 0
#define YMIN 2 // 1
#define ZMIN 4 // 2
#define XMAX 1 // 3
#define YMAX 3 // 4
#define ZMAX 5 // 5

VGeometry2::VGeometry2() : QWidget() {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    QHBoxLayout *xLayout = new QHBoxLayout;
    _xRange = new QRangeSliderTextCombo();
    connect(_xRange, SIGNAL(ValueChanged(float, float)), this, SLOT(_xRangeChanged(float, float)));
    xLayout->addWidget(new QLabel("X"));
    xLayout->addWidget(_xRange);
    layout->addLayout(xLayout);

    QHBoxLayout *yLayout = new QHBoxLayout;
    _yRange = new QRangeSliderTextCombo();
    connect(_yRange, SIGNAL(ValueChanged(float, float)), this, SLOT(_yRangeChanged(float, float)));
    yLayout->addWidget(new QLabel("Y"));
    yLayout->addWidget(_yRange);
    layout->addLayout(yLayout);

    QHBoxLayout *zLayout = new QHBoxLayout;
    _zRange = new QRangeSliderTextCombo();
    connect(_zRange, SIGNAL(ValueChanged(float, float)), this, SLOT(_zRangeChanged(float, float)));
    zLayout->addWidget(new QLabel("Z"));
    zLayout->addWidget(_zRange);
    layout->addLayout(zLayout);
}

void VGeometry2::SetRange(const std::vector<float> &range) {
    int dim = range.size() / 2;
    VAssert(dim == 2 || dim == 3);

    _range = range;

    _xRange->SetRange(range[XMIN], range[XMAX]);
    _yRange->SetRange(range[YMIN], range[YMAX]);

    if (dim == 3) {
        _zRange->show();
        _zRange->SetRange(_range[ZMIN], _range[ZMAX]);
    } else
        _zRange->hide();
}

void VGeometry2::SetValue(const std::vector<float> &vals) {
    int dim = vals.size() / 2;
    VAssert(dim == 2 || dim == 3);
    VAssert(vals[XMIN] >= _range[XMIN]);
    VAssert(vals[XMAX] <= _range[XMAX]);
    VAssert(vals[YMIN] >= _range[YMIN]);
    VAssert(vals[YMAX] <= _range[YMAX]);
    VAssert(vals[ZMIN] >= _range[ZMIN]);
    VAssert(vals[ZMAX] <= _range[ZMAX]);

    _values = vals;

    _xRange->SetValue(_values[XMIN], _values[XMAX]);
    _yRange->SetValue(_values[YMIN], _values[YMAX]);

    if (dim == 3) {
        _zRange->show();
        _zRange->SetValue(_values[ZMIN], _values[ZMAX]);
    } else
        _zRange->hide();
}

void VGeometry2::GetValue(std::vector<float> &vals) const { vals = _values; }

void VGeometry2::_xRangeChanged(float min, float max) {
    VAssert(min >= _range[XMIN]);
    VAssert(max <= _range[XMAX]);

    _values[XMIN] = min;
    _values[XMAX] = max;

    emit ValueChanged(_values);
}

void VGeometry2::_yRangeChanged(float min, float max) {
    VAssert(min >= _range[YMIN]);
    VAssert(max <= _range[YMAX]);

    _values[YMIN] = min;
    _values[YMAX] = max;

    emit ValueChanged(_values);
}

void VGeometry2::_zRangeChanged(float min, float max) {
    VAssert(min >= _range[ZMIN]);
    VAssert(max <= _range[ZMAX]);

    _values[ZMIN] = min;
    _values[ZMAX] = max;

    emit ValueChanged(_values);
}
