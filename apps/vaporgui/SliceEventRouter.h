#ifndef SLICEEVENTROUTER_H
#define SLICEEVENTROUTER_H

#include "GL/glew.h"
#include "RenderEventRouter.h"
#include "RenderEventRouter2.h"
#include "VariablesWidget.h"
#include "vapor/SliceParams.h"
#include "vapor/SliceRenderer.h"
#include <qobject.h>
#include <vapor/MyBase.h>
//#include "PVariablesWidget.h"
#include "PDoubleInput.h"
#include "PIntegerInput.h"
#include "PSimpleWidget.h"
#include "SliceSubtabs.h"
#include "VLineComboBox.h"
#include "VSliderEdit.h"
#include "VariablesWidget2.h"
#include <QTabWidget>

QT_USE_NAMESPACE

namespace VAPoR {
class ControlExec;
}

class GLSliceImageWindow;
//#include "PFidelityWidget.h"
class PFidelityWidget3;
class VVariablesWidget;
class PVariablesWidget;
class FidelityWidget3;
class VVariablesContainer;

//!
//! \class SliceEventRouter
//! \ingroup Public_GUI
//! \brief An EventRouter subclass that handles the Slice tab in the GUI
//! \author Scott Pearse
//! \version 3.0
//! \date  April 2016

//!	The SliceEventRouter class manages the Slice gui.  There are three sub-tabs,
//! for variables, geometry, and appearance.

// class SliceEventRouter : public QTabWidget,  public RenderEventRouter {
class SliceEventRouter : public QTabWidget, public RenderEventRouter {

    Q_OBJECT

  public:
    SliceEventRouter(QWidget *parent, VAPoR::ControlExec *ce);

    void GetWebHelp(vector<pair<string, string>> &help) const;

    //
    static string GetClassType() { return (VAPoR::SliceRenderer::GetClassType()); }
    string GetType() const { return GetClassType(); }

    virtual DimFlags GetDimFlags() const { return _variables->_variablesWidget->GetDimFlags(); }

  protected:
    void _updateTab();
    virtual string _getDescription() const;

    virtual string _getSmallIconImagePath() const { return ("Slice_small.png"); }
    virtual string _getIconImagePath() const { return ("Slice.png"); }

  private:
    //! Override default wheel behavior on the tab.  This has the effect of
    //! ignoring wheel events over the tab.  This is because wheel events will always
    //! affect the combo boxes and other widgets in the tab, and it would be confusing
    //! if wheel events also scrolled the tab itself
    void wheelEvent(QWheelEvent *) {}

    VLineComboBox *_vLineComboBox;
    VSliderEdit *_vSliderEdit;
    VLineItem *_vli;

    VVariablesContainer *_vVariablesContainer;

    VariablesWidget2 *_variablesWidget;
    VVariablesWidget *_vVariablesWidget;
    PVariablesWidget *_pVariablesWidget;
    PSimpleWidget *_pSimpleWidget;
    SimpleWidget *_simpleWidget;
    FidelityWidget3 *_fidelityWidget3;
    PFidelityWidget3 *_pFidelityWidget;
    PIntegerInput *_pIntegerInput;
    PDoubleInput *_pDoubleInput;

    //! VariablesWidget is used as Variables tab
    SliceVariablesSubtab *_variables;
    SliceGeometrySubtab *_geometry;
    GLSliceImageWindow *_glSliceImageWindow;
    SliceAppearanceSubtab *_appearance;
    SliceAnnotationSubtab *_annotation;

#ifdef VAPOR3_0_0_ALPHA
    SliceImageGUI *_image;
#endif
};

#endif // SLICEEVENTROUTER_H
