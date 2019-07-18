#pragma once

#include <QCheckBox>
#include <QWidget>
#include <vapor/RenderParams.h>

class OSPRayEnableCheckbox : public QWidget {

    Q_OBJECT

    VAPoR::RenderParams *_renderParams = nullptr;
    QCheckBox *_checkBox;

  public:
    OSPRayEnableCheckbox(QWidget *parent);
    void Update(VAPoR::RenderParams *rParams);

  private slots:
    void checkbox_clicked(bool checked);
};
