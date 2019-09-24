#include "TFColorWidget.h"
#include "QPaintUtils.h"
#include "TFColorInfoWidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>

using namespace VAPoR;
using glm::vec2;
using std::vector;

static vec2 qvec2(const QPoint &qp) { return vec2(qp.x(), qp.y()); }
static vec2 qvec2(const QPointF &qp) { return vec2(qp.x(), qp.y()); }
static QPointF qvec2(const vec2 &v) { return QPointF(v.x, v.y); }

TFColorMap::TFColorMap(TFMapWidget *parent) : TFMap(parent) {}

void TFColorMap::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                        VAPoR::RenderParams *rp) {
    _renderParams = rp;
    _paramsMgr = paramsMgr;
    update();

    if (rp && _selectedId > -1)
        UpdateInfo(getColormap()->controlPointValueNormalized(_selectedId),
                   VColorToQColor(getColormap()->controlPointColor(_selectedId)));
}

QSize TFColorMap::minimumSizeHint() const { return QSize(100, 30); }

void TFColorMap::Deactivate() { DeselectControlPoint(); }

#define PROPERTY_INDEX ("index")
#define PROPERTY_VALUE ("value")

void TFColorMap::PopulateContextMenu(QMenu *menu, const glm::vec2 &p) {
    int selected = findSelectedControlPoint(p);

    if (selected != -1)
        menu->addAction("Delete control point", this, SLOT(menuDeleteSelectedControlPoint()))
            ->setProperty(PROPERTY_INDEX, selected);
    else
        menu->addAction("Add control point", this, SLOT(menuAddControlPoint()))
            ->setProperty(PROPERTY_VALUE, QVariant(valueForControlX(p.x)));
}

TFInfoWidget *TFColorMap::createInfoWidget() {
    TFColorInfoWidget *info = new TFColorInfoWidget;

    connect(this, SIGNAL(ControlPointDeselected()), info, SLOT(DeselectControlPoint()));
    connect(this, SIGNAL(UpdateInfo(float, QColor)), info, SLOT(SetControlPoint(float, QColor)));
    connect(info, SIGNAL(ControlPointChanged(float, QColor)), this,
            SLOT(UpdateFromInfo(float, QColor)));

    return info;
}

#define CONTROL_POINT_RADIUS (4.0f)
#define PADDING (CONTROL_POINT_RADIUS + 1.0f)

void TFColorMap::paintEvent(QPainter &p) {
    //     243 245 249
    p.fillRect(rect(), Qt::lightGray);
    QPaintUtils::BoxDropShadow(p, paddedRect(), 10, QColor(0, 0, 0, 120));

    if (_renderParams) {
        RenderParams *rp = _renderParams;

        ColorMap *cm = rp->GetMapperFunc(rp->GetVariableName())->GetColorMap();

        QMargins padding = GetPadding();
        int nSamples = width() - (padding.left() + padding.right());
        unsigned char buf[nSamples * 3];
        float rgb[3];
        for (int i = 0; i < nSamples; i++) {
            cm->colorNormalized(i / (float)nSamples).toRGB(rgb);
            buf[i * 3 + 0] = rgb[0] * 255;
            buf[i * 3 + 1] = rgb[1] * 255;
            buf[i * 3 + 2] = rgb[2] * 255;
        }
        QImage image(buf, nSamples, 1, QImage::Format::Format_RGB888);

        p.drawImage(paddedRect(), image);

        for (int i = 0; i < cm->numControlPoints(); i++) {
            drawControl(p, controlQPositionForValue(cm->controlPointValueNormalized(i)),
                        i == _selectedId);
        }
    }
}

void TFColorMap::mousePressEvent(QMouseEvent *event) {
    emit Activated(this);
    ColorMap *cm = getColormap();
    vec2 mouse(event->pos().x(), event->pos().y());

    for (int i = 0; i < cm->numControlPoints(); i++) {
        float value = cm->controlPointValueNormalized(i);
        if (controlPointContainsPixel(value, mouse)) {
            _isDraggingControl = true;
            _draggingControlID = i;
            selectControlPoint(i);
            update();
            _dragOffset = controlPositionForValue(value) - mouse;
            BeginSaveStateGroup(_paramsMgr, "Colormap modification");
            return;
        }
    }

    DeselectControlPoint();
    event->ignore();
    update();
}

void TFColorMap::mouseReleaseEvent(QMouseEvent *event) {
    if (_isDraggingControl)
        EndSaveStateGroup(_paramsMgr);
    else
        event->ignore();
    _isDraggingControl = false;
}

