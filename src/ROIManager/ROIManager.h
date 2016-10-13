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
  QList<QSharedPointer<const ROIBase>> hit(int x, int y);

  QList<QSharedPointer<const ROIBase>> getROIs(void) const;

  QSharedPointer<const ROIBase> getROI(unsigned int sn) const;

  void addRectROI(int x, int y, int width, int height);

  void addEllipROI(int x, int y, int major, int minor);

  void addCirROI(int x, int y, int radius);

  void removeROI(unsigned int sn);

  void adjustROI(unsigned int sn, const QRect& rect);

  void drawROIs(QPainter& painter, double scale = 1.0, bool bRect = true, bool bEllip = true, bool bCir = true) const;

  void reset(void);

signals:
  void onListChanged(const QList<QSharedPointer<const ROIBase>> listpROIs);

private:
  void clearROIState(void);

  QSharedPointer<ROIBase> getROI(unsigned int sn);
};

#endif // ROIMANAGER_H
