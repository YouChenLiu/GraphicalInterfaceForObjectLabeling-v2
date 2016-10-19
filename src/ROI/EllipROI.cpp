#include "EllipROI.h"

#include <QPainter>

EllipROI::EllipROI(unsigned int sn, int x, int y, int width, int height, Shapes shape) : ROIBase(sn, x, y, width, height, shape)
{
  clearState();
}

bool EllipROI::contains(const QPoint& pt) const
{
  bool bInside = false;
  double dDiffX = qAbs(center().x() - pt.x());
  double dDiffY = qAbs(center().y() - pt.y());
  double dMajor = major();
  double dMinor = minor();
  double dValue = (dDiffX * dDiffX) / (dMajor * dMajor) +
                  (dDiffY * dDiffY) / (dMinor * dMinor);
  if (dValue <= 1) {
    bInside = true;
  }
  return bInside;
}

void EllipROI::draw(QPainter& painter, double scale) const
{
  ROIBase::draw(painter, scale);
  painter.drawEllipse(rect());
}
