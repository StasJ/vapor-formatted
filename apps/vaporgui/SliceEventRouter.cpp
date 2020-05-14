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

#include "VLineComboBox.h"

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

    /*_pVariablesWidget = new PVariablesWidget();
    addTab( _pVariablesWidget, "PVW" );
    _pVariablesWidget->Reinit(
        (VariableFlags)(SCALAR),
        (DimFlags)(THREED)
    );*/

    //_vLineComboBox = new VLineComboBox("_vLineComboBox");
    // addTab( _vLineComboBox, "VLineComboBox" );

    //_vSliderEdit = new VSliderEdit();
    //_vli = new VLineItem("VLineItem", _vSliderEdit);
    // addTab( _vli, "VLineItem/VSliderEdit" );

    //_pDoubleInput = new PDoubleInput("demo_double", "PDoubleInput");
    // addTab( _pDoubleInput, "PDoubleInput Tab" );

    _appearance = new SliceAppearanceSubtab(this);
    QScrollArea *qsapp = new QScrollArea(this);
    qsapp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsapp->setWidget(_appearance);
    qsapp->setWidgetResizable(true);
    addTab(qsapp, "Appearance");

    //_pIntegerInput = new PIntegerInput("demo_int", "PIntegerInput");
    // addTab( _pIntegerInput, "PIntegerInput Tab" );

    _pSimpleWidget = new PSimpleWidget();
    addTab(_pSimpleWidget, "PSimpleWidget's Tab");

    _simpleWidget = new SimpleWidget();
    addTab(_simpleWidget, "SimpleWidget's Tab");

    _fidelityWidget = new FidelityWidget3();
    addTab(_fidelityWidget, "FidelityWidget's Tab");
    _fidelityWidget->Reinit((VariableFlags)(SCALAR));
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

    /*_pVariablesWidget->Update(
        GetActiveParams(),
        _controlExec->GetParamsMgr(),
        GetActiveDataMgr()
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

    _pSimpleWidget->Update(GetActiveParams(), _controlExec->GetParamsMgr(), GetActiveDataMgr());
    _simpleWidget->Update(GetActiveDataMgr(), _controlExec->GetParamsMgr(), GetActiveParams());

    _fidelityWidget->Update(GetActiveDataMgr(), _controlExec->GetParamsMgr(), GetActiveParams());

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
