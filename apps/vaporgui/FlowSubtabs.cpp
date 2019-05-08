#include "FlowSubtabs.h"
#include "ColorbarWidget.h"
#include "CopyRegionWidget.h"
#include "GeometryWidget.h"
#include "TFWidget.h"
#include "TransformTable.h"
#include "VaporWidgets.h"
#include "VariablesWidget.h"

QVaporSubtab::QVaporSubtab(QWidget *parent) : QWidget(parent) {
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->insertSpacing(-1, 20);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
}

//
//================================
//
FlowVariablesSubtab::FlowVariablesSubtab(QWidget *parent) : QVaporSubtab(parent) {
    _variablesWidget = new VariablesWidget(this);
    _variablesWidget->Reinit((VariableFlags)(VECTOR | COLOR), (DimFlags)(THREED));
    _layout->addWidget(_variablesWidget, 0, 0);

    _velocityMltp = new QLineEdit(this);
    _layout->addWidget(_velocityMltp);

    _steady = new VCheckBox(this, "Use Steady Flow");
    _layout->addWidget(_steady);

    _steadyNumOfSteps = new QLineEdit(this);
    _layout->addWidget(_steadyNumOfSteps);

    connect(_steady, SIGNAL(_checkboxClicked()), this, SLOT(_steadyGotClicked()));
    connect(_velocityMltp, SIGNAL(editingFinished()), this, SLOT(_velocityMultiplierChanged()));
    connect(_steadyNumOfSteps, SIGNAL(editingFinished()), this, SLOT(_steadyNumOfStepsChanged()));
}

void FlowVariablesSubtab::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                                 VAPoR::RenderParams *rParams) {
    _params = dynamic_cast<VAPoR::FlowParams *>(rParams);
    assert(_params);
    _variablesWidget->Update(dataMgr, paramsMgr, rParams);

    // Update custom widgets
    bool isSteady = _params->GetIsSteady();
    _steady->SetCheckState(isSteady);

    auto mltp = _params->GetVelocityMultiplier();
    _velocityMltp->setText(QString::number(mltp, 'f', 3));

    int numOfSteps = _params->GetSteadyNumOfSteps();
    _steadyNumOfSteps->setText(QString::number(numOfSteps));
}

void FlowVariablesSubtab::_steadyGotClicked() {
    bool userInput = _steady->GetCheckState();
    _params->SetIsSteady(userInput);
}

void FlowVariablesSubtab::_velocityMultiplierChanged() {
    bool ok;
    double d = _velocityMltp->text().toDouble(&ok);
    if (ok) // We don't need this verification once the line edit has its own validator
        _params->SetVelocityMultiplier(d);
}

void FlowVariablesSubtab::_steadyNumOfStepsChanged() {
    bool ok;
    int i = _steadyNumOfSteps->text().toInt(&ok);
    if (ok) // We don't need this verification once the line edit has its own validator
        _params->SetSteadyNumOfSteps(i);
}

//
//================================
//
FlowAppearanceSubtab::FlowAppearanceSubtab(QWidget *parent) : QVaporSubtab(parent) {
    _streamlineAppearanceTab = new VTabWidget(this, "Streamline Appearance Settings");

    _shapeCombo = new VComboBox(this, "Integration type");
    _shapeCombo->AddOption("Tube", 0);
    _shapeCombo->AddOption("Points", 1);
    _shapeCombo->AddOption("Arrows", 2);
    _streamlineAppearanceTab->AddWidget(_shapeCombo);

    _colorCombo = new VComboBox(this, "Color");
    _colorCombo->AddOption("Constant");
    _colorCombo->AddOption("Color mapped variable");
    _colorCombo->AddOption("Distance from start");
    _colorCombo->AddOption("Seed index");
    _streamlineAppearanceTab->AddWidget(_colorCombo);

    _lengthSpinBox = new VSpinBox(this, "Length");
    _streamlineAppearanceTab->AddWidget(_lengthSpinBox);

    _sizeSpinBox = new VSpinBox(this, "Size");
    _streamlineAppearanceTab->AddWidget(_sizeSpinBox);

    _smoothnessSliderEdit = new QSliderEdit(this);
    _smoothnessSliderEdit->SetLabel("Smoothness");
    _streamlineAppearanceTab->AddWidget(_smoothnessSliderEdit);
    _layout->addWidget(_streamlineAppearanceTab);

    _TFWidget = new TFWidget(this);
    _TFWidget->Reinit((TFFlags)(SAMPLING | CONSTANT_COLOR));

    //_layout->addWidget( _TFWidget, 0, 0 );
    _layout->addWidget(_TFWidget);

    _params = NULL;
}

void FlowAppearanceSubtab::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                                  VAPoR::RenderParams *rParams) {
    _params = dynamic_cast<VAPoR::FlowParams *>(rParams);
    assert(_params);

    _TFWidget->Update(dataMgr, paramsMgr, rParams);
}

