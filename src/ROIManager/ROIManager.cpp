#include "ROIManager.h"

#include <QRect>

#include "ROI/ROIBase.h"
#include "ROI/RectROI.h"
#include "ROI/EllipROI.h"
#include "ROI/CirROI.h"

ROIManager::ROIManager(QObject* parent) : QObject(parent)
{
  reset();
  m_listROI.reserve(20);
}

ROIManager::~ROIManager()
{
}

QList<QSharedPointer<const ROIBase>> ROIManager::hit(int x, int y)
{
  QList<QSharedPointer<const ROIBase>> list;
  for (auto& roi : m_listROI) {
    if (roi->contains(x, y)) {
      roi->setSelected(true);
      list.append(roi);
    } else {
      roi->setSelected(false);
    }
  }
  return list;
}

QList<QSharedPointer<const ROIBase>> ROIManager::getROIs() const
{
  QList<QSharedPointer<const ROIBase>> list;
  for (const QSharedPointer<const ROIBase> roi : m_listROI ){
    list.append(roi);
  }

  return list;
}

void ROIManager::addRectROI(int x, int y, int width, int height)
{
  m_listROI.append(QSharedPointer<RectROI>::create(m_iSNGen++, x, y, width, height));
  emit onListChanged(getROIs());
}

void ROIManager::addEllipROI(int x, int y, int major, int minor)
{
  m_listROI.append(QSharedPointer<EllipROI>::create(m_iSNGen++, x, y, major, minor));
  emit onListChanged(getROIs());
}

void ROIManager::addCirROI(int x, int y, int radius)
{
  m_listROI.append(QSharedPointer<CirROI>::create(m_iSNGen++, x, y, radius));
  emit onListChanged(getROIs());
}

void ROIManager::removeROI(unsigned int sn)
{
  for (const auto& roi : m_listROI) {
    if (roi->sn() == sn) {
      m_listROI.removeOne(roi);
      emit onListChanged(getROIs());
      break;
    }
  }
}

void ROIManager::adjustROI(unsigned int sn, const QRect& rect)
{
  auto& roi = getROI(sn);
  if (!roi.isNull()) {
    roi->setRect(rect);
    emit onListChanged(getROIs());
  }
}

void ROIManager::drawROIs(QPainter& painter, double scale, bool bRect, bool bEllip, bool bCir) const
{
  for (const auto& roi : m_listROI) {
    if (!roi->visable()) {
      continue;
    }
    if (roi->shape() == Shapes::UNDEFINE ||
        !bRect && roi->shape() == Shapes::RECTANGLE ||
        !bEllip && roi->shape() == Shapes::ELLIPSE ||
        !bCir && roi->shape() == Shapes::CIRCLE) {
      continue;
    }
    roi->draw(painter);
  }
}

void ROIManager::reset()
{
  m_iSNGen = 0;
  m_listROI.clear();
  emit onListChanged(getROIs());
}

void ROIManager::clearROIState(void)
{
  for (auto& roi : m_listROI) {
    roi->setSelected(false);
  }
}

QSharedPointer<const ROIBase> ROIManager::getROI(unsigned int sn) const
{
  for (auto& roi : m_listROI) {
    if (roi->sn() == sn) {
      return roi;
      break;
    }
  }

  return QSharedPointer<const ROIBase>();
}

QSharedPointer<ROIBase> ROIManager::getROI(unsigned int sn)
{
  for (auto& roi : m_listROI) {
    if (roi->sn() == sn) {
      return roi;
      break;
    }
  }

  return QSharedPointer<ROIBase>();
}
