#include "EllipROI.h"

#include <opencv2/imgproc.hpp>

EllipROI::EllipROI(unsigned int sn, int x, int y, int major, int minor, Shapes shape) : ROIBase(sn, x, y, major, minor, shape)
{

}

void EllipROI::draw(cv::Mat &image) const
{
  ROIBase::draw(image);
  cv::ellipse(image, getStart(), getSize(), 0, 0, 360, cv::Scalar::all(255));
}

bool EllipROI::checkAmbit(int x, int y) const
{
  bool bInside = false;
  auto center = getStart();
  double dDiffX = qAbs(center.x - x);
  double dDiffY = qAbs(center.y - y);
  double dMajor = getWidth();
  double dMinor = getHeight();
  double dValue = dDiffX * dDiffX / dMajor * dMajor +
                  dDiffY * dDiffY / dMinor * dMinor;
  if (dValue <= 1) {
    bInside = true;
  }
  return bInside;
}

bool EllipROI::hitModifyingPos(int x, int y)
{
  return false;
}
