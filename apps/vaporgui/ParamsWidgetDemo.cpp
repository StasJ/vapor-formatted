#include "ParamsWidgetDemo.h"
#include "GUIStateParams.h"
#include <QVBoxLayout>
#include <vapor/ParamsBase.h>
#include <vapor/ParamsMgr.h>
#include <vapor/VAssert.h>

#include "PCheckbox.h"
#include "PColorSelector.h"
#include "PDisplay.h"
#include "PDoubleInput.h"
#include "PEnumDropdown.h"
#include "PEnumDropdownHLI.h"
#include "PFileSelector.h"
#include "PFileSelectorHLI.h"
#include "PGroup.h"
#include "PIntegerInput.h"
#include "PSection.h"
#include "PSliderEdit.h"
#include "PStringDropdown.h"

#include <QTabWidget>

ParamsWidgetDemo::ParamsWidgetDemo() {
    setWindowTitle("Params Widget Demo");
    setLayout(new QVBoxLayout);

    PSection *section;

    QTabWidget *qtw = new QTabWidget();
    layout()->addWidget(qtw);

    PDoubleSliderEdit *pdse = new PDoubleSliderEdit("demo_double", "foo");
    qtw->addTab(pdse, "foo");

    PDoubleSliderEdit *pdse2 = new PDoubleSliderEdit("demo_double", "bar");
    qtw->addTab(pdse2, "bar");

    layout()->addWidget(pg = new PGroup);

    section = new PSection("Numbers");
    section->Add(new PIntegerInput("demo_int", "PIntegerInput"));
    section->Add(new PDoubleInput("demo_double", "PDoubleInput"));
    section->Add((new PIntegerSliderEdit("demo_int", "PIntegerSliderEdit"))->SetRange(0, 100));
    section->Add((new PDoubleSliderEdit("demo_double", "Dynamic Double"))->EnableDynamicUpdate());
    pg->Add(section);

    section = new PSection("Files");
    section->Add(new PFileOpenSelector("demo_path", "Open File"));
    section->Add(new PFileSaveSelector("demo_path", "Save File"));
    section->Add(new PDirectorySelector("demo_path", "Select Directory"));
    pg->Add(section);

    section = new PSection("Show or Enable");
    section->Add(new PCheckbox("demo_bool", "Enable Dropdown"));
    section->Add(
        (new PEnumDropdown("demo_drop", {"Double Input", "Color Input"}, {}, "Show Widget"))
            ->EnableBasedOnParam("demo_bool"));
    section->Add(
        (new PDoubleSliderEdit("demo_double", "Double"))->ShowBasedOnParam("demo_drop", 0));
    section->Add((new PColorSelector("demo_color", "Color"))->ShowBasedOnParam("demo_drop", 1));
    pg->Add(section);

    section = new PSection("Other");
    section->Add(new PColorSelector("demo_color", "Color"));
    section->Add(new PStringDropdown("demo_path", {"One", "Two", "Three"}, "String Dropdown"));
    section->Add(
        new PStringDropdown("demo_var",
                            {"Automatically", "presents", "2D or 3D", "Variables", "----------",
                             "Only works for", "RenderParams", "so cannot", "demo"},
                            "PVariableSelector"));
    section->Add((new PCheckbox("demo_bool", "Tooltips"))->SetTooltip("This is a tooltip"));
    pg->Add(section);

    section = new PSection("Labels");
    section->Add(new PIntegerDisplay("demo_int", "PIntegerDisplay"));
    section->Add(new PDoubleDisplay("demo_double", "PDoubleDisplay"));
    section->Add(new PStringDisplay("demo_path", "PStringDisplay"));
    section->Add(new PBooleanDisplay("demo_bool", "PBooleanDisplay"));
    pg->Add(section);

    section = new PSection("High Level Interface");
    section->Add(new_PDirectorySelectorHLI("Image Output Dir", &GUIStateParams::GetCurrentImagePath,
                                           &GUIStateParams::SetCurrentImagePath));
    section->Add(new PEnumDropdownHLI<GUIStateParams>("Flow Dimensions", {"2", "3"}, {2, 3},
                                                      &GUIStateParams::GetFlowDimensionality,
                                                      &GUIStateParams::SetFlowDimensionality));
    pg->Add(section);
}

void ParamsWidgetDemo::Update(VAPoR::ParamsBase *params, VAPoR::ParamsMgr *paramsMgr,
                              VAPoR::DataMgr *dataMgr) {
    pg->Update(params, paramsMgr);
}
