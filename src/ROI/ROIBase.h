#ifndef ROIBASE_H
#define ROIBASE_H

#include "common.h"

#include <QRect>
class QPoint;
class QPainter;

class ROIBase : protected QRect
{
protected:
  static const unsigned int GLOBAL_STATE_POS = 24;
  static const unsigned int PRIVATE_STATE_POS = 0;
  static const unsigned int SELECTED = 0x80 << GLOBAL_STATE_POS;
  static const unsigned int INVISABLE = 0x40 << GLOBAL_STATE_POS;
private:
  Shapes m_Shape;
  unsigned int m_iSN;

  /**
   * @brief state of the ROI
   *
   * <pre>
   * -----------------------------------------------------
   * |   Global   |  Reserve  |   Reserve   |   Private  |
   * -----------------------------------------------------
   * 31          24           16            8            0
   * </pre>
   */
  unsigned int m_iState;

public:
  explicit ROIBase(unsigned int sn, const QPoint& ptTopLeft, const QPoint& ptBottomRight, Shapes shape = Shapes::UNDEFINE);
  explicit ROIBase(unsigned int sn, const QPoint& ptTopLeft, const QSize& Size, Shapes shape = Shapes::UNDEFINE);
  explicit ROIBase(unsigned int sn, int x, int y, int width, int height, Shapes shape = Shapes::UNDEFINE);

  auto sn(void) const
  {
    return m_iSN;
  }

  auto x(void) const
  {
    return QRect::x();
  }

  auto y(void) const
  {
    return QRect::y();
  }

  auto topLeft(void) const
  {
    return QRect::topLeft();
  }

  void setTopLeft(const QPoint& pt)
  {
    QRect::setTopLeft(pt);
  }

  auto bottomRight(void) const
  {
    return QRect::bottomRight();
  }

  void setBottomRight(const QPoint& pt)
  {
    QRect::setBottomRight(pt);
  }

  auto center(void) const
  {
    return QRect::center();
  }

  auto size(void) const
  {
    return QRect::size();
  }

  void setSize(const QSize& size)
  {
    QRect::setSize(size);
  }

  auto width(void) const
  {
    return QRect::width();
  }

  void setWidth(int width)
  {
    QRect::setWidth(width);
  }

  auto height(void) const
  {
    return QRect::height();
  }

  void setHeight(int height)
  {
    QRect::setHeight(height);
  }

  auto shape(void) const
  {
    return m_Shape;
  }

  virtual const QString getShapeString(void) const {
    return QString("unknow");
  }

  virtual bool contains(const QPoint& pt) const
  {
    return QRect::contains(pt, false);
  }

  bool contains(int x, int y) const
  {
    return contains(QPoint(x, y));
  }

  virtual void draw(QPainter& painter) const = 0;

  void clearState(void)
  {
    m_iState = 0;
  }

  void setSelected(bool value);

  bool selected(void) const
  {
    return (m_iState & SELECTED) ? true : false;
  }

  void setVisable(bool value);

  auto visable(void) const
  {
    return (m_iState & INVISABLE) ? false : true;
  }

  auto rect(void) const
  {
    return QRect(topLeft(), bottomRight());
  }

  virtual void setRect(const QRect& rect)
  {
    QRect::setRect(rect.x(), rect.y(), rect.width(), rect.height());
  }

  bool operator==(const ROIBase& rhs)
  {
    return this->sn() == rhs.sn();
  }

protected:

};

#endif // ROIBASE_H
