#include "ROIManager.h"
#include <QRect>

ROIManager::ROIManager(QObject* parent) : QObject(parent)
{
  m_listROI.reserve(20);
}

ROIManager::~ROIManager()
{
}

QList<QSharedPointer<ROIBase>> ROIManager::hit(int x, int y)
{
  QList<QSharedPointer<ROIBase>> list;
  for (auto& roi : m_listROI) {
    if (roi->checkAmbit(x, y)) {
      roi->setSelected(true);
      list.append(roi);
    } else {
      roi->setSelected(false);
    }
  }
  return list;
}

QSharedPointer<ROIBase>& ROIManager::getROI(unsigned int sn)
{
  for (auto& roi : m_listROI) {
    if (roi->getSN() == sn) {
      return roi;
      break;
    }
  }

  return QSharedPointer<ROIBase>();
}

void ROIManager::addRectROI(const cv::Point2i& ptLT, const cv::Point2i& ptRB)
{
  int x = ptLT.x;
  int y = ptLT.y;
  int width = ptRB.x - ptLT.x;
  int height = ptRB.y - ptLT.y;
  addRectROI(x, y, width, height);
}

void ROIManager::drawROIs(cv::Mat &image)
{
  for (auto roi : m_listROI) {
    roi->draw(image);
  }
}

void ROIManager::reset()
{
  m_iSNGen = 0;
  m_listROI.clear();
}

void ROIManager::clearROIState(void)
{
  for (auto& roi : m_listROI) {
    roi->setSelected(false);
  }
}
