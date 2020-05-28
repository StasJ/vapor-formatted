#ifdef WIN32
// Annoying unreferenced formal parameter warning
#pragma warning(disable : 4100)
#endif

#include "SliceEventRouter.h"
#include "EventRouter.h"
#include "RenderEventRouter.h"
#include "VariablesWidget.h"
#include "vapor/SliceParams.h"
#include "vapor/SliceRenderer.h"
#include <QFileDialog>
#include <qcolordialog.h>
#include <qlineedit.h>
#include <qscrollarea.h>
#include <string>
#include <vapor/glutil.h>
#include <vector>

#include "PVariablesWidget.h"
#include "VLineComboBox.h"
#include "VVariablesContainer.h"

using namespace VAPoR;

//
// Register class with object factory!!!
//
static RenderEventRouterRegistrar<SliceEventRouter> registrar(SliceEventRouter::GetClassType());

SliceEventRouter::SliceEventRouter(QWidget *parent, ControlExec *ce)
    : QTabWidget(parent), RenderEventRouter(ce, SliceParams::GetClassType()) {
    _variables = new SliceVariablesSubtab(this);
    QScrollArea *qsvar = new QScrollArea(this);
    qsvar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _variables->adjustSize();
    qsvar->setWidget(_variables);
    qsvar->setWidgetResizable(true);
    addTab(qsvar, "Variables");

    _geometry = new SliceGeometrySubtab(this);
    QScrollArea *qsgeo = new QScrollArea(this);
    qsgeo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsgeo->setWidget(_geometry);
    qsgeo->setWidgetResizable(true);
    addTab(qsgeo, "Geometry");

    _annotation = new SliceAnnotationSubtab(this);
    QScrollArea *qsAnnotation = new QScrollArea(this);
    qsAnnotation->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsAnnotation->setWidget(_annotation);
    qsAnnotation->setWidgetResizable(true);
    addTab(qsAnnotation, "Annotations");

    _appearance = new SliceAppearanceSubtab(this);
    QScrollArea *qsapp = new QScrollArea(this);
    qsapp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsapp->setWidget(_appearance);
    qsapp->setWidgetResizable(true);
    addTab(qsapp, "Appearance");

    _vVariablesContainer = new VVariablesContainer();
    _vVariablesContainer->Reinit((VariableFlags)(SCALAR), (DimFlags)(THREED));
    QScrollArea *qsvvw = new QScrollArea(this);
    qsvvw->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsvvw->setWidget(_vVariablesContainer);
    qsvvw->setWidgetResizable(true);
    addTab(qsvvw, "VVariablesContainer");

    _pVariablesWidget = new PVariablesWidget();
    _pVariablesWidget->Reinit((VariableFlags)(SCALAR), (DimFlags)(THREED));
    QScrollArea *qspvw = new QScrollArea(this);
    qspvw->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qspvw->setWidget(_pVariablesWidget);
    qspvw->setWidgetResizable(true);
    addTab(qspvw, "PVariablesWidget");

#if 0	
	QScrollArea *qsimg = new QScrollArea(this);
	qsimg->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_image = new SliceImageGUI(this);
	qsimg->setWidget(_image);
	addTab(qsimg,"Image");
	_image->adjustSize();

	ImageFrame *imageFrame = _image->imageFrame;
	QGLFormat fmt;
	fmt.setAlpha(true);
	fmt.setRgba(true);
	fmt.setDoubleBuffer(true);
	fmt.setDirectRendering(true);
	_glSliceImageWindow = new GLSliceImageWindow(fmt,imageFrame,"gltwoddatawindow",imageFrame);
	imageFrame->attachRenderWindow(_glSliceImageWindow, this);
#endif
}

void SliceEventRouter::GetWebHelp(vector<pair<string, string>> &help) const {
    help.clear();

    help.push_back(make_pair("Slice Overview",
                             "http://www.vapor.ucar.edu/docs/vapor-gui-help/twoD#SliceOverview"));

    help.push_back(make_pair("Renderer control",
                             "http://www.vapor.ucar.edu/docs/vapor-how-guide/renderer-instances"));

    help.push_back(
        make_pair("Data accuracy control",
                  "http://www.vapor.ucar.edu/docs/vapor-how-guide/refinement-and-lod-control"));

    help.push_back(make_pair("Slice geometry options",
                             "http://www.vapor.ucar.edu/docs/vapor-gui-help/twoD#SliceGeometry"));

    help.push_back(make_pair("Slice Appearance settings",
                             "http://www.vapor.ucar.edu/docs/vapor-gui-help/twoD#HelloAppearance"));
}

void SliceEventRouter::_updateTab() {

    /*_variablesWidget->Update(
        GetActiveDataMgr(),
        _controlExec->GetParamsMgr(),
        GetActiveParams()
    );*/

    /*_pDoubleInput->Update(
        GetActiveParams(),
        _controlExec->GetParamsMgr(),
        GetActiveDataMgr()
    );*/
    /*_pIntegerInput->Update(
        GetActiveParams(),
        _controlExec->GetParamsMgr(),
        GetActiveDataMgr()
    );*/

    /*_pSimpleWidget->Update(
        GetActiveParams(),
        _controlExec->GetParamsMgr(),
        GetActiveDataMgr()
    );
    */

    /*_simpleWidget->Update(
        GetActiveDataMgr(),
        _controlExec->GetParamsMgr(),
        GetActiveParams()
    );*/

    _vVariablesContainer->Update(GetActiveParams(), _controlExec->GetParamsMgr(),
                                 GetActiveDataMgr());
    _pVariablesWidget->Update(GetActiveParams(), _controlExec->GetParamsMgr(), GetActiveDataMgr());

    /*_pFidelityWidget->Update(
        GetActiveParams(),
        _controlExec->GetParamsMgr(),
        GetActiveDataMgr()
    );*/

    //_vSliderEdit->SetValue( 1.0 );

    /*_fidelityWidget3->Update(
        GetActiveDataMgr(),
        _controlExec->GetParamsMgr(),
        GetActiveParams()
    );*/

    /*_pTest->Update(
        GetActiveParams(),
        _controlExec->GetParamsMgr(),
        GetActiveDataMgr()
    );
    _pdse->Update(
        GetActiveParams(),
        _controlExec->GetParamsMgr(),
        GetActiveDataMgr()
    );*/

    // The variable tab updates itself:
    //
    _variables->Update(GetActiveDataMgr(), _controlExec->GetParamsMgr(), GetActiveParams());

    _appearance->Update(GetActiveDataMgr(), _controlExec->GetParamsMgr(), GetActiveParams());
    _geometry->Update(_controlExec->GetParamsMgr(), GetActiveDataMgr(), GetActiveParams());
    _annotation->Update(_controlExec->GetParamsMgr(), GetActiveDataMgr(), GetActiveParams());
}

string SliceEventRouter::_getDescription() const {
    return ("Displays an axis-aligned slice or cutting plane through"
            "a 3D variable.  Slices are sampled along the plane's axes according"
            " to a sampling rate define by the user.\n\n");
}
