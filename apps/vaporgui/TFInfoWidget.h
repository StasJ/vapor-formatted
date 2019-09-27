#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QWidget>
#include <string>

namespace VAPoR {
class RenderParams;
}

class TFInfoWidget : public QWidget {
    Q_OBJECT

  public:
    enum ValueFormat { Mapped = 0, Percent = 1 };

    bool UsingColormapVariable = false;

    TFInfoWidget();

    void Update(VAPoR::RenderParams *rParams);
    void DeselectControlPoint();
    void SetNormalizedValue(float value);

  protected:
    void paintEvent(QPaintEvent *event);
    void updateValue();
    void updateValueEditValidator();
    bool isUsingNormalizedValue() const;
    bool isUsingMappedValue() const;
    float toMappedValue(float normalized) const;
    float toNormalizedValue(float mapped) const;
    float getValueFromEdit() const;

    virtual void controlPointChanged(){};

  protected:
    QLineEdit *_valueEdit;
    QComboBox *_valueEditType;

    float _min = 0;
    float _max = 1;

  protected:
    float _value;

  private slots:
    void valueEditTypeChanged(int);
    void valueEditChanged();
};
