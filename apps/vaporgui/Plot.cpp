//************************************************************************
//                                                                      *
//           Copyright (C)  2016                                        *
//     University Corporation for Atmospheric Research                  *
//           All Rights Reserved                                        *
//                                                                      *
//************************************************************************/
//
//  File:       plot.h
//
//  Author:     Samuel Li
//              National Center for Atmospheric Research
//              PO 3000, Boulder, Colorado
//
//  Date:       January 2018
//

#include "Plot.h"
#include "GUIStateParams.h"
#include <vapor/GetAppPath.h>

#define NPY_NO_DEPRECATED_API NPY_1_8_API_VERSION
#include <numpy/ndarrayobject.h>

// Constructor
Plot::Plot(VAPoR::DataStatus *status, VAPoR::ParamsMgr *manager, QWidget *parent) {
    _dataStatus = status;
    _paramsMgr = manager;

    // Store the active dataset name
    std::vector<std::string> dmNames = _dataStatus->GetDataMgrNames();
    if (dmNames.empty()) {
        std::cerr << "No data set chosen yet. Plot shouldn't run into this condition." << std::endl;
    } else {
        GUIStateParams *guiParams =
            dynamic_cast<GUIStateParams *>(_paramsMgr->GetParams(GUIStateParams::GetClassType()));
        std::string dsName = guiParams->GetStatsDatasetName();
        if (dsName == "" || dsName == "NULL") // not initialized yet
            guiParams->SetPlotDatasetName(dmNames[0]);
    }

    // Do some QT stuff
    setupUi(this);
    setWindowTitle("Plot Utility");
    p1P2Widget->setTabText(0, QString::fromAscii("Point 1 Position"));
    p1P2Widget->setTabText(1, QString::fromAscii("Point 2 Position"));
    myFidelityWidget->Reinit(FidelityWidget::AUXILIARY);

    // Connect signals with slots
    connect(newVarCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(_newVarChanged(int)));
    connect(removeVarCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(_removeVarChanged(int)));

    // Put the current window on top
    show();
    raise();
    activateWindow();
}

// Destructor
Plot::~Plot() {
    _dataStatus = NULL;
    _paramsMgr = NULL;
}

