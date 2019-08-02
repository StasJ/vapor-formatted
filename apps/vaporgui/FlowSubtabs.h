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
#include "TFWidget.h"
#include "TransformTable.h"
#include "VaporWidgets.h"
#include "VariablesWidget.h"

namespace VAPoR {
class ControlExec;
class RenderParams;
class ParamsMgr;
class DataMgr;
} // namespace VAPoR

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

    VLineEdit *_velocityMltp;

    VCheckBox *_steady;

    VLineEdit *_steadyNumOfSteps;

    VCheckBox *_periodicX;
    VCheckBox *_periodicY;
    VCheckBox *_periodicZ;

  private slots:
    // Respond to user input
    void _steadyGotClicked();
    void _velocityMultiplierChanged();
    void _steadyNumOfStepsChanged();
    void _periodicClicked();
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
    TFWidget *_TFWidget;
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
    /* Respond to user input */
    void _seedGenModeChanged(int newIdx);
    void _fileReaderChanged();
    void _fileWriterChanged();
    void _flowDirectionChanged(int newIdx);
    void _outputButtonClicked();

    void _rakeGeometryChanged();
    void _rakeNumOfSeedsChanged();

  private:
    VAPoR::FlowParams *_params;

    /* Add some QT widgets */
    VComboBox *_seedGenMode;
    VFileReader *_fileReader;
    VFileWriter *_fileWriter;
    QPushButton *_outputButton;
    VComboBox *_flowDirection;

    /* Rake related widgets */
    VGeometry *_rake;
    VLineEdit *_rakeXNum, *_rakeYNum, *_rakeZNum, *_rakeTotalNum;

    /* Helper functions */
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
