#include "TFIsoValueWidget.h"
#include "QPaintUtils.h"
#include "TFIsoValueInfoWidget.h"
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

TFIsoValueMap::TFIsoValueMap(TFMapWidget *parent) : TFMap(parent) {}

void TFIsoValueMap::Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr,
                           VAPoR::RenderParams *rp) {
    if (!rp->HasIsoValues()) {
        hide();
        return;
    }

    if (rp != _renderParams)
        DeselectControlPoint();

    _renderParams = rp;
    _paramsMgr = paramsMgr;
    loadFromParams(rp);
    update();

    if (_selectedId > -1)
        UpdateInfo(_isoValues[_selectedId]);
}

#define PROPERTY_INDEX ("index")
#define PROPERTY_VALUE ("value")

void TFIsoValueMap::PopulateContextMenu(QMenu *menu, const glm::vec2 &p) {
    int selectedId = findSelectedControlPoint(p);
    if (selectedId != -1)
        menu->addAction("Delete control point", this, SLOT(menuDeleteControlPoint()))
            ->setProperty(PROPERTY_INDEX, selectedId);
    else {
        QAction *action = menu->addAction("Add control point", this, SLOT(menuAddControlPoint()));
        action->setProperty(PROPERTY_VALUE, QVariant(valueForControlX(p.x)));
        if (_isoValues.size() >= 4)
            action->setDisabled(true);
    }
}

QSize TFIsoValueMap::minimumSizeHint() const { return GetControlPointArea(QPoint(0, 0)).size(); }

void TFIsoValueMap::Deactivate() { DeselectControlPoint(); }

TFInfoWidget *TFIsoValueMap::createInfoWidget() {
    TFIsoValueInfoWidget *info = new TFIsoValueInfoWidget;

    connect(this, SIGNAL(ControlPointDeselected()), info, SLOT(Deselect()));
    connect(this, SIGNAL(UpdateInfo(float)), info, SLOT(SetControlPoint(float)));
    connect(info, SIGNAL(ControlPointChanged(float)), this, SLOT(UpdateFromInfo(float)));

    return info;
}

void TFIsoValueMap::paintEvent(QPainter &p) {
    //     243 245 249
    p.fillRect(rect(), Qt::lightGray);

    if (_renderParams) {
        for (int i = 0; i < _isoValues.size(); i++) {
            drawControl(p, controlQPositionForValue(_isoValues[i]), i == _selectedId);
        }

        if (_isoValues.size() == 0) {
            QFont font = getFont();
            font.setPixelSize(rect().height());
            p.setFont(font);
            p.drawText(rect(), Qt::AlignCenter, "doubleclick to add isovalues");
        }
    }
}

void TFIsoValueMap::drawControl(QPainter &p, const QPointF &pos, bool selected) const {
    float r = GetControlPointRadius();
    float t = GetControlPointTriangleHeight();
    float s = GetControlPointSquareHeight();

    QPen pen(Qt::darkGray, 0.5);
    QBrush brush(QColor(0xfa, 0xfa, 0xfa));
    p.setBrush(brush);
    p.setPen(pen);

    //    p.drawEllipse(pos, radius, radius);

    QPolygonF graph;
    graph.push_back(pos + QPointF(0, 0));
    graph.push_back(pos + QPointF(-r, t));
    graph.push_back(pos + QPointF(-r, t + s));
    graph.push_back(pos + QPointF(r, t + s));
    graph.push_back(pos + QPointF(r, t));

    p.drawPolygon(graph);

    if (selected) {
        if (selected) {
            p.setPen(Qt::NoPen);
            p.setBrush(QBrush(Qt::black));
            r *= 0.38;
            p.drawEllipse(pos + QPointF(0, t + (s / 3)), r, r);
        }
    }
}

float TFIsoValueMap::GetControlPointTriangleHeight() const {
    return GetControlPointRadius() * 2 * 0.618;
}

float TFIsoValueMap::GetControlPointSquareHeight() const { return GetControlPointRadius() * 1.618; }

QRect TFIsoValueMap::GetControlPointArea(const QPoint &p) const {
    float h = GetControlPointSquareHeight() + GetControlPointTriangleHeight();
    float r = GetControlPointRadius();
    return QRect(p - QPoint(r, 0), p + QPoint(r, h));
}

void TFIsoValueMap::mousePressEvent(QMouseEvent *event) {
    emit Activated(this);
    vec2 mouse(event->pos().x(), event->pos().y());

    int selectedId = findSelectedControlPoint(mouse);
    if (selectedId >= 0) {
        float value = _isoValues[selectedId];
        _isDraggingControl = true;
        _draggingControlID = selectedId;
        selectControlPoint(selectedId);
        update();
        _dragOffset = controlPositionForValue(value) - mouse;
        _paramsMgr->BeginSaveStateGroup("IsoValue modification");
        return;
    }

    DeselectControlPoint();
    event->ignore();
    update();
}

void TFIsoValueMap::mouseReleaseEvent(QMouseEvent *event) {
    if (_isDraggingControl)
        _paramsMgr->EndSaveStateGroup();
    else
        event->ignore();
    _isDraggingControl = false;
}

void TFIsoValueMap::mouseMoveEvent(QMouseEvent *event) {
    vec2 mouse = qvec2(event->pos());

    if (_isDraggingControl) {
        float newVal = glm::clamp(valueForControlX(mouse.x + _dragOffset.x), 0.f, 1.f);

        moveControlPoint(&_draggingControlID, newVal);
        selectControlPoint(_draggingControlID);
        saveToParams(_renderParams);
        update();
        _paramsMgr->IntermediateChange();
    } else {
        event->ignore();
    }
}

