#pragma once

#include <QAction>
#include <cassert>
#include <vector>

namespace VAPoR {
class ParamsBase;
}

class ParamsMenuItem : public QAction {
    Q_OBJECT

  public:
    ParamsMenuItem(const std::string &tag, const std::string &label = "");
    virtual void Update(VAPoR::ParamsBase *p) = 0;

  protected:
    VAPoR::ParamsBase *_params = nullptr;
    std::string _tag;
    std::string _label;
};

class ParamsDropdownMenuItem : public ParamsMenuItem {
    Q_OBJECT

    std::vector<int> _itemValues;
    std::vector<QAction *> _items;

  public:
    ParamsDropdownMenuItem(const std::string &tag, const std::vector<std::string> &items,
                           const std::vector<int> &itemValues = {}, const std::string &label = "");
    void Update(VAPoR::ParamsBase *p);

  private:
    int getValueForIndex(int index) const;
    int getIndexForValue(int value) const;

  private slots:
    void itemSelected();
};
