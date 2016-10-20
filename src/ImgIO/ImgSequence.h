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

  auto root(void) const
  {
    return m_sRoot;
  }

  auto prefix(void) const
  {
    return m_sPrefix;
  }

  auto extension(void) const
  {
    return m_sExtName;
  }

  auto paddingLen(void) const
  {
    return m_iPadding;
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

typedef QSharedPointer<ImgSequence> ImgSeqPtr;

#endif // IMGSEQUENCE_H