void TFColorMap::mouseMoveEvent(QMouseEvent *event) {
    vec2 mouse = qvec2(event->pos());

    if (_isDraggingControl) {
        float newVal = glm::clamp(valueForControlX(mouse.x + _dragOffset.x), 0.f, 1.f);

        moveControlPoint(&_draggingControlID, newVal);
        selectControlPoint(_draggingControlID);
        update();
        _paramsMgr->IntermediateChange();
    } else {
        event->ignore();
    }
}

void TFColorMap::mouseDoubleClickEvent(QMouseEvent *event) {
    vec2 mouse = qvec2(event->pos());
    int selectedId = findSelectedControlPoint(mouse);
    if (selectedId >= 0) {
        deleteControlPoint(selectedId);
        return;
    }

    float newVal = valueForControlX(mouse.x);
    if (newVal >= 0 && newVal <= 1)
        addControlPoint(newVal);
}

void TFColorMap::moveControlPoint(int *index, float value, const VAPoR::ColorMap::Color &c) {
    ColorMap *cm = getColormap();

    cm->deleteControlPoint(*index);
    *index = cm->addNormControlPoint(value, c);
}

void TFColorMap::moveControlPoint(int *index, float value) {
    ColorMap *cm = getColormap();
    ColorMap::Color c = cm->controlPointColor(_draggingControlID);
    moveControlPoint(index, value, c);
}

void TFColorMap::deleteControlPoint(int index) {
    if (index == _selectedId)
        DeselectControlPoint();
    else if (index < _selectedId)
        _selectedId--;
    getColormap()->deleteControlPoint(index);
    update();
}

void TFColorMap::addControlPoint(float value) {
    selectControlPoint(getColormap()->addNormControlPointAt(value));
    update();
}

ColorMap *TFColorMap::getColormap() const {
    return _renderParams->GetMapperFunc(_renderParams->GetVariableName())->GetColorMap();
}

void TFColorMap::selectControlPoint(int index) {
    _selectedId = index;
    ColorMap *cm = getColormap();

    float value = cm->controlPointValueNormalized(index);
    ColorMap::Color vColor = cm->controlPointColor(index);

    UpdateInfo(value, VColorToQColor(vColor));
}

void TFColorMap::DeselectControlPoint() {
    _selectedId = -1;
    emit ControlPointDeselected();
    update();
}

void TFColorMap::UpdateFromInfo(float value, QColor color) {
    moveControlPoint(&_selectedId, value, QColorToVColor(color));
}

void TFColorMap::menuDeleteSelectedControlPoint() {
    const ColorMap *cm = getColormap();
    QVariant valueVariant = sender()->property(PROPERTY_INDEX);
    if (valueVariant.isValid()) {
        int index = valueVariant.toInt();
        if (index >= 0 && index < cm->numControlPoints())
            deleteControlPoint(index);
    }
}

void TFColorMap::menuAddControlPoint() {
    QVariant value = sender()->property(PROPERTY_VALUE);
    if (value.isValid())
        addControlPoint(value.toFloat());
}

int TFColorMap::findSelectedControlPoint(const glm::vec2 &mouse) const {
    const ColorMap *cm = getColormap();
    const int n = cm->numControlPoints();
    for (int i = 0; i < n; i++)
        if (controlPointContainsPixel(cm->controlPointValueNormalized(i), mouse))
            return i;
    return -1;
}

bool TFColorMap::controlPointContainsPixel(float cp, const vec2 &pixel) const {
    return glm::distance(pixel, controlPositionForValue(cp)) <= GetControlPointRadius();
}

QPointF TFColorMap::controlQPositionForValue(float value) const {
    const vec2 v = controlPositionForValue(value);
    return QPointF(v.x, v.y);
}

glm::vec2 TFColorMap::controlPositionForValue(float value) const {
    return vec2(controlXForValue(value), height() / 2.f);
}

float TFColorMap::controlXForValue(float value) const { return NDCToPixel(vec2(value, 0.f)).x; }

float TFColorMap::valueForControlX(float position) const {
    return PixelToNDC(vec2(position, 0.f)).x;
}

QColor TFColorMap::VColorToQColor(const ColorMap::Color &c) {
    float rgb[3];
    c.toRGB(rgb);
    return QColor(rgb[0] * 255, rgb[1] * 255, rgb[2] * 255);
}

ColorMap::Color TFColorMap::QColorToVColor(const QColor &c) {
    double h, s, v;
    c.getHsvF(&h, &s, &v);
    return ColorMap::Color(h, s, v);
}
