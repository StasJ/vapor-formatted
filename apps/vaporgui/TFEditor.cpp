#include "TFEditor.h"
#include "QRangeSlider.h"
#include "QRangeSliderTextCombo.h"
#include "TFColorInfoWidget.h"
#include "TFColorWidget.h"
#include "TFHistogramWidget.h"
#include "TFOpacityInfoWidget.h"
#include "TFOpacityWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <vapor/ColorMap.h>
#include <vapor/ResourcePath.h>

TFEditor::TFEditor() {
    addTab(new QWidget(this), "Transfer Function");

    QVBoxLayout *layout = new QVBoxLayout;
    //    layout->setSpacing(0);
    layout->setMargin(12);
    _tab()->setLayout(layout);

    std::string stylesheet = R"(
    QToolButton {
        border: none;
        background-color: none;
        padding: 0px;
    }
    )";

    _tool = new QToolButton(this);
#define ABSOLUTE 0
#define CORNER 1
#if ABSOLUTE
#else
#if CORNER
    setCornerWidget(_tool);

    stylesheet +=
        R"(
    QTabWidget::right-corner {
        top: 20px;
        right: 5px;
    }
    )";
#else
    QHBoxLayout *l2 = new QHBoxLayout;
    QWidget *w = new QWidget;
    w->setLayout(l2);
    l2->setMargin(0);
    l2->setAlignment(Qt::AlignRight);
    l2->addWidget(_tool);
    //    layout->setAlignment(Qt::AlignRight);
    //    layout->addWidget(tool);
    layout->addWidget(w);

    int left, top, right, bottom;
    layout->getContentsMargins(&left, &top, &right, &bottom);
    layout->setContentsMargins(left, 0, right, bottom);
#endif
#endif
    _tool->setIcon(QIcon(QString::fromStdString(GetSharePath("images/gear.png"))));
    _tool->setIconSize(QSize(13, 13));
    _tool->setCursor(QCursor(Qt::PointingHandCursor));
    setStyleSheet(QString::fromStdString(stylesheet));

    layout->addWidget(_maps = new TFMapsGroup);
    layout->addWidget(_mapsInfo = _maps->CreateInfoGroup());
    layout->addWidget(range = new QRangeSliderTextCombo);
    layout->addWidget(colorMapTypeDropdown = new ParamsWidgetDropdown(
                          VAPoR::ColorMap::_interpTypeTag, {"Linear", "Discrete", "Diverging"},
                          "Color Interpolation"));

    connect(range, SIGNAL(ValueChanged(float, float)), this, SLOT(_rangeChanged(float, float)));

    //    this->setStyleSheet(R"(QWidget:hover:!pressed {border: 1px solid red;})");
}

void TFEditor::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                      VAPoR::RenderParams *rParams) {
    _rParams = rParams;
    colorMapTypeDropdown->Update(rParams->GetMapperFunc(rParams->GetVariableName())->GetColorMap());
    _maps->Update(dataMgr, paramsMgr, rParams);
    _mapsInfo->Update(rParams);
    _updateMappingRangeControl(dataMgr, paramsMgr, rParams);
}

void TFEditor::mousePressEvent(QMouseEvent *event) { printf("CLICK\n"); }

#include <QResizeEvent>
void TFEditor::resizeEvent(QResizeEvent *event) {
    QTabWidget::resizeEvent(event);
#if ABSOLUTE
    _tool->move(event->size().width() - _tool->size().width() - 5, 15);
#endif

#define PR(r) printf("%s = %i, %i, %i, %i\n", #r, r.x(), r.y(), r.width(), r.height())
    QRect geom = tabBar()->geometry();
    PR(geom);
}

QWidget *TFEditor::_tab() const { return this->widget(0); }

void TFEditor::_updateMappingRangeControl(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                                          VAPoR::RenderParams *rParams) {
    float min, max;
    _getDataRange(dataMgr, rParams, &min, &max);
    range->SetRange(min, max);
    vector<double> mapperRange =
        rParams->GetMapperFunc(rParams->GetVariableName())->getMinMaxMapValue();
    range->SetValue(mapperRange[0], mapperRange[1]);
}

void TFEditor::_getDataRange(VAPoR::DataMgr *d, VAPoR::RenderParams *r, float *min,
                             float *max) const {
    std::vector<double> range;
    d->GetDataRange(r->GetCurrentTimestep(), r->GetVariableName(), r->GetRefinementLevel(),
                    r->GetCompressionLevel(),
                    d->GetDefaultMetaInfoStride(r->GetVariableName(), r->GetRefinementLevel()),
                    range);
    *min = range[0];
    *max = range[1];
}

void TFEditor::_rangeChanged(float left, float right) {
    _rParams->GetMapperFunc(_rParams->GetVariableName())->setMinMaxMapValue(left, right);
}

TFMapsGroup::TFMapsGroup() {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);

    TFMapWidget *o;
    add(o = new TFOpacityWidget);
    o->AddMap(new TFColorMap(o));
    add(new TFHistogramWidget);
    add(new TFColorWidget);
}

void TFMapsGroup::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                         VAPoR::RenderParams *rParams) {
    for (auto map : _maps)
        map->Update(dataMgr, paramsMgr, rParams);
}

TFMapsInfoGroup *TFMapsGroup::CreateInfoGroup() {
    TFMapsInfoGroup *infos = new TFMapsInfoGroup;

    for (auto map : _maps) {
        infos->add(map);
    }

    return infos;
}

void TFMapsGroup::add(TFMapWidget *map) {
    _maps.push_back(map);
    layout()->addWidget(map);
    connect(map, SIGNAL(Activated(TFMapWidget *)), this, SLOT(mapActivated(TFMapWidget *)));
}

void TFMapsGroup::mapActivated(TFMapWidget *activatedMap) {
    for (auto map : _maps)
        if (map != activatedMap)
            map->Deactivate();
}

TFMapsInfoGroup::TFMapsInfoGroup() {}

void TFMapsInfoGroup::Update(VAPoR::RenderParams *rParams) {
    for (auto info : _infos)
        info->Update(rParams);
}

void TFMapsInfoGroup::add(TFMapWidget *map) {
    TFInfoWidget *info = map->GetInfoWidget();
    _infos.push_back(info);
    addWidget(info);
    connect(map, SIGNAL(Activated(TFMapWidget *)), this, SLOT(mapActivated(TFMapWidget *)));
}

void TFMapsInfoGroup::mapActivated(TFMapWidget *activatedMap) {
    setCurrentWidget(activatedMap->GetInfoWidget());
}
