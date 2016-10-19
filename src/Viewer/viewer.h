#ifndef VIEWER_H
#define VIEWER_H

#include <QLabel>
#include <QList>

#include "common.h"

class Viewer : public QLabel
{
  Q_OBJECT

private:
  struct ModifyingROI
  {
    unsigned int SN;
    QRect rect;
    QRect modifyingRect;
    Shapes shape;
    bool bMoving = false;
    bool bResizing = false;

  private:
    QPoint originalCenter;

  public:
    ModifyingROI(void)
    {
      SN = -1;
      rect = modifyingRect = QRect();
      shape = Shapes::UNDEFINE;
    }

    ModifyingROI(unsigned int sn, const QRect& r, Shapes s)
    {
      SN = sn;
      rect = r;
      shape = s;
      QPoint ptModify = r.bottomRight() - QPoint(5, 5);
      modifyingRect = QRect(ptModify,QSize(10, 10));
      originalCenter = r.center();
    }

    void move(const QPoint& distance)
    {
      rect.moveCenter(originalCenter + distance);
      modifyingRect.moveCenter(rect.bottomRight());
    }

    void moveTo(const QPoint& distance)
    {
      originalCenter += distance;
    }

    void resize(const QPoint& ptBottomRight)
    {
      rect.setBottomRight(ptBottomRight);
      originalCenter = rect.center();
      modifyingRect.moveCenter(rect.bottomRight());
    }

    bool operator==(const ModifyingROI& rhs) const
    {
      return this->SN == rhs.SN;
    }
  };

  struct MouseState
  {
    bool bLeftPressed;
    bool bRightPressed;
    QPoint ptLeftPressed;
    QPoint ptRightPressed;
    QPoint ptCurrent;
    QPoint ptLeftRelease;
    QPoint ptRightRelease;
  } m_MouseState;

private:
  bool m_bDrawPreview;
  Shapes m_Shape;
  QList<ModifyingROI> m_listMROI;
  QPoint m_ptPicAnchor;

public:
  Viewer(QWidget* parent = NULL, Qt::WindowFlags flag = 0);
  Viewer(const QString& text, QWidget* parent = NULL, Qt::WindowFlags flag = 0);

  ~Viewer();

public:
  void setShape(Shapes shape)
  {
    m_Shape = shape;
  }

  void setDrawPreview(bool value)
  {
    m_bDrawPreview = value;
  }

  void addModifyingROI(unsigned int sn, const QRect& rect, Shapes shape);

  void removeModifyingROI(unsigned int sn);

  void clearModifyingROI(void);

protected:
  void mouseMoveEvent(QMouseEvent* e);
  void mousePressEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void paintEvent(QPaintEvent* e);

private:
  void init(void);
  QPoint getLeftTop(QPoint& pt1, QPoint& pt2) const;
  QPoint getRightBottom(QPoint& pt1, QPoint& pt2) const;

signals:

  void newROI(const QRect& rect);

  void clicked(int x, int y);

  void adjustROI(unsigned int sn, const QRect& rect);

public slots:
  void setPixmap(const QPixmap& newPixmap);
};

#endif // VIEWER_H
