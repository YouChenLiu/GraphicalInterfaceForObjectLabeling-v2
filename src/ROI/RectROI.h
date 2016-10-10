#ifndef RECTROI_H
#define RECTROI_H

#include "ROIBase.h"

class RectROI : public ROIBase
{
public:
  explicit RectROI(unsigned int sn, int x, int y, int width, int height, Shapes shape = Shapes::RECTANGLE);

  virtual const QString getShapeString(void) const
  {
    return QString("rectangle");
  }

  virtual bool checkAmbit(int x, int y) const final;

  virtual bool hitModifyingPos (int x, int y) final;

  virtual void draw(cv::Mat& image) const final;
};

#endif // RECTROI_H
