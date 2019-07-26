#include "GL/glew.h"
#include "qcolordialog.h"
#include <qdesktopwidget.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qrect.h>

#include <QGroupBox>
#include <fstream>
#include <iostream>
#include <qlabel.h>
#include <sstream>
#include <string>
#include <vector>

#include "TabManager.h"

#include "EventRouter.h"
#include "OSPRayEventRouter.h"
#include "ParamsWidgets.h"
#include "vapor/ControlExecutive.h"
#include <vapor/OSPRayParams.h>

using namespace VAPoR;

OSPRayEventRouter::OSPRayEventRouter(QWidget *parent, ControlExec *ce)
    : QWidget(parent), EventRouter(ce, OSPRayParams::GetClassType()) {
    QVBoxLayout *layout = new QVBoxLayout;
    this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
    this->setLayout(layout);

    ParamsWidgetGroup *group = new ParamsWidgetGroup("Global Config");

    _addParamsWidget(
        group, new ParamsWidgetNumber(OSPRayParams::_aoSamplesTag, "Ambient Occlusion Samples"));
    _addParamsWidget(group, new ParamsWidgetNumber(OSPRayParams::_samplesPerPixelTag));
    _addParamsWidget(group, new ParamsWidgetFloat(OSPRayParams::_ambientIntensity));
    _addParamsWidget(group, new ParamsWidgetFloat(OSPRayParams::_spotlightIntensity));

    layout->addWidget(group);
    layout->addStretch();
}

OSPRayEventRouter::~OSPRayEventRouter() {}

void OSPRayEventRouter::_blockSignals(bool block) {
    QList<QWidget *> widgetList = this->findChildren<QWidget *>();
    QList<QWidget *>::const_iterator widgetIter(widgetList.begin());
    QList<QWidget *>::const_iterator lastWidget(widgetList.end());

    while (widgetIter != lastWidget) {
        (*widgetIter)->blockSignals(block);
        ++widgetIter;
    }
}

void OSPRayEventRouter::_addParamsWidget(QWidget *parent, ParamsWidget *widget) {
    parent->layout()->addWidget(widget);
    _paramsWidgets.push_back(widget);
}

// Insert values from params into tab panel
//
void OSPRayEventRouter::_updateTab() {
    _blockSignals(true);

    OSPRayParams *params = _controlExec->GetParamsMgr()->GetOSPRayParams();

    const auto end = _paramsWidgets.end();
    for (auto it = _paramsWidgets.begin(); it != end; ++it)
        (*it)->Update(params);

    _blockSignals(false);
}

void OSPRayEventRouter::GetWebHelp(vector<pair<string, string>> &help) const {
    help.clear();

    help.push_back(
        make_pair("Overview of the Settings tab",
                  "http://www.vapor.ucar.edu/docs/vapor-gui-help/startup-tab#SettingsOverview"));
}