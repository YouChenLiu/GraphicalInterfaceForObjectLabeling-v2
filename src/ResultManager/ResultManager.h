#ifndef RESULTMANAGER_H
#define RESULTMANAGER_H

#include <QObject>
#include <QDomDocument>
#include <QDate>
#include "common.h"

class ROIBase;

class ResultManager : public QObject
{
  Q_OBJECT
public:
  explicit ResultManager(QObject *parent = 0);

signals:

public slots:

private:
  struct ResultInfo {
    QString Type;
    int iFirstNum;
    int iEndNum;
    int iLeadingLength;
    char cLeadingChar;
    QString Path;
    QDate CreationDate;
    QDate ModifyDate;
  };

public:
  void create(void);
  IOResult openFile(const QString& path);
  IOResult saveFile(void);
  IOResult saveTempFile(void);
  IOResult closeFile(void);
  void setFilePath(const QString& path)
  {
    filePath = path;
  }

  void addROIs(int iFrameNum, const QList<QSharedPointer<const ROIBase>>& ROIs);

  const QList<QSharedPointer<ROIBase>> getROIs(int iFrameNum) const;
private:
  QString tempPath;
  QString filePath;
  QDomDocument result;
  ResultInfo m_ResultInfo;

private:

};

#endif // RESULTMANAGER_H
