#ifndef ROIMANAGER_H
#define ROIMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QList>

class ROIBase;
class QPainter;

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

  void addRectROI(int x, int y, int width, int height);

  void addEllipROI(int x, int y, int major, int minor);

  void addCirROI(int x, int y, int radius);

  void removeROI(unsigned int sn);

  void drawROIs(QPainter& painter, double scale = 1.0, bool bRect = true, bool bEllip = true, bool bCir = true) const;

  void reset(void);

signals:
  void countChanged(const QList<QSharedPointer<ROIBase>>& listpROIs);

private:
  void clearROIState(void);
};

#endif // ROIMANAGER_H
