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
    SrcType Type;
    unsigned int iFirstNum;
    unsigned int iEndNum;
    unsigned int iPaddingLength;
    QString sExtension;
    QString CreationDate;
  } m_Info;

  const QMap<SrcType, QString> TypeStr = {
    {SrcType::IMAGE, QString("ImgSeq")},
    {SrcType::VIDEO, QString("Video")}
  };

  const QMap<QString, SrcType> StrType = {
    {QString("ImgSeq"), SrcType::IMAGE},
    {QString("Video"), SrcType::VIDEO}
  };

public:
  auto type(void) const
  {
    return m_Info.Type;
  }

  void setType(SrcType type);

  auto firstNum(void)
  {
    return m_Info.iFirstNum;
  }

  void setFirstNum(unsigned int value);

  auto endNum(void) const
  {
    return m_Info.iEndNum;
  }

  void setEndNum(unsigned int value);

  auto paddingLength(void) const
  {
    return m_Info.iPaddingLength;
  }

  void setPaddingLength(unsigned int value);

  auto extension(void) const
  {
    return m_Info.sExtension;
  }

  void setExtension(const QString& ext);

private:
  QString m_sTempPath;
  QString m_sFilePath;
  QDomDocument m_Document;
  QSet<unsigned int> m_Frames;
  QString m_sDefaultPath;
  bool m_bTempFileSaved;

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

  void setDefaultPath(const QString& path)
  {
    m_sDefaultPath = path;
  }

private:
  QDomElement createTextTag(const QString& tagName, const QString& text);

  IOResult saveTo(const QString& path);

  void readXML(void);

  void modifyInfoTag(const QDomElement& tag);
};

#endif // RESULTMANAGER_H
