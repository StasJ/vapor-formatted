#pragma once

#include <string>

#include <QPushButton>
#include <QWidget>

#include "VContainer.h"

class VPushButton2 : public VContainer {
    Q_OBJECT

  public:
    VPushButton2(const std::string &buttonText = "Select");

  private:
    QPushButton *_pushButton;

  public slots:
    void emitButtonClicked();

  signals:
    void ButtonClicked();
};
