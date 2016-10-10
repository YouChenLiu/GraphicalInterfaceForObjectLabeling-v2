#include "ROIBase.h"
#include <QPoint>
#include <QRect>

ROIBase::ROIBase(unsigned int sn, int x, int y, int width, int height, Shapes shape)
{
  m_iSN = sn;
  m_ptStart = cv::Point2i(x, y);
  m_Size = cv::Size2i(width, height);
  m_Shape = shape;
}

void ROIBase::draw(cv::Mat& image) const
{
  if (getVisable() == false) {
    return;
  }
}

void ROIBase::move(int x, int y)
{
  m_ptStart.x += x;
  m_ptStart.y += y;
}

void ROIBase::setSelected(bool value)
{
  if (value) {
    m_iState |= SELECTED;
    setVisable(false);
  } else {
    m_iState &= !SELECTED;
    setVisable(true);
  }
}

void ROIBase::setVisable(bool value)
{
  if (value) {
    m_iState &= !INVISABLE;
  } else {
    m_iState |= INVISABLE;
  }
}

void ROIBase::setRegion(const QRect& rect)
{
  setStart(rect.topLeft().x(), rect.topLeft().y());
  setSize(rect.width(), rect.height());
}

QRect ROIBase::getQRect() const
{
  QPoint ptLT = QPoint(getStart().x, getStart().y);
  QSize size = QSize(getWidth(), getHeight());
  return QRect(ptLT, size);
}
