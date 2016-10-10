#ifndef ROIBASE_H
#define ROIBASE_H

#include "common.h"

class QRect;

class ROIBase
{
protected:
  static const unsigned int GLOBAL_STATE_POS = 24;
  static const unsigned int PRIVATE_STATE_POS = 0;
  static const unsigned int SELECTED = 0x80 << GLOBAL_STATE_POS;
  static const unsigned int INVISABLE = 0x40 << GLOBAL_STATE_POS;
private:
  cv::Point2i m_ptStart;
  cv::Size2i m_Size;
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
  explicit ROIBase(unsigned int sn, int x, int y, int width, int height, Shapes shape = Shapes::UNDEFINE);

  auto getSN(void) const
  {
    return m_iSN;
  }

  auto getStart(void) const
  {
    return m_ptStart;
  }

  auto getSize(void) const
  {
    return m_Size;
  }

  auto getWidth(void) const
  {
    return getSize().width;
  }

  auto getHeight(void) const
  {
    return getSize().height;
  }

  auto getShape(void) const
  {
    return m_Shape;
  }

  virtual const QString getShapeString(void) const {
    return QString("unknow");
  }

  virtual bool checkAmbit(int x, int y) const = 0;

  virtual bool hitModifyingPos (int x, int y) = 0;

  virtual void draw(cv::Mat& image) const;

  void clearState(void)
  {
    m_iState = 0;
  }

  void setSelected(bool value);

  bool getSelected(void) const
  {
    return (m_iState & SELECTED) ? true : false;
  }

  void setVisable(bool value);

  auto getVisable(void) const
  {
    return (m_iState & INVISABLE) ? false : true;
  }

  virtual void setStart(int x, int y)
  {
    m_ptStart = cv::Point2i(x, y);
  }

  virtual void setSize(int width, int height)
  {
    m_Size = cv::Size2i(width, height);
  }

  virtual void setRegion(const QRect& rect);

  virtual QRect getQRect(void) const;

protected:
  virtual void move(int x, int y);

  virtual void adjustWidth(int value) {
    m_Size.width += value;
  }

  virtual void adjustHeight(int value) {
    m_Size.height += value;
  }


};

#endif // ROIBASE_H
