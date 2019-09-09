#ifdef WIN32
// Annoying unreferenced formal parameter warning
#pragma warning(disable : 4100)
#endif

#include "SliceEventRouter.h"
#include "EventRouter.h"
#include "ParamsWidget.h"
#include "VGeometry.h"
#include "VLabel.h"
#include "VRange.h"
#include "VSpinBox.h"
#include "VaporWidget.h"
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

using namespace VAPoR;

//
// Register class with object factory!!!
//
static RenderEventRouterRegistrar<SliceEventRouter> registrar(SliceEventRouter::GetClassType());

SliceEventRouter::SliceEventRouter(QWidget *parent, ControlExec *ce)
    : QTabWidget(parent), RenderEventRouter(ce, SliceParams::GetClassType()) {
    _testTab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;

    _psb = new PSpinBox(_testTab, "testTag", "testDescription", "PSpinBox", 0, 100, 5);
    std::string toolTip = "test tool tip";
    _psb->SetToolTip(toolTip);
    layout->addWidget(_psb);

    _psb2 = new PSpinBox(_testTab, "testTag", "testDescription", "PSpinBox2", 0, 100, 5);
    layout->addWidget(_psb2);

    _vsb = new VSpinBox(
        //_testTab,
        "VSpinBox", 0, 100, 5);
    layout->addWidget(_vsb);

    _psl = new PSlider(_testTab, "sliderTag", "sliderDescription", "PSlider1", 0, 100, 33);
    layout->addWidget(_psl);

    _psl2 = new PSlider(_testTab, "sliderTag", "sliderDescription", "PSlider2", 0, 100, 33);
    layout->addWidget(_psl2);

    VLabel *label = new VLabel(_testTab, "VLabel");
    layout->addWidget(label);

    _vrange = new VRange(_testTab, 0, 200, "VRangeMin", "VRangeMax");
    layout->addWidget(_vrange);

    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    std::vector<double> range = {0., 0., 0., 1000., 1000., 1000.};
    std::vector<std::string> labels = {"X min", "Y min", "Z min", "X max", "Y max", "Z max"};
    _vgeometry = new VGeometry(nullptr, range, labels);
    // layout->addWidget( _pgeometry );
    tabWidget->addTab(_vgeometry, "Geometry");
    layout->addWidget(tabWidget);

    _testTab->setLayout(layout);
    _testTab->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _testTab->adjustSize();
    QScrollArea *sa = new QScrollArea(this);
    sa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sa->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sa->setWidget(_testTab);
    sa->setWidgetResizable(true);
    addTab(sa, "ParamsWidgets");

    // Default subtabs

    _variables = new SliceVariablesSubtab(this);
    QScrollArea *qsvar = new QScrollArea(this);
    qsvar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _variables->adjustSize();
    qsvar->setWidget(_variables);
    qsvar->setWidgetResizable(true);
    addTab(qsvar, "Variables");

    _appearance = new SliceAppearanceSubtab(this);
    QScrollArea *qsapp = new QScrollArea(this);
    qsapp->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    qsapp->setWidget(_appearance);
    qsapp->setWidgetResizable(true);
    addTab(qsapp, "Appearance");

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

SliceEventRouter::~SliceEventRouter() {
    if (_variables)
        delete _variables;
    if (_geometry)
        delete _geometry;
    if (_appearance)
        delete _appearance;
    if (_annotation)
        delete _annotation;
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

    ParamsBase *params = GetActiveParams();
    _psb->Update(params);
    _psb2->Update(params);
    _psl->Update(params);
    _psl2->Update(params);

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
