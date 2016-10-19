#include "ROIBase.h"
#include <QPoint>
#include <QPainter>

ROIBase::ROIBase(unsigned int sn, const QPoint& ptTopLeft, const QPoint& ptBottomRight, Shapes shape) : QRect(ptTopLeft, ptBottomRight)
{
  m_iSN = sn;
  m_Shape = shape;
}

ROIBase::ROIBase(unsigned int sn, const QPoint& ptTopLeft, const QSize& Size, Shapes shape) : QRect(ptTopLeft, Size)
{
  m_iSN = sn;
  m_Shape = shape;
}

ROIBase::ROIBase(unsigned int sn, int x, int y, int width, int height, Shapes shape) : QRect(x, y, width, height)
{
  m_iSN = sn;
  m_Shape = shape;
}

void ROIBase::draw(QPainter& painter, double scale) const
{
  QPen pen;
  if (enabled()) {
    pen.setColor(Colors::normal);
    pen.setStyle(LineStyle::normal);
  } else {
    pen.setColor(Colors::disabled);
    pen.setStyle(LineStyle::disabled);
  }

  painter.setPen(pen);
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

void ROIBase::setEnable(bool value)
{
  if (value) {
    m_iState &= !DISABLE;
  } else {
    m_iState |= DISABLE;
  }
}
