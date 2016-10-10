#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QList>
#include <QPoint>
#include "ROI/ROIBase.h"
#include "ROI/RectROI.h"
#include "ROI/EllipROI.h"
#include "ROI/CirROI.h"

class ROIManager : public QObject
{
  Q_OBJECT
public:
  ROIManager(QObject *parent = 0);

  virtual ~ROIManager();

private:
  QList<QSharedPointer<ROIBase>> m_listROI;
  unsigned int m_iSNGen;

public:
  QList<QSharedPointer<ROIBase>> hit(int x, int y);

  const auto& getROIs(void) const
  {
    return m_listROI;
  }

  QSharedPointer<ROIBase>& getROI(unsigned int sn);

  void addRectROI(int x, int y, int width, int height)
  {
    m_listROI.append(QSharedPointer<RectROI>::create(m_iSNGen++, x, y, width, height));
  }

  void addRectROI(const cv::Point2i& ptLT, const cv::Point2i& ptRB);

  void addRectROI(const QPoint& ptLT, const QPoint& ptRB)
  {
    addRectROI(cv::Point2i(ptLT.x(), ptLT.y()), cv::Point2i(ptRB.x(), ptRB.y()));
  }

  void addEllipROI(int x, int y, int major, int minor)
  {
    m_listROI.append(QSharedPointer<EllipROI>::create(m_iSNGen++, x, y, major, minor));
  }

  void addCirROI(int x, int y, int radius)
  {
    m_listROI.append(QSharedPointer<CirROI>::create(m_iSNGen++, x, y, radius));
  }

  void drawROIs(cv::Mat& image);

  void reset(void);

private:
  void clearROIState(void);

};

#endif // ROIMANAGER_H
