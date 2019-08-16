#pragma once

#include <QFrame>
#include <QWidget>
#include <glm/glm.hpp>
#include <vapor/ParamsMgr.h>
#include <vapor/RenderParams.h>
#include <vapor/VAssert.h>

class TFHistogramWidget : public QFrame {
    Q_OBJECT

  public:
    TFHistogramWidget();

    void Update(VAPoR::DataMgr *dataMgr, VAPoR::ParamsMgr *paramsMgr, VAPoR::RenderParams *rParams);

    QSize minimumSizeHint() const;

  protected:
    void paintEvent(QPaintEvent *event);
    //    void mousePressEvent(QMouseEvent *event);
    //    void mouseReleaseEvent(QMouseEvent *event);
    //    void mouseMoveEvent(QMouseEvent *event);
    //    void mouseDoubleClickEvent(QMouseEvent *event);

  private:
    VAPoR::DataMgr *_dataMgr = nullptr;
    VAPoR::RenderParams *_renderParams = nullptr;

    glm::vec2 NDCToPixel(const glm::vec2 &v) const;
    QPointF QNDCToPixel(const glm::vec2 &v) const;
    glm::vec2 PixelToNDC(const QPointF &p) const;
    glm::vec2 PixelToNDC(const glm::vec2 &p) const;
};
