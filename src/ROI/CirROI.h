#ifndef CIRROI_H
#define CIRROI_H

#include "EllipROI.h"

class CirROI final : public EllipROI
{
public:
  CirROI(unsigned int sn, int x, int y, int radius);

public:
  auto getRadius(void) const
  {
    return getSize().width;
  }

  virtual const QString getShapeString(void) const final
  {
    return QString("circle");
  }

  virtual bool checkAmbit(int x, int y) const final;

  virtual bool hitModifyingPos (int x, int y) final;

  virtual void draw(cv::Mat& image) const final;
};

#endif // CIRROI_H