void TFIsoValueMap::mouseDoubleClickEvent(QMouseEvent *event) {
    vec2 mouse = qvec2(event->pos());
    int selectedId = findSelectedControlPoint(mouse);
    if (selectedId >= 0) {
        deleteControlPoint(selectedId);
        return;
    }

    if (_isoValues.size() < 4) {
        float newVal = valueForControlX(mouse.x);
        if (newVal >= 0 && newVal <= 1) {
            int newId = addControlPoint(newVal);
        }
        return;
    }
}

QMargins TFIsoValueMap::GetPadding() const {
    QMargins m = TFMap::GetPadding();
    m.setTop(0);
    return m;
}

void TFIsoValueMap::saveToParams(VAPoR::RenderParams *rp) const {
    if (!rp)
        return;
    if (!rp->HasIsoValues())
        return;
    assert(rp->HasIsoValues());

    const float min = getDataRangeMin();
    const float max = getDataRangeMax();

    vector<double> values(_isoValues.size());
    for (int i = 0; i < values.size(); i++)
        values[i] = _isoValues[i] * (max - min) + min;
    rp->SetIsoValues(values);
}

void TFIsoValueMap::loadFromParams(VAPoR::RenderParams *rp) {
    if (!rp)
        return;
    if (!rp->HasIsoValues())
        return;
    assert(rp->HasIsoValues());

    const float min = getDataRangeMin();
    const float max = getDataRangeMax();

    vector<double> newValues = rp->GetIsoValues();
    if (newValues.size() != _isoValues.size()) {
        DeselectControlPoint();
        _isoValues.resize(newValues.size());
    }
    for (int i = 0; i < newValues.size(); i++)
        _isoValues[i] = (newValues[i] - min) / (max - min);
}

int TFIsoValueMap::addControlPoint(float value) {
    if (_isoValues.size() >= 4)
        return;

    int index = -1;
    for (int i = 0; i < _isoValues.size(); i++) {
        if (value < _isoValues[i]) {
            _isoValues.insert(_isoValues.begin() + i, value);
            index = i;
            break;
        }
    }
    if (index == -1) {
        _isoValues.push_back(value);
        index = _isoValues.size() - 1;
    }
    saveToParams(_renderParams);
    selectControlPoint(index);
    update();
    return index;
}

void TFIsoValueMap::deleteControlPoint(int i) {
    if (i == _selectedId)
        DeselectControlPoint();
    _isoValues.erase(_isoValues.begin() + i);
    update();
    saveToParams(_renderParams);
}

void TFIsoValueMap::moveControlPoint(int *index, float value) {
    if (_equidistantIsoValues) {
        if (*index == 0) {
            const float initialValue = _isoValues[0];
            const float diff = value - initialValue;
            for (float &v : _isoValues)
                v += diff;
        } else {
            const float baseValue = _isoValues[0];
            const float initialValue = _isoValues[*index];
            const float scale = (value - baseValue) / (initialValue - baseValue);
            for (float &v : _isoValues)
                v = baseValue + (v - baseValue) * scale;
        }
    } else {
        deleteControlPoint(*index);
        *index = addControlPoint(value);
    }
}

void TFIsoValueMap::selectControlPoint(int index) {
    _selectedId = index;
    float value = _isoValues[index];
    emit UpdateInfo(value);
}

void TFIsoValueMap::DeselectControlPoint() {
    _selectedId = -1;
    emit ControlPointDeselected();
    update();
}

void TFIsoValueMap::UpdateFromInfo(float value) {
    if (_selectedId >= 0 && _selectedId < _isoValues.size()) {
        moveControlPoint(&_selectedId, value);
        update();
        saveToParams(_renderParams);
    }
}

int TFIsoValueMap::findSelectedControlPoint(const glm::vec2 &mouse) const {
    for (int i = _isoValues.size() - 1; i >= 0; i--)
        if (controlPointContainsPixel(_isoValues[i], mouse))
            return i;
    return -1;
}

bool TFIsoValueMap::controlPointContainsPixel(float cp, const vec2 &p) const {
    QRect rect = GetControlPointArea(controlQPositionForValue(cp));
    return rect.contains(QPoint(p.x, p.y));
}

QPoint TFIsoValueMap::controlQPositionForValue(float value) const {
    const vec2 v = controlPositionForValue(value);
    return QPoint(v.x, v.y);
}

glm::vec2 TFIsoValueMap::controlPositionForValue(float value) const {
    return vec2(controlXForValue(value), 0);
}

float TFIsoValueMap::controlXForValue(float value) const { return NDCToPixel(vec2(value, 0.f)).x; }

float TFIsoValueMap::valueForControlX(float position) const {
    return PixelToNDC(vec2(position, 0.f)).x;
}

float TFIsoValueMap::getDataRangeMin() const {
    if (!_renderParams)
        return 0;
    return _renderParams->GetMapperFunc(_renderParams->GetVariableName())->getMinMapValue();
}

float TFIsoValueMap::getDataRangeMax() const {
    if (!_renderParams)
        return 1;
    return _renderParams->GetMapperFunc(_renderParams->GetVariableName())->getMaxMapValue();
}

void TFIsoValueMap::menuDeleteControlPoint() {
    emit Activated(this);
    QVariant valueVariant = sender()->property(PROPERTY_INDEX);
    if (valueVariant.isValid()) {
        int index = valueVariant.toInt();
        if (index >= 0 && index < _isoValues.size())
            deleteControlPoint(index);
    }
}

void TFIsoValueMap::menuAddControlPoint() {
    emit Activated(this);
    QVariant value = sender()->property(PROPERTY_VALUE);
    if (value.isValid())
        addControlPoint(value.toFloat());
}
