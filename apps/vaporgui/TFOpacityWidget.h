#pragma once

#include <QFrame>
#include <QWidget>
#include <glm/glm.hpp>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>
#include <vapor/VAssert.h>

class ControlPointList {
  public:
    class PointIterator {
        ControlPointList *list;
        int i;

        PointIterator(ControlPointList *list, int i) : list(list), i(i) {}

      public:
        PointIterator() {}
        PointIterator &operator++() {
            ++i;
            return *this;
        }
        PointIterator &operator--() {
            --i;
            return *this;
        }
        PointIterator operator+(int x) const { return PointIterator(list, i + x); }
        PointIterator operator-(int x) const { return PointIterator(list, i - x); }
        glm::vec2 &operator*() { return (*list)[i]; }
        bool operator!=(const PointIterator &other) { return !(*this == other); }
        bool operator==(const PointIterator &other) {
            return other.list == this->list && other.i == this->i;
        }

        bool IsFirst() const { return i == 0; }
        bool IsLast() const { return i == list->Size() - 1; }

        friend class ControlPointList;
    };

    class LineIterator : public PointIterator {
        using PointIterator::PointIterator;

      public:
        glm::vec2 &operator*() = delete;
        bool IsFirst() const = delete;
        bool IsLast() const = delete;

        const glm::vec2 a() {
            if (i == 0)
                return glm::vec2(0, (*list)[0].y);
            return (*list)[i - 1];
        }
        const glm::vec2 b() {
            if (i == list->SizeLines() - 1)
                return glm::vec2(1, (*list)[list->Size() - 1].y);
            return (*list)[i];
        }

        friend class ControlPointList;
    };

    std::vector<glm::vec2> _points;

  public:
    glm::vec2 &operator[](const int i) {
        VAssert(i >= 0 && i < _points.size());
        return _points[i];
    }

    void Add(const glm::vec2 &v) { _points.push_back(v); }

    void Add(const glm::vec2 &v, const int i) {
        VAssert(i >= 0 && i <= _points.size());
        _points.insert(_points.begin() + i, v);
    }

    void Add(const glm::vec2 &v, const LineIterator &line) {
        VAssert(line.i >= 0 && line.i <= _points.size());
        _points.insert(_points.begin() + line.i, v);
    }

    void Remove(const PointIterator &point) {
        VAssert(point.i >= 0 && point.i < _points.size());
        if (Size() > 2) // VAPoR::MapperFunc cannot handle less than 2
            _points.erase(_points.begin() + point.i);
    }

    int Size() const { return _points.size(); }
    int SizeLines() const { return Size() + 1; }
    void Resize(int n) { _points.resize(n); }

    PointIterator BeginPoints() { return PointIterator(this, 0); }
    PointIterator EndPoints() { return PointIterator(this, Size()); }
    LineIterator BeginLines() { return LineIterator(this, 0); }
    LineIterator EndLines() { return LineIterator(this, SizeLines()); }
};

class TFOpacityWidget : public QFrame {
    Q_OBJECT

  public:
    TFOpacityWidget();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

    QSize minimumSizeHint() const;

  protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

  private:
    VAPoR::RenderParams *_renderParams = nullptr;
    ControlPointList _controlPoints;
    bool _isDraggingControl = false;
    ControlPointList::PointIterator _draggedControl;
    glm::vec2 _dragOffset;
    glm::vec2 m;

    void opacityChanged();

    bool controlPointContainsPixel(const glm::vec2 &cp, const glm::vec2 &pixel) const;
    ControlPointList::PointIterator findSelectedControlPoint(const glm::vec2 &mouse);

    glm::vec2 NDCToPixel(const glm::vec2 &v) const;
    QPointF QNDCToPixel(const glm::vec2 &v) const;
    glm::vec2 PixelToNDC(const QPointF &p) const;
    glm::vec2 PixelToNDC(const glm::vec2 &p) const;
};
