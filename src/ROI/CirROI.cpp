#include "CirROI.h"

#include <QPainter>

CirROI::CirROI(unsigned int sn, int x, int y, int radius) : EllipROI(sn, x, y, radius, radius, Shapes::CIRCLE)
{
  clearState();
}

bool CirROI::contains(const QPoint& pt) const
{
  bool result = false;

  auto diff = center() - pt;
  auto diffX = diff.x();
  auto diffY = diff.y();
  auto distance = static_cast<int>(sqrt(diffX * diffX + diffY * diffY));

  result = distance <= radius() ? true : false;
  return result;
}
