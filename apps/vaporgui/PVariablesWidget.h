#pragma once

#include "PFidelityWidget.h"
#include "PLineItem.h"
#include "PVariableSelector.h"
class VComboBox;

class PDimensionSelector : public PLineItem {
    Q_OBJECT
    VComboBox *_vComboBox;

  public:
    PDimensionSelector();

  protected:
    virtual void updateGUI() const override;
    bool requireDataMgr() const override { return true; }

  private slots:
    void dropdownTextChanged(std::string text);
};

class PScalarVariableSelector : public PVariableSelector {
  public:
    PScalarVariableSelector();
};
class PColorMapVariableSelector : public PVariableSelector {
  public:
    PColorMapVariableSelector();
};
class PHeightVariableSelector : public PVariableSelector2D {
  public:
    PHeightVariableSelector();
};
class PXFieldVariableSelector : public PVariableSelector {
  public:
    PXFieldVariableSelector();
};
class PYFieldVariableSelector : public PVariableSelector {
  public:
    PYFieldVariableSelector();
};
class PZFieldVariableSelector : public PVariableSelector {
  public:
    PZFieldVariableSelector();
};