//
//================================
//
FlowIntegrationSubtab::FlowIntegrationSubtab(QWidget *parent)
    : QVaporSubtab(parent), _dataMgr(nullptr), _paramsMgr(nullptr), _params(nullptr),
      _initialized(false) {
    _integrationSettingsTab = new VTabWidget(this, "Flow Integration Settings");

    _integrationTypeCombo = new VComboBox(this, "Integration type");
    _integrationTypeCombo->AddOption("Steady", 0);
    _integrationTypeCombo->AddOption("Unsteady", 1);
    _integrationSettingsTab->AddWidget(_integrationTypeCombo);
    connect(_integrationTypeCombo, SIGNAL(_indexChanged(int)), this,
            SLOT(_configureIntegrationType()));

    _multiplierLineEdit = new VLineEdit(this, "Vector field multiplier");
    _integrationSettingsTab->AddWidget(_multiplierLineEdit);
    connect(_multiplierLineEdit, SIGNAL(_editingFinished()), this, SLOT(_multiplierChanged()));

    _integrationLengthEdit = new VLineEdit(this, "Integration Length/Steps/Multiplier");
    _integrationSettingsTab->AddWidget(_integrationLengthEdit);

    _directionCombo = new VComboBox(this, "Integration direction");
    _directionCombo->AddOption("Forward", 0);
    _directionCombo->AddOption("Backward", 1);
    _directionCombo->AddOption("Bi-directional", 2);
    _integrationSettingsTab->AddWidget(_directionCombo);

    _startSpinBox = new VSpinBox(this, "Injection start time step", 0);
    _integrationSettingsTab->AddWidget(_startSpinBox);
    _endSpinBox = new VSpinBox(this, "Injection end time step");
    _integrationSettingsTab->AddWidget(_endSpinBox);
    _lifespanSpinBox = new VSpinBox(this, "Seed lifespan after injection");
    _integrationSettingsTab->AddWidget(_lifespanSpinBox);
    _intervalSpinBox = new VSpinBox(this, "Seed injection interval", 1);
    _integrationSettingsTab->AddWidget(_intervalSpinBox);

    _periodicBoundaryComboX = new VCheckBox(this, "X axis periodicity");
    _integrationSettingsTab->AddWidget(_periodicBoundaryComboX);
    _periodicBoundaryComboY = new VCheckBox(this, "Y axis periodicity");
    _integrationSettingsTab->AddWidget(_periodicBoundaryComboY);
    _periodicBoundaryComboZ = new VCheckBox(this, "Z axis periodicity");
    _integrationSettingsTab->AddWidget(_periodicBoundaryComboZ);

    _configureIntegrationType();
    _layout->addWidget(_integrationSettingsTab);
}

void FlowIntegrationSubtab::_multiplierChanged() {
    double value = (double)_multiplierLineEdit->GetEditText();
    std::cout << "Vector multiplier line edit changed to " << value << std::endl;
    _params->SetVelocityMultiplier(value);
    std::cout << "Vector multiplier line edit changed to " << _params->GetVelocityMultiplier()
              << std::endl;
}

void FlowIntegrationSubtab::_configureIntegrationType() {
    string seedType = _integrationTypeCombo->GetCurrentText();
    if (seedType == "Steady") {
        _params->SetIsSteady(true);
        _startSpinBox->hide();
        _endSpinBox->hide();
        _lifespanSpinBox->hide();
        _intervalSpinBox->hide();
        _directionCombo->show();
        _integrationLengthEdit->show();
    } else {
        _params->SetIsSteady(false);
        _startSpinBox->show();
        _endSpinBox->show();
        _lifespanSpinBox->show();
        _intervalSpinBox->show();
        _directionCombo->hide();
        _integrationLengthEdit->hide();
    }
}

void FlowIntegrationSubtab::_initialize() {
    int numTimeSteps = _dataMgr->GetNumTimeSteps();
    _endSpinBox->SetValue(numTimeSteps);
    _lifespanSpinBox->SetValue(numTimeSteps);
}

void FlowIntegrationSubtab::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                                   VAPoR::RenderParams *rParams) {
    _dataMgr = dataMgr;
    _paramsMgr = paramsMgr;
    _params = dynamic_cast<VAPoR::FlowParams *>(rParams);

    if (!_initialized) {
        _initialize();
        _initialized = true;
    }
}

