#include <cmath>
#include <iostream>

#include <QMenu>

#include "vapor/VAssert.h"

#include "VActions.h"
#include "VSlider.h"
#include "VSliderEditInterface.h"

VSliderEditInterface::VSliderEditInterface() : VContainer() {
    _slider = new VSlider();
    _slider->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    layout()->addWidget(_slider);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &VSliderEditInterface::customContextMenuRequested, this,
            &VSliderEditInterface::ShowContextMenu);
}

QSize VSliderEditInterface::sizeHint() const {
    QWidget *parent = this->parentWidget();
    return QSize(parent->width() * 3 / 5., 20);
}
