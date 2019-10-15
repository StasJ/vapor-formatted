#ifdef WIN32
// Annoying unreferenced formal parameter warning
#pragma warning(disable : 4100)
#endif

#include "ModelEventRouter.h"
#include "EventRouter.h"
#include "VariablesWidget.h"
#include "vapor/ModelParams.h"
#include <QFileDialog>
#include <qcolordialog.h>
#include <qlineedit.h>
#include <qscrollarea.h>
#include <string>
#include <vapor/MapperFunction.h>
#include <vapor/glutil.h>
#include <vector>

using namespace VAPoR;

//
// Register class with object factory!!!
//
static RenderEventRouterRegistrar<ModelEventRouter> registrar(ModelEventRouter::GetClassType());

ModelEventRouter::ModelEventRouter(QWidget *parent, ControlExec *ce)
    : QTabWidget(parent), RenderEventRouter(ce, ModelParams::GetClassType()) {

    _variables = new ModelVariablesSubtab(this);
    QScrollArea *qsvar = new QScrollArea(this);
    qsvar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _variables->adjustSize();
    qsvar->setWidget(_variables);
    qsvar->setWidgetResizable(true);
    addTab(qsvar, "Variables");

    _appearance = new ModelAppearanceSubtab(this);
    QScrollArea *qsapp = new QScrollArea(this);
    qsapp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsapp->setWidget(_appearance);
    qsapp->setWidgetResizable(true);
    addTab(qsapp, "Appearance");

    _geometry = new ModelGeometrySubtab(this);
    QScrollArea *qsgeo = new QScrollArea(this);
    qsgeo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsgeo->setWidget(_geometry);
    qsgeo->setWidgetResizable(true);
    addTab(qsgeo, "Geometry");

    _annotation = new ModelAnnotationSubtab(this);
    QScrollArea *qsAnnotation = new QScrollArea(this);
    qsAnnotation->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsAnnotation->setWidget(_annotation);
    qsAnnotation->setWidgetResizable(true);
    addTab(qsAnnotation, "Annotation");
}

// Destructor does nothing
ModelEventRouter::~ModelEventRouter() {
    if (_variables)
        delete _variables;
#ifdef VAPOR3_0_0_ALPHA
    if (_image)
        delete _image;
#endif
    if (_geometry)
        delete _geometry;
    if (_appearance)
        delete _appearance;
}

void ModelEventRouter::GetWebHelp(vector<pair<string, string>> &help) const {
    help.clear();

    help.push_back(
        make_pair("Model Overview", "http://www.vapor.ucar.edu/docs/vapor-gui-help/ModelOverview"));
    help.push_back(make_pair("Renderer control",
                             "http://www.vapor.ucar.edu/docs/vapor-how-guide/renderer-instances"));
    help.push_back(
        make_pair("Data accuracy control",
                  "http://www.vapor.ucar.edu/docs/vapor-how-guide/refinement-and-lod-control"));
    help.push_back(make_pair("Model geometry options",
                             "http://www.vapor.ucar.edu/docs/vapor-gui-help/ModelGeometry"));
    help.push_back(make_pair("Model Appearance settings",
                             "<>"
                             "http://www.vapor.ucar.edu/docs/vapor-gui-help/ModelAppearance"));
}

void ModelEventRouter::_initializeTab() {
    _updateTab();
    ModelParams *rParams = (ModelParams *)GetActiveParams();
}

void ModelEventRouter::_updateTab() {
    // The variable tab updates itself:
    _variables->Update(GetActiveDataMgr(), _controlExec->GetParamsMgr(), GetActiveParams());

    _appearance->Update(GetActiveDataMgr(), _controlExec->GetParamsMgr(), GetActiveParams());

    _geometry->Update(_controlExec->GetParamsMgr(), GetActiveDataMgr(), GetActiveParams());

    _annotation->Update(_controlExec->GetParamsMgr(), GetActiveDataMgr(), GetActiveParams());
}

string ModelEventRouter::_getDescription() const {
    return ("Allows the import of 3D model files as well as more complex scenes that "
            "can be modified per timestep by using .vms files");
}
