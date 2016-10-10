#include "RectROI.h"

#include <opencv2/imgproc.hpp>

RectROI::RectROI(unsigned int sn, int x, int y, int width, int height, Shapes shape) : ROIBase(sn, x, y, width, height, shape)
{
  clearState();
}

bool RectROI::checkAmbit(int x, int y) const
{
  cv::Rect2i rect(getStart(), getSize());
  return rect.contains(cv::Point2i(x, y));
}

bool RectROI::hitModifyingPos(int x, int y)
{
  return false;
}

void RectROI::draw(cv::Mat& image) const
{
  ROIBase::draw(image);
  cv::Rect2i rect(getStart(), getSize());
  cv::rectangle(image, rect, cv::Scalar::all(255));
}
