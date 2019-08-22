#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>
#include <cassert>
#include <vapor/ParamsBase.h>
#include <vector>

class ParamsWidget : public QWidget {
    Q_OBJECT

  public:
    ParamsWidget(const std::string &tag, const std::string &label = "");
    virtual void Update(VAPoR::ParamsBase *p) = 0;

  protected:
    VAPoR::ParamsBase *_params = nullptr;
    std::string _tag;
    std::string _label;
};

class ParamsWidgetCheckbox : public ParamsWidget {
    Q_OBJECT

    QCheckBox *_checkBox = nullptr;

  public:
    ParamsWidgetCheckbox(const std::string &tag, const std::string &label = "");
    void Update(VAPoR::ParamsBase *p);

  private slots:
    void checkbox_clicked(bool checked);
};

class ParamsWidgetNumber : public ParamsWidget {
    Q_OBJECT

    QLineEdit *_lineEdit = nullptr;

  public:
    ParamsWidgetNumber(const std::string &tag, const std::string &label = "");
    void Update(VAPoR::ParamsBase *p);

    ParamsWidgetNumber *SetRange(int min, int max);

  private slots:
    void valueChangedSlot();
};

class ParamsWidgetFloat : public ParamsWidget {
    Q_OBJECT

    QLineEdit *_lineEdit = nullptr;

  public:
    ParamsWidgetFloat(const std::string &tag, const std::string &label = "");
    void Update(VAPoR::ParamsBase *p);

    ParamsWidgetFloat *SetRange(float min, float max);

  private slots:
    void valueChangedSlot();
};

class ParamsWidgetDropdown : public ParamsWidget {
    Q_OBJECT

    QComboBox *_box = nullptr;

  public:
    ParamsWidgetDropdown(const std::string &tag, const std::vector<std::string> &items,
                         const std::string &label = "");
    void Update(VAPoR::ParamsBase *p);

  private slots:
    void indexChangedSlot(int index);
};

class QColorWidget;
class ParamsWidgetColor : public ParamsWidget {
    Q_OBJECT

    //    QPushButton *_button = nullptr;
    QColorWidget *_color = nullptr;

  public:
    ParamsWidgetColor(const std::string &tag, const std::string &label = "");
    void Update(VAPoR::ParamsBase *p);

    static QColor VectorToQColor(const std::vector<double> &v);
    static std::vector<double> QColorToVector(const QColor &c);

  private slots:
    //    void pressed();
    void colorChanged(QColor color);
};

class ParamsWidgetGroup : public QGroupBox {
    Q_OBJECT

    bool fontUpdated = false;

  public:
    ParamsWidgetGroup(const std::string &title);

  protected:
    void changeEvent(QEvent *event);
};

class ParamsWidgetTabGroup : public QTabWidget {
    Q_OBJECT

    QWidget *_tab() const;
    std::vector<ParamsWidget *> _widgets;

  public:
    ParamsWidgetTabGroup(const std::string &title);
    void Update(VAPoR::ParamsBase *p);
    void Add(ParamsWidget *widget);
};