void Plot::Update() {
    // Initialize pointers
    std::vector<std::string> dmNames = _dataStatus->GetDataMgrNames();
    if (dmNames.empty()) {
        this->close();
    }
    GUIStateParams *guiParams =
        dynamic_cast<GUIStateParams *>(_paramsMgr->GetParams(GUIStateParams::GetClassType()));
    std::string currentDatasetName = guiParams->GetPlotDatasetName();
    assert(currentDatasetName != "" && currentDatasetName != "NULL");
    int currentIdx = -1;
    for (int i = 0; i < dmNames.size(); i++)
        if (currentDatasetName == dmNames[i]) {
            currentIdx = i;
            break;
        }
    if (currentIdx == -1) // currentDatasetName is closed!!!
    {
        currentDatasetName = dmNames[0];
        currentIdx = 0;
        guiParams->SetPlotDatasetName(currentDatasetName);
    }
    VAPoR::DataMgr *currentDmgr = _dataStatus->GetDataMgr(currentDatasetName);
    PlotParams *plotParams = dynamic_cast<PlotParams *>(
        _paramsMgr->GetAppRenderParams(currentDatasetName, PlotParams::GetClassType()));
    std::vector<std::string> enabledVars = plotParams->GetAuxVariableNames();

    // Update DataMgrCombo
    dataMgrCombo->blockSignals(true);
    dataMgrCombo->clear();
    for (int i = 0; i < dmNames.size(); i++)
        dataMgrCombo->addItem(QString::fromStdString(dmNames[i]));
    dataMgrCombo->setCurrentIndex(currentIdx);
    dataMgrCombo->blockSignals(false);

    // Update "Add a Variable"
    std::vector<std::string> availVars = currentDmgr->GetDataVarNames(2, true);
    std::vector<std::string> availVars3D = currentDmgr->GetDataVarNames(3, true);
    for (int i = 0; i < availVars3D.size(); i++)
        availVars.push_back(availVars3D[i]);
    for (int i = 0; i < enabledVars.size(); i++)
        for (int rmIdx = 0; rmIdx < availVars.size(); rmIdx++)
            if (availVars[rmIdx] == enabledVars[i]) {
                availVars.erase(availVars.begin() + rmIdx);
                break;
            }
    std::sort(availVars.begin(), availVars.end());
    newVarCombo->blockSignals(true);
    newVarCombo->clear();
    newVarCombo->addItem(QString::fromAscii("Add a Variable"));
    for (std::vector<std::string>::iterator it = availVars.begin(); it != availVars.end(); ++it)
        newVarCombo->addItem(QString::fromStdString(*it));
    newVarCombo->setCurrentIndex(0);
    newVarCombo->blockSignals(false);

    // Update "Remove a Variable"
    std::sort(enabledVars.begin(), enabledVars.end());
    removeVarCombo->blockSignals(true);
    removeVarCombo->clear();
    removeVarCombo->addItem(QString::fromAscii("Remove a Variable"));
    for (int i = 0; i < enabledVars.size(); i++)
        removeVarCombo->addItem(QString::fromStdString(enabledVars[i]));
    removeVarCombo->setCurrentIndex(0);
    removeVarCombo->blockSignals(false);

    // Update "Variable Table"
    variablesTable->clear(); // This also deletes the items properly.
    QStringList header;      // Start from the header
    header << "Enabled Variables";
    variablesTable->setColumnCount(header.size());
    variablesTable->setHorizontalHeaderLabels(header);
    variablesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    variablesTable->horizontalHeader()->setFixedHeight(30);
    variablesTable->verticalHeader()->setFixedWidth(30);

    variablesTable->setRowCount(enabledVars.size()); // Then work on the cells
    for (int row = 0; row < enabledVars.size(); row++) {
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromStdString(enabledVars[row]));
        item->setFlags(Qt::NoItemFlags);
        item->setTextAlignment(Qt::AlignCenter);
        variablesTable->setItem(row, 0, item);
    }
    variablesTable->update();
    variablesTable->repaint();
    variablesTable->viewport()->update();

    // Update LOD, Refinement
    myFidelityWidget->Update(currentDmgr, _paramsMgr, plotParams);
}

void Plot::_newVarChanged(int index) {
    assert(index > 0);

    // Initialize pointers
    GUIStateParams *guiParams =
        dynamic_cast<GUIStateParams *>(_paramsMgr->GetParams(GUIStateParams::GetClassType()));
    std::string dsName = guiParams->GetPlotDatasetName();
    PlotParams *plotParams = dynamic_cast<PlotParams *>(
        _paramsMgr->GetAppRenderParams(dsName, PlotParams::GetClassType()));
    std::string varName = newVarCombo->itemText(index).toStdString();

    // Add this variable to parameter
    std::vector<std::string> vars = plotParams->GetAuxVariableNames();
    vars.push_back(varName);
    plotParams->SetAuxVariableNames(vars);
}

void Plot::_removeVarChanged(int index) {
    assert(index > 0);

    // Initialize pointers
    GUIStateParams *guiParams =
        dynamic_cast<GUIStateParams *>(_paramsMgr->GetParams(GUIStateParams::GetClassType()));
    std::string dsName = guiParams->GetPlotDatasetName();
    PlotParams *plotParams = dynamic_cast<PlotParams *>(
        _paramsMgr->GetAppRenderParams(dsName, PlotParams::GetClassType()));
    std::string varName = removeVarCombo->itemText(index).toStdString();

    // Remove this variable from parameter
    std::vector<std::string> vars = plotParams->GetAuxVariableNames();
    int rmIdx = -1;
    for (int i = 0; i < vars.size(); i++)
        if (vars[i] == varName) {
            rmIdx = i;
            break;
        }
    assert(rmIdx != -1);
    vars.erase(vars.begin() + rmIdx);
    plotParams->SetAuxVariableNames(vars);
}

void Plot::_dataSourceChanged(int) {}

void Plot::_plotClicked() {}

void Plot::_spaceTimeModeChanged(bool) {}

void Plot::_spaceModeP1P2Changed() {}

void Plot::_spaceModeTimeChanged() {}

void Plot::_timeModePointChanged() {}

void Plot::_timeModeT1T2Changed() {}

void Plot::_fidelityChanged() {}
