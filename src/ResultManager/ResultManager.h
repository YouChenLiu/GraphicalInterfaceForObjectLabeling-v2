#ifndef RESULTMANAGER_H
#define RESULTMANAGER_H

#include <QObject>
#include <QDomDocument>
#include <QSet>
#include "common.h"

class ROIBase;
class QChar;

class ResultManager : public QObject
{
  Q_OBJECT
public:
  explicit ResultManager(QObject *parent = 0);

signals:

public slots:

private:
  struct Info {
    QString Type;
    int iFirstNum;
    int iEndNum;
    int iPaddingLength;
    QChar cPaddingChar;
    QString sExtension;
    QString CreationDate;
    QString ModifyDate;
  } m_Info;

public:
  auto type(void) const
  {
    return m_Info.Type;
  }
  auto firstNum(void)
  {
    return m_Info.iFirstNum;
  }
  auto endNum(void) const
  {
    return m_Info.iEndNum;
  }
  auto paddingLength(void) const
  {
    return m_Info.iPaddingLength;
  }
  auto paddingChar(void) const
  {
    return m_Info.cPaddingChar;
  }
  auto extension(void) const
  {
    return m_Info.sExtension;
  }

private:
  QString m_sTempPath;
  QString m_sFilePath;
  QDomDocument m_Document;
  QSet<unsigned int> m_Frames;

public:
  void create(void);

  IOResult openFile(QString& path = QString());

  IOResult saveFile(void);

  IOResult saveTempFile(void);

  IOResult saveAs(void);

  IOResult closeFile(bool force = false);

  void addROIs(int iFrameNum, const QList<QSharedPointer<const ROIBase>>& ROIs);

  QList<QSharedPointer<ROIBase>> getROIs(int iFrameNum) const;

  auto filePath(void) const
  {
    return m_sFilePath;
  }

  bool hasFrame(int iFrameNum) const
  {
    return m_Frames.contains(iFrameNum);
  }

private:
  IOResult saveTo(const QString& path);

  void readXML(void);
};

#endif // RESULTMANAGER_H
