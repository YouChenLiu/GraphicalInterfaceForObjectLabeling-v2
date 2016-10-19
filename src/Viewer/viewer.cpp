#include "viewer.h"

#include <QMouseEvent>
#include <QPainter>

Viewer::Viewer(QWidget* parent, Qt::WindowFlags flag) : QLabel(parent, flag)
{
  init();
  m_ptPicAnchor = QPoint(0, 0);
}

Viewer::Viewer(const QString& text, QWidget* parent, Qt::WindowFlags flag) : QLabel(text, parent, flag)
{
  init();
  m_ptPicAnchor = QPoint(0, 0);
}

Viewer::~Viewer()
{

}

void Viewer::addModifyingROI(unsigned int sn, const QRect& rect, Shapes shape)
{
  auto newMROI = ModifyingROI(sn, rect, shape);
  for (const auto& mroi : m_listMROI) {
    if (mroi == newMROI) {
      return;
    }
  }
  m_listMROI.append(newMROI);
  m_bDrawPreview = false;
}

void Viewer::removeModifyingROI(unsigned int sn)
{
  for (const auto& roi : m_listMROI) {
    if (roi.SN == sn) {
      m_listMROI.removeOne(roi);
   }
 }

 if (m_listMROI.count() == 0) {
   m_bDrawPreview = true;
 }
}

void Viewer::clearModifyingROI()
{
  m_listMROI.clear();
  m_bDrawPreview = false;
}

void Viewer::mouseMoveEvent(QMouseEvent* e)
{
  m_MouseState.ptCurrent = e->pos();
  this->setCursor(Qt::ArrowCursor);

  for (auto& m : m_listMROI) {
    if (m.modifyingRect.contains(m_MouseState.ptCurrent)) {
      this->setCursor(Qt::SizeFDiagCursor);
      break;
    }
    if (m.rect.contains(m_MouseState.ptCurrent)) {
      this->setCursor(Qt::SizeAllCursor);
      break;
    }
  }

  if (!m_MouseState.bLeftPressed) {
    return;
  }

  for (auto& m : m_listMROI) {
    if (m.bResizing == true) {
      if (m.shape == Shapes::CIRCLE) {
        QPoint diff = m_MouseState.ptCurrent - m.rect.topLeft();
        int diameter = diff.x() < diff.y() ? diff.x() : diff.y();
        QPoint ptRB = QPoint(m.rect.x() + diameter, m.rect.y() + diameter);
        m.resize(ptRB);
      } else {
        m.resize(m_MouseState.ptCurrent);
      }
      break;
    }
    if (m.bMoving) {
      m.move(m_MouseState.ptCurrent - m_MouseState.ptLeftPressed);
      break;
    }
  }

  repaint();
}

void Viewer::mousePressEvent(QMouseEvent* e)
{
  m_MouseState.ptCurrent = e->pos();
  switch (e->button()) {
  case Qt::LeftButton:
    m_MouseState.ptLeftPressed = e->pos();
    m_MouseState.bLeftPressed = true;
    break;
  case Qt::RightButton:
    m_MouseState.ptRightPressed = e->pos();
    m_MouseState.bRightPressed = true;
  default:
    break;
  }

  for (auto& m : m_listMROI) {
    if (m.modifyingRect.contains(m_MouseState.ptCurrent)) {
      m.bResizing = true;
      break;
    }
    if (m.rect.contains(m_MouseState.ptCurrent)) {
      m.bMoving = true;
      break;
    }
    m_listMROI.removeOne(m);
  }
}

void Viewer::mouseReleaseEvent(QMouseEvent* e)
{
  m_MouseState.ptCurrent = e->pos();
  switch (e->button()) {
  case Qt::LeftButton:
    m_MouseState.ptLeftRelease = e->pos();
    m_MouseState.bLeftPressed = false;
    break;
  case Qt::RightButton:
    m_MouseState.ptRightRelease = e->pos();
    m_MouseState.bRightPressed = false;
    break;
  default:
    break;
  }

  for (auto& m : m_listMROI) {
    if (m.bMoving) {
      m.moveTo(m_MouseState.ptCurrent - m_MouseState.ptLeftPressed);
      emit adjustROI(m.SN, m.rect);
      m.bMoving = false;
    }
    if (m.bResizing) {
      emit adjustROI(m.SN, m.rect);
      m.bResizing = false;
    }
  }

  if (m_MouseState.ptCurrent == m_MouseState.ptLeftPressed) {
    emit clicked(m_MouseState.ptCurrent.x(), m_MouseState.ptCurrent.y());
  } else {
    if (!m_bDrawPreview) {
      return;
    }
    QPoint ptLT = getLeftTop(m_MouseState.ptCurrent, m_MouseState.ptLeftPressed);
    QPoint ptRB = getRightBottom(m_MouseState.ptCurrent, m_MouseState.ptLeftPressed);
    emit newROI(QRect(ptLT, ptRB));
  }

  init();
}

void Viewer::paintEvent(QPaintEvent* e)
{
  QLabel::paintEvent(e);

  QPainter painter(this);

  for (auto& m : m_listMROI) {
    painter.setPen(Qt::DashLine);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(m.rect);

    painter.setPen(Qt::SolidLine);
    painter.setBrush(Qt::SolidPattern);
    painter.drawRect(m.modifyingRect);

    if (m.shape != Shapes::RECTANGLE) {
      painter.setBrush(Qt::NoBrush);
      painter.drawEllipse(m.rect);
    }
  }


  if (m_bDrawPreview && m_listMROI.isEmpty() && m_MouseState.ptLeftPressed != m_MouseState.ptLeftRelease) {
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::SolidLine);
    QPoint ptLT = getLeftTop(m_MouseState.ptCurrent, m_MouseState.ptLeftPressed);
    QPoint ptRB = getRightBottom(m_MouseState.ptCurrent, m_MouseState.ptLeftPressed);
    QRect rect = QRect(ptLT, ptRB);
    int iDiameter = 0;

    switch (m_Shape) {
      case Shapes::RECTANGLE:
      painter.drawRect(rect);
      break;
    case Shapes::ELLIPSE:
      painter.drawEllipse(rect);
      break;
    case Shapes::CIRCLE:
      iDiameter = rect.width() < rect.height() ? rect.width() : rect.height();
      rect.setWidth(iDiameter);
      rect.setHeight(iDiameter);
      painter.drawEllipse(rect);
      break;
    default:
      break;
    }
  }
}

void Viewer::init(void)
{
  m_MouseState.ptCurrent = QPoint(-1, -1);
  m_MouseState.ptLeftPressed = m_MouseState.ptRightPressed = QPoint(-1, -1);
  m_MouseState.ptLeftRelease = m_MouseState.ptRightRelease = QPoint(-1, -1);
  m_MouseState.bLeftPressed = m_MouseState.bRightPressed = false;
}

QPoint Viewer::getLeftTop(QPoint& pt1, QPoint& pt2) const
{
  return QPoint(pt1.x() > pt2.x() ? pt2.x() : pt1.x(),
                pt1.y() > pt2.y() ? pt2.y() : pt1.y());
}

QPoint Viewer::getRightBottom(QPoint& pt1, QPoint& pt2) const
{
  return QPoint(pt1.x() < pt2.x() ? pt2.x() : pt1.x(),
                pt1.y() < pt2.y() ? pt2.y() : pt1.y());
}

void Viewer::setPixmap(const QPixmap& newPixmap)
{
  QLabel::setPixmap(newPixmap);
}
