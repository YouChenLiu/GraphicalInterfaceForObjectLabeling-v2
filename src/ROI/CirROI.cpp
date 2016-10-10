#include "CirROI.h"

#include <opencv2/imgproc.hpp>

CirROI::CirROI(unsigned int sn, int x, int y, int radius) : EllipROI(sn, x, y, radius, radius, Shapes::CIRCLE)
{

}

void CirROI::draw(cv::Mat &image) const
{
  ROIBase::draw(image);
  cv::circle(image, getStart(), getRadius(), cv::Scalar::all(255));
}

bool CirROI::checkAmbit(int x, int y) const
{
  bool bInside = false;
  auto center = getStart();
  auto radius = getRadius();
  int iDiffX = x - center.x;
  int iDiffY = y - center.y;
  int iValue = iDiffX * iDiffX + iDiffY * iDiffY;
  if (iValue <= radius * radius) {
    bInside = true;
  }
  return bInside;
}

bool CirROI::hitModifyingPos(int x, int y)
{
  return false;
}
