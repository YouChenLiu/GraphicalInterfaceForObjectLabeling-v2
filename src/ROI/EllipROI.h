#ifndef ELLIPROI_H
#define ELLIPROI_H

#include "ROIBase.h"

class EllipROI : public ROIBase
{
public:
  EllipROI(unsigned int sn, int x, int y, int major, int minor, Shapes shape = Shapes::ELLIPSE);

public:
  virtual const QString getShapeString(void) const
  {
    return QString("ellipse");
  }

  virtual bool checkAmbit(int x, int y) const;

  virtual bool hitModifyingPos (int x, int y);

  virtual void draw(cv::Mat& image) const;
};

#endif // ELLIPROI_H
