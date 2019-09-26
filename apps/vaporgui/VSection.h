#pragma once

#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <string>

#include <QToolButton>

class VSection : public QTabWidget {
    Q_OBJECT

    class SettingsMenuButton;

  public:
    VSection(const std::string &title);
    QVBoxLayout *layout() const;
    void setMenu(QMenu *menu);

    void setLayout(QLayout *layout) = delete;
    int addTab(QWidget *page, const QString &label) = delete;
    QWidget *widget(int index) const = delete;
    void setCornerWidget(QWidget *widget, Qt::Corner corner) = delete;
    QWidget *cornerWidget() const = delete;

  private:
    QWidget *_tab() const;
    QString _createStylesheet() const;
};

class VSection::SettingsMenuButton : public QToolButton {
    Q_OBJECT

  public:
    SettingsMenuButton();

  protected:
    void paintEvent(QPaintEvent *event);
};