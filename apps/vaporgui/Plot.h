//************************************************************************
//                                                                      *
//           Copyright (C)  2016                                        *
//     University Corporation for Atmospheric Research                  *
//           All Rights Reserved                                        *
//                                                                      *
//************************************************************************/
//
//  File:       Plot.h
//
//  Author:     Samuel Li
//              National Center for Atmospheric Research
//              PO 3000, Boulder, Colorado
//
//  Date:       January 2018
//

#ifndef PLOT_H
#define PLOT_H

#include "PlotParams.h"
#include "ui_plotWindow.h"
#include <QWidget>
#include <vapor/DataStatus.h>
#include <vapor/ParamsMgr.h>
#include <vector>

using namespace VAPoR;

class Plot : public QDialog, public Ui_PlotWindow {
    Q_OBJECT

  public:
    Plot(VAPoR::DataStatus *status, VAPoR::ParamsMgr *manager, QWidget *parent = 0);
    ~Plot();

    /// This is called whenever there's a change to the parameters.
    void Update();

  private slots:
    /// Update list of enabled variables upon add/remove events
    void _newVarChanged(int);
    void _removeVarChanged(int);

    /// Clean up everything when data source is changed
    void _dataSourceChanged(int);

    /// Clean up data points for plotting when the following events happen
    void _spaceTimeModeChanged(int);
    void _spaceModeP1P2Changed();
    void _spaceModeTimeChanged();
    void _timeModePointChanged();
    void _timeModeT1T2Changed();
    void _fidelityChanged();

    /// Plot when the plot button is clicked
    void _plotClicked();

  private:
    VAPoR::DataStatus *_dataStatus;
    VAPoR::ParamsMgr *_paramsMgr;

    PlotParams *_getCurrentPlotParams() const;
};

#endif // PLOT_H
