#ifndef CIRROI_H
#define CIRROI_H

#include "EllipROI.h"

class CirROI final : public EllipROI
{
public:
  CirROI(unsigned int sn, int x, int y, int radius);

public:
  auto radius(void) const
  {
    return width() / 2;
  }

  virtual const QString getShapeString(void) const final
  {
    return QString("circle");
  }

  virtual bool contains(const QPoint& pt) const;
};

#endif // CIRROI_H
