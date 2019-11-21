#ifndef FLOWSUBTABS_H
#define FLOWSUBTABS_H

#include "Flags.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <vapor/FlowParams.h>

#include <QLineEdit>

#include "ColorbarWidget.h"
#include "CopyRegionWidget.h"
#include "GeometryWidget.h"
#include "TFEditor.h"
#include "TransformTable.h"
#include "VariablesWidget.h"

#define UNSTEADY_STRING "Pathlines"
#define STEADY_STRING "Streamlines"
#define GRIDDED_STRING "Gridded"
#define LIST_STRING "List of seeds"
#define RANDOM_STRING "Random"

#define MIN_AXIS_SEEDS 1
#define MAX_AXIS_SEEDS 1000
#define MIN_RANDOM_SEEDS 1
#define MAX_RANODM_SEEDS 1000000

#define X 0
#define Y 1
#define Z 2
#define RANDOM_INDEX 4

class VLineEdit;
class VCheckBox;
class VComboBox;
class VSlider;
class VSliderEdit;
class VFileReader;
class VFileWriter;
class VGeometry;
class VFrame;
class VSpinBox;

namespace VAPoR {
class ControlExec;
class RenderParams;
class ParamsMgr;
class DataMgr;
} // namespace VAPoR

using VAPoR::FlowDir;
using VAPoR::FlowSeedMode;

class QVaporSubtab : public QWidget {
    Q_OBJECT

  public:
    QVaporSubtab(QWidget *parent);

  protected:
    QVBoxLayout *_layout;
};

//
//================================
//
class FlowVariablesSubtab : public QVaporSubtab {

    Q_OBJECT

  public:
    FlowVariablesSubtab(QWidget *parent);

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    VAPoR::FlowParams *_params;
    VariablesWidget *_variablesWidget;

    /*    VLineEdit*          _velocityMltp;  // Note on this widget: its name and associated
       functions
                                            // use the name "velocity multiplier," while it displays
                                            // "Field Scale Factor." They'll need to be reconciled
                                            // before the final merge.

        VCheckBox*          _periodicX;
        VCheckBox*          _periodicY;
        VCheckBox*          _periodicZ;*/

  private slots:
    // Respond to user input
    /*    void _velocityMultiplierChanged();
        void _periodicClicked();*/
};

//
//================================
//
class FlowAppearanceSubtab : public QVaporSubtab {

    Q_OBJECT

  public:
    FlowAppearanceSubtab(QWidget *parent);

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private:
    VAPoR::FlowParams *_params;
    TFEditor *_TFEditor;
};

//
//================================
//
class FlowSeedingSubtab : public QVaporSubtab {

    Q_OBJECT

  public:
    FlowSeedingSubtab(QWidget *parent);

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

  private slots:
    void _configureFlowType(const std::string &value = STEADY_STRING);
    void _configureSeedType(const std::string &value = GRIDDED_STRING);
    void _streamlineLengthChanged(int value);
    void _flowDirectionChanged();
    void _periodicClicked();
    void _pathlineLengthChanged(int value);
    void _velocityMultiplierChanged(const std::string &);
    void _rakeNumOfSeedsChanged();
    void _seedGenModeChanged(int newIdx);
    // Respond to user input
    /*void _fileReaderChanged();
    void _fileWriterChanged();

    void _rakeGeometryChanged();
    void _rakeNumOfSeedsChanged();
    void _rakeBiasVariableChanged( int );
    void _rakeBiasStrengthChanged();

    void _steadyGotClicked();
    void _pastNumOfTimeStepsChanged( int );
    void _seedInjIntervalChanged( int );

    void _selectedTabChanged(int index);*/

  private:
    void _createIntegrationSection();
    void _createSeedingSection();
    void _updateSteadyFlowWidgets(VAPoR::DataMgr *dataMgr);
    void _updateUnsteadyFlowWidgets(VAPoR::DataMgr *dataMgr);

    VAPoR::FlowParams *_params;
    VAPoR::ParamsMgr *_paramsMgr;

    // Integration options
    //
    VSection *_integrationSection;
    VComboBox *_flowTypeCombo;

