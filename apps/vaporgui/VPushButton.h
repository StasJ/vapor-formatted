#pragma once

#include <string>

#include <QPushButton>
#include <QWidget>

#include "VContainer.h"

//! \class VPushButton
//!
//! Wraps a QPushButton with vaporgui's VContainer, so that
//! the QPushButton follows correct size policy

class VPushButton : public VContainer {
    Q_OBJECT

  public:
    VPushButton(const std::string &buttonText = "Select");

  private:
    QPushButton *_pushButton;

  public slots:
    void emitButtonClicked();

  signals:
    void ButtonClicked();
};
