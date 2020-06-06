#include "PGroup.h"
#include "VSubGroup.h"
#include <QVBoxLayout>
#include <vapor/ParamsBase.h>

PGroup::PGroup() : PGroup(new QWidget) {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(4);
    _widget->setLayout(layout);
}

PGroup::PGroup(QWidget *w) : PWidget("", _widget = w) {}

PGroup *PGroup::Add(PWidget *pw) {
    _children.push_back(pw);
    _widget->layout()->addWidget(pw);
    return this;
}

void PGroup::updateGUI() const {
    auto params = getParams();
    auto paramsMgr = getParamsMgr();
    auto dataMgr = getDataMgr();

    for (PWidget *child : _children)
        child->Update(params, paramsMgr, dataMgr);
}

PSubGroup::PSubGroup() : PGroup(new VSubGroup) {}