    //  Pathline integration options
    VFrame *_pathlineWidgetFrame;
    VSliderEdit *_pathlineLengthSliderEdit;
    VComboBox *_pathlineDirectionCombo;

    //  Streamline integration options
    VFrame *_streamlineWidgetFrame;
    VSliderEdit *_streamlineLengthSliderEdit;
    VSliderEdit *_streamlineInjIntervalSliderEdit;
    VSliderEdit *_streamlineStartSliderEdit;
    VSliderEdit *_streamlineEndSliderEdit;
    VSliderEdit *_streamlineLifetimeSliderEdit;

    //  Universal integration options
    VCheckBox *_periodicXCheckBox;
    VCheckBox *_periodicYCheckBox;
    VCheckBox *_periodicZCheckBox;
    VLineEdit *_velocityMultiplierLineEdit;

    // Seed distribution options
    //
    VSection *_seedDistributionSection;
    VComboBox *_seedTypeCombo;

    //  Gridded seed distribution
    VFrame *_griddedSeedsFrame;
    VSpinBox *_xSeedSpinBox;
    VSpinBox *_ySeedSpinBox;
    VSpinBox *_zSeedSpinBox;
    VSliderEdit *_xSeedSliderEdit;
    VSliderEdit *_ySeedSliderEdit;
    VSliderEdit *_zSeedSliderEdit;

    //  Seeds read from a text file
    VFrame *_listOfSeedsFrame;
    VFileReader *_listOfSeedsFileReader;

    //  Random seed distribution
    VFrame *_randomSeedsFrame;
    // VSpinBox*               _randomSeedSpinBox;
    VSliderEdit *_randomSeedsSliderEdit;
    VComboBox *_biasVariableComboBox;
    VSliderEdit *_biasWeightSliderEdit;

    VFileWriter *_exportGeometryFileWriter;

    VSliderEdit *_pastNumOfTimeSteps;
    VSliderEdit *_seedInjInterval;

    VComboBox *_seedGenMode;
    VFileReader *_fileReader;
    VFileWriter *_fileWriter;

    // Rake related widgets
    VGeometry *_rake;
    VLineEdit *_rakeXNum, *_rakeYNum, *_rakeZNum, *_rakeTotalNum;
    VComboBox *_rakeBiasVariable;
    VSliderEdit *_rakeBiasStrength;

    /*    // Add some QT widgets
        VCheckBox*              _steady;
        VLineEdit*              _steadyNumOfSteps;
        VSliderEdit*            _pastNumOfTimeSteps;
        VSliderEdit*            _seedInjInterval;

        VComboBox*              _seedGenMode;
        VFileReader*            _fileReader;
        VFileWriter*            _fileWriter;
        VComboBox*              _flowDirection;

        // Rake related widgets
        VGeometry*              _rake;
        VLineEdit              *_rakeXNum, *_rakeYNum, *_rakeZNum, *_rakeTotalNum;
        VComboBox*              _rakeBiasVariable;
        VSliderEdit*            _rakeBiasStrength;

        QFrame                  *_hline1, *_hline2;     // horizontal lines

        // Helper functions
        void _hideShowWidgets(); // hide and show widgets based on the current seed generation mode.
    */
};

//
//================================
//
class FlowGeometrySubtab : public QVaporSubtab {

    Q_OBJECT

  public:
    FlowGeometrySubtab(QWidget *parent);

    void Update(VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr, VAPoR::RenderParams *rParams);

  private:
    VAPoR::FlowParams *_params;
    GeometryWidget *_geometryWidget;
    CopyRegionWidget *_copyRegionWidget;
    TransformTable *_transformTable;
};

//
//================================
//
class FlowAnnotationSubtab : public QVaporSubtab {

    Q_OBJECT

  public:
    FlowAnnotationSubtab(QWidget *parent);

    void Update(VAPoR::ParamsMgr *paramsMgr, VAPoR::DataMgr *dataMgr, VAPoR::RenderParams *rParams);

  private:
    ColorbarWidget *_colorbarWidget;
};

#endif // FLOWSUBTABS_H
