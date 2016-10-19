#ifndef IMGSEQUENCE_H
#define IMGSEQUENCE_H

#include "ImgIOBase.h"

#include <QMap>

class QString;
class QChar;

class ImgSequence : public ImgIOBase
{
private:
  QString m_sRoot;
  QString m_sPrefix;
  QString m_sExtName;
  unsigned int m_iPadding;
  int m_iReadingFlag;

public:
  explicit ImgSequence(const QString& root, const QString& extName, const QString& prefix, unsigned int length, unsigned int first);

public:
  virtual QString srcTypeStr(void) const
  {
    return QString("ImgSeq");
  }

  virtual QPixmap queryNext(void);

  virtual QPixmap queryPre(void);

  virtual QPixmap query(unsigned int num);

  bool jumpTo(unsigned int num);

  virtual bool hasNext(void);

private:
  QString getPath(unsigned int num) const;

  QPixmap loadImg(unsigned int num) const;
};

#endif // IMGSEQUENCE_H
