#include "viewer.h"

#include <QMouseEvent>
#include <QPainter>
#include <QCursor>
#include <QSet>

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

void Viewer::mouseMoveEvent(QMouseEvent* e)
{
  m_MouseState.ptCurrent = e->pos();

  for (auto& m : m_listMROI) {
    if (m.modifyingRect.contains(m_MouseState.ptCurrent)) {
      this->setCursor(Qt::SizeFDiagCursor);
      break;
    }
    if (m.rect.contains(m_MouseState.ptCurrent)) {
      this->setCursor(Qt::SizeAllCursor);
      break;
    }

    this->setCursor(Qt::ArrowCursor);
  }

  if (!m_MouseState.bLeftPressed) {
    return;
  }

  for (auto& m : m_listMROI) {
    if (m.bResizing == true) {
        m.resize(m_MouseState.ptCurrent);
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

  QSet<int> removedIdxes;
  for (int i = 0; i < m_listMROI.count(); ++i) {
    auto& m = m_listMROI[i];
    if (m.modifyingRect.contains(m_MouseState.ptCurrent)) {
      m.bResizing = true;
      break;
    }
    if (m.rect.contains(m_MouseState.ptCurrent)) {
      m.bMoving = true;
      break;
    }
    removedIdxes.insert(i);
  }

  for (const auto idx : removedIdxes) {
    m_listMROI.removeAt(idx);
  }
}

void Viewer::mouseReleaseEvent(QMouseEvent* e)
{
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
  }
  if (m_bDrawPreview) {
    QPoint ptLT = getLeftTop(m_MouseState.ptCurrent, m_MouseState.ptLeftPressed);
    QPoint ptRB = getRightBottom(m_MouseState.ptCurrent, m_MouseState.ptLeftPressed);
    emit newROI(ptLT, ptRB);
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
  }


  if (m_bDrawPreview && m_listMROI.isEmpty() && m_MouseState.ptLeftPressed != QPoint(-1, -1)) {
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::SolidLine);
    QPoint ptLT = getLeftTop(m_MouseState.ptCurrent, m_MouseState.ptLeftPressed);
    QPoint ptRB = getRightBottom(m_MouseState.ptCurrent, m_MouseState.ptLeftPressed);
    QPoint ptCenter = QPoint((ptLT.x() + ptRB.x() / 2),
                             (ptLT.y() + ptRB.y() / 2));
    int iWidth = ptRB.x() - ptLT.x();
    int iHeight = ptRB.y() - ptLT.y();
    int iRadius = 0;

    switch (m_Shape) {
      case Shapes::RECTANGLE:
      painter.drawRect(ptLT.x(), ptLT.y(), iWidth, iHeight);
      break;
    case Shapes::ELLIPSE:
      painter.drawEllipse(ptCenter, iWidth / 2, iHeight / 2);
      break;
    case Shapes::CIRCLE:
      iRadius = static_cast<int>(sqrt(iWidth * iWidth + iHeight * iHeight) / 2);
      painter.drawEllipse(ptCenter, iRadius, iRadius);
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
