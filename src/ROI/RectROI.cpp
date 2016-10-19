#include "RectROI.h"
#include <QPainter>

RectROI::RectROI(unsigned int sn, const QPoint& ptTopLeft, const QPoint& ptBottomRight, Shapes shape) : ROIBase(sn, ptTopLeft, ptBottomRight, shape)
{
  clearState();
}

RectROI::RectROI(unsigned int sn, const QPoint& ptTopLeft, const QSize& Size, Shapes shape) : ROIBase(sn, ptTopLeft, Size, shape)
{
  clearState();
}

RectROI::RectROI(unsigned int sn, int x, int y, int width, int height, Shapes shape) : ROIBase(sn, x, y, width, height, shape)
{
  clearState();
}

void RectROI::draw(QPainter& painter, double scale) const
{
  ROIBase::draw(painter, scale);

  painter.drawRect(rect());
}