//
//================================
//
FlowSeedingSubtab::FlowSeedingSubtab(QWidget *parent) : QVaporSubtab(parent) {
    _seedSettingsTab = new VTabWidget(this, "Seed Distribution Settings");
    _distributionCombo = new VComboBox(this, "Seed distribution type");
    _distributionCombo->AddOption("Gridded", 0);
    _distributionCombo->AddOption("Random", 1);
    _distributionCombo->AddOption("List of points", 2);
    _seedSettingsTab->AddWidget(_distributionCombo);
    connect(_distributionCombo, SIGNAL(_indexChanged(int)), this, SLOT(_configureRakeType()));

    // Random rake options
    _randomCountSpinBox = new VSpinBox(this, "Number of random seeds", 64);
    _seedSettingsTab->AddWidget(_randomCountSpinBox);
    _biasVariableCombo = new VComboBox(this, "Random distribution bias variable");
    _seedSettingsTab->AddWidget(_biasVariableCombo);
    _biasSliderEdit = new QSliderEdit(this);
    _biasSliderEdit->SetLabel("Bias weight");
    _seedSettingsTab->AddWidget(_biasSliderEdit);

    // Gridded rake options
    _xDistributionSpinBox = new VSpinBox(this, "X axis seeds", 8);
    _seedSettingsTab->AddWidget(_xDistributionSpinBox);
    _yDistributionSpinBox = new VSpinBox(this, "Y axis seeds", 8);
    _seedSettingsTab->AddWidget(_yDistributionSpinBox);
    _zDistributionSpinBox = new VSpinBox(this, "Z axis seeds", 8);
    _seedSettingsTab->AddWidget(_zDistributionSpinBox);

    // List of seed file picker
    _fileReader = new VFileReader(this, "Seed File");
    _seedSettingsTab->AddWidget(_fileReader);
    _layout->addWidget(_seedSettingsTab);

    // Rake region selector
    _geometryWidget = new GeometryWidget(this);
    _geometryWidget->Reinit((DimFlags)THREED, (VariableFlags)VECTOR);
    _layout->addWidget(_geometryWidget);

    _configureRakeType();

    _exportGeometryDialog =
        new VFileWriter(this, "Export geometry", "Select", QDir::homePath().toStdString());
    _layout->addWidget(_exportGeometryDialog);
}

void FlowSeedingSubtab::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                               VAPoR::RenderParams *rParams) {
    // VAPoR::Box* rakeBox = rParams->GetRakeBox();
    //_geometryWidget->Update(paramsMgr, dataMgr, rParams, rakeBox);
    _geometryWidget->Update(paramsMgr, dataMgr, rParams);
}

void FlowSeedingSubtab::_configureRakeType() {
    string seedType = _distributionCombo->GetCurrentText();
    if (seedType == "Random") {
        _randomCountSpinBox->show();
        _biasVariableCombo->show();
        _biasSliderEdit->show();

        _xDistributionSpinBox->hide();
        _yDistributionSpinBox->hide();
        _zDistributionSpinBox->hide();

        _fileReader->hide();

        _geometryWidget->setEnabled(true);
    } else if (seedType == "Gridded") {
        _randomCountSpinBox->hide();
        _biasVariableCombo->hide();
        _biasSliderEdit->hide();

        _xDistributionSpinBox->show();
        _yDistributionSpinBox->show();
        _zDistributionSpinBox->show();

        _fileReader->hide();

        _geometryWidget->setEnabled(true);
    } else { // ( seedType == "List of points" )
        _randomCountSpinBox->hide();
        _biasVariableCombo->hide();
        _biasSliderEdit->hide();

        _xDistributionSpinBox->hide();
        _yDistributionSpinBox->hide();
        _zDistributionSpinBox->hide();

        _fileReader->show();

        _geometryWidget->setEnabled(false);
    }
}

void FlowSeedingSubtab::_pushTestPressed() { cout << "Push button pressed" << endl; }

void FlowSeedingSubtab::_comboBoxSelected(int index) {
    string option = "*** Need to turn on _comboTest at FlowSubtabs.cpp:107";
    cout << "Combo selected at index " << index << " for option " << option << endl;
}

void FlowSeedingSubtab::_checkBoxSelected() {
    bool checked = 0; //_checkboxTest->GetCheckState();
    cout << "Checkbox is checked? " << checked << endl;
}

//
//================================
//
FlowGeometrySubtab::FlowGeometrySubtab(QWidget *parent) : QVaporSubtab(parent) {
    _geometryWidget = new GeometryWidget(this);
    _copyRegionWidget = new CopyRegionWidget(this);
    _transformTable = new TransformTable(this);
    _geometryWidget->Reinit((DimFlags)THREED, (VariableFlags)VECTOR);

    _layout->addWidget(_geometryWidget, 0, 0);
    _layout->addWidget(_copyRegionWidget, 0, 0);
    _layout->addWidget(_transformTable, 0, 0);

    _params = NULL;
}

void FlowGeometrySubtab::Update(VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr,
                                VAPoR::RenderParams *rParams) {
    _params = dynamic_cast<VAPoR::FlowParams *>(rParams);
    assert(_params);

    _geometryWidget->Update(paramsMgr, dataMgr, rParams);
    _copyRegionWidget->Update(paramsMgr, rParams);
    _transformTable->Update(rParams->GetTransform());
}

//
//================================
//
FlowAnnotationSubtab::FlowAnnotationSubtab(QWidget *parent) : QVaporSubtab(parent) {
    _colorbarWidget = new ColorbarWidget(this);
    _layout->addWidget(_colorbarWidget, 0, 0);
}

void FlowAnnotationSubtab::Update(VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr,
                                  VAPoR::RenderParams *rParams) {
    _colorbarWidget->Update(dataMgr, paramsMgr, rParams);
}
