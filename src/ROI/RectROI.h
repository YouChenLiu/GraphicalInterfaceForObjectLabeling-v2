#ifndef RECTROI_H
#define RECTROI_H

#include "ROIBase.h"

class RectROI : public ROIBase
{
public:
  explicit RectROI(unsigned int sn, const QPoint& ptTopLeft, const QPoint& ptBottomRight, Shapes shape = Shapes::RECTANGLE);
  explicit RectROI(unsigned int sn, const QPoint& ptTopLeft, const QSize& Size, Shapes shape = Shapes::RECTANGLE);
  explicit RectROI(unsigned int sn, int x, int y, int width, int height, Shapes shape = Shapes::RECTANGLE);

  virtual QString getShapeString(void) const
  {
    return QString("rectangle");
  }

  virtual void draw(QPainter& painter, double scale = 1.0) const final;
};

#endif // RECTROI_H
