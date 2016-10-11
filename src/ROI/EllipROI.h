#ifndef ELLIPROI_H
#define ELLIPROI_H

#include "ROIBase.h"

class EllipROI : public ROIBase
{
public:
  EllipROI(unsigned int sn, int x, int y, int width, int height, Shapes shape = Shapes::ELLIPSE);

public:
  auto major(void) const
  {
    return width() / 2;
  }

  auto minor(void) const
  {
    return height() / 2;
  }

  virtual const QString getShapeString(void) const
  {
    return QString("ellipse");
  }

  virtual bool contains(const QPoint& pt) const;

  virtual void draw(QPainter& painter) const;
};

#endif // ELLIPROI_H
