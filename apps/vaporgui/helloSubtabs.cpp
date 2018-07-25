#include "helloSubtabs.h"
#include <vapor/HelloParams.h>

using namespace VAPoR;

HelloVariablesSubtab::HelloVariablesSubtab(QWidget *parent)
    : QWidget(parent), Ui_HelloVariablesGUI() {

    setupUi(this);

    _variablesWidget->Reinit(
        (VariablesWidget::DisplayFlags)(VariablesWidget::SCALAR | VariablesWidget::HGT),
        (VariablesWidget::THREED));
}
