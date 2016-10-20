#include "ResultManager.h"
#include <QDir>
#include <QFile>
#include <QDate>
#include <QTextStream>
#include <QSharedPointer>
#include <QList>
#include <QStringList>
#include <QFileDialog>
#include <QCryptographicHash>
#include "ROI/RectROI.h"
#include "ROI/EllipROI.h"
#include "ROI/CirROI.h"

ResultManager::ResultManager(QObject *parent) : QObject(parent)
{
  m_sTempPath = QDir::tempPath() + "/temp.xml";
  m_sDefaultPath = QString();
  m_bTempFileSaved = false;
  create();
}

void ResultManager::setType(SrcType type)
{
  // set type tag
  auto infoTag = m_Document.firstChildElement(XMLLabel::tagRoot).firstChildElement(XMLLabel::tagInfo);
  auto typeTag = infoTag.firstChildElement(XMLLabel::tagType);
  auto typeText = m_Document.createTextNode(TypeStr[type]);
  typeTag.appendChild(typeText);

  if (type == SrcType::IMAGE) {
    auto firstNumTag = m_Document.createElement(XMLLabel::tagFirstNum);
    infoTag.appendChild(firstNumTag);

    auto endNumTag = m_Document.createElement(XMLLabel::tagEndNum);
    infoTag.appendChild(endNumTag);

    auto paddingLengthTag = m_Document.createElement(XMLLabel::tagPaddingLength);
    infoTag.appendChild(paddingLengthTag);

    auto extTag = m_Document.createElement(XMLLabel::tagExt);
    infoTag.appendChild(extTag);
  } else if (type == SrcType::VIDEO) {

  }
}

void ResultManager::setFirstNum(unsigned int value)
{
  m_Info.iFirstNum = value;

  auto firstNumTag = createTextTag(XMLLabel::tagFirstNum, QString::number(value));
  modifyInfoTag(firstNumTag);
}

void ResultManager::setEndNum(unsigned int value)
{
  m_Info.iEndNum = value;

  auto endNumTag = createTextTag(XMLLabel::tagEndNum, QString::number(value));
  modifyInfoTag(endNumTag);
}

void ResultManager::setPaddingLength(unsigned int value)
{
  m_Info.iPaddingLength = value;

  auto paddingLengthTag = createTextTag(XMLLabel::tagPaddingLength, QString::number(value));
  modifyInfoTag(paddingLengthTag);
}

void ResultManager::setExtension(const QString& ext)
{
  m_Info.sExtension = ext;

  auto extTag = createTextTag(XMLLabel::tagExt, ext);
  modifyInfoTag(extTag);
}

IOResult ResultManager::openFile(QString& path)
{
  if (path.isEmpty()) {
    path = QFileDialog::getOpenFileName(nullptr, tr("Select XML File"), QString(), tr("XML (*.xml)"));
    if (path.isEmpty()) {
      return IOResult::CANCEL_OPERATION;
    }
  }

  m_sFilePath = path;

  QFile file(path);
  if (!file.exists()) {
    return IOResult::FILE_NOT_FOUND;
  } else {
    QFile::copy(path, m_sTempPath);
    QFile tmpFile(m_sTempPath);
    tmpFile.open(QIODevice::ReadOnly);
    if (!m_Document.setContent(&tmpFile)) {
        file.close();
        tmpFile.close();
        return IOResult::TEMP_FILE_ERROR;
    }
  }

  readXML();
  return IOResult::SUCCESSFUL;
}

IOResult ResultManager::saveFile(void)
{
  if (m_sFilePath.isEmpty()) {
    m_sFilePath = QFileDialog::getSaveFileName(nullptr, tr("Save File"), m_sDefaultPath, tr("XML (*.xml)"));
  }
  return saveTo(m_sFilePath);
}

IOResult ResultManager::saveTempFile(void)
{
  m_bTempFileSaved = true;
  return saveTo(m_sTempPath);
}

IOResult ResultManager::saveAs()
{
  QString newPath = QFileDialog::getSaveFileName(nullptr, tr("Select XML File"), m_sFilePath, tr("XML (*.xml)"));
  if (!newPath.isEmpty()) {
    return IOResult::CANCEL_OPERATION;
  }

  m_sFilePath = newPath;
  return saveTo(m_sFilePath);
}

IOResult ResultManager::closeFile(bool force)
{
  if (m_bTempFileSaved && m_sFilePath.isEmpty()) {
    return IOResult::FILE_NOT_SAVED;
  }

  if (!m_bTempFileSaved) {
    force = true;
  }

  QFile tempFile(m_sTempPath);

  if (!force) {
    if (m_sFilePath.isNull()) {
      return IOResult::FILE_PATH_NOT_SET;
    }
    QFile file(m_sFilePath);
    file.open(QIODevice::ReadOnly);
    tempFile.open(QIODevice::ReadOnly);
    // check the change of temp file by computing hash
    QCryptographicHash hash(QCryptographicHash::Sha1);
    QCryptographicHash tempHash(QCryptographicHash::Sha1);
    if (hash.addData(&file) && tempHash.addData(&tempFile)) {
      // compare hash code
      auto result = hash.result();
      auto tempResult = tempHash.result();
      if (result != tempResult) {
        return IOResult::FILE_NOT_SAVED;
      }
    } else {
      return IOResult::OPEN_FAILED;
    }
  }

  // save to the right file or discard changes

  // delete the temp file
  tempFile.remove();

  return IOResult::SUCCESSFUL;
}

void ResultManager::addROIs(unsigned int iFrameNum, const QList<QSharedPointer<const ROIBase> >& ROIs)
{
  // create new frame element
  auto newFrame = m_Document.createElement(XMLLabel::tagFrame);
  newFrame.setAttribute(XMLLabel::attrNum, iFrameNum);
  newFrame.setAttribute(XMLLabel::attrRecords, ROIs.count());

  // convert all rois to xml element and append to the frame
  for (auto roi : ROIs) {
    // create record element
    auto recordElement = m_Document.createElement(XMLLabel::tagRecord);

    // add sn attribute
    recordElement.setAttribute(XMLLabel::attrSN, roi->sn());

    // shape
    auto shapeTag = createTextTag(XMLLabel::tagShape, roi->getShapeString());
    recordElement.appendChild(shapeTag);

    // start point
    auto pt = roi->topLeft();
    auto startPtStr = QString("%1, %2").arg(pt.x()).arg(pt.y());
    auto startPtTag = createTextTag(XMLLabel::tagStartPoint, startPtStr);
    recordElement.appendChild(startPtTag);

    // width
    auto widthStr = QString::number(roi->width());
    auto widthTag = createTextTag(XMLLabel::tagWidth, widthStr);
    recordElement.appendChild(widthTag);

    // height
    auto heightStr = QString::number(roi->height());
    auto heightTag = createTextTag(XMLLabel::tagHeight, heightStr);
    recordElement.appendChild(heightTag);

    // add record to frame element
    newFrame.appendChild(recordElement);
  }

  auto dataSet = m_Document.firstChildElement(XMLLabel::tagRoot).firstChildElement(XMLLabel::tagDataSet);

  // check the document has the same frame or not
  if (hasFrame(iFrameNum)) {
    // the document has the old one
    // find it and replace by new one
    auto frameList = dataSet.elementsByTagName(XMLLabel::tagFrame);
    for (int i = 0; i < frameList.length(); ++i) {
      auto frame = frameList.item(i).toElement();
      if (frame.attribute(XMLLabel::attrNum).toInt() == iFrameNum) {
        dataSet.replaceChild(newFrame, frame);
        break;
      }
    }
  } else {
    // the document dosn't have the frame
    // append it and add number to set
    dataSet.appendChild(newFrame);
    m_Frames.insert(iFrameNum);

    // check the end number of information
    if (m_Info.iEndNum < iFrameNum) {
      m_Info.iEndNum = iFrameNum;
      auto endNumTag = createTextTag(XMLLabel::tagEndNum, QString::number(iFrameNum));
      modifyInfoTag(endNumTag);
    }

  }
}

QList<QSharedPointer<ROIBase>> ResultManager::getROIs(int iFrameNum) const
{
  QList<QSharedPointer<ROIBase>> ROIs;
  // get all frames
  const auto dataSet = m_Document.firstChildElement(XMLLabel::tagRoot).firstChildElement(XMLLabel::tagDataSet);
  const auto frameList = dataSet.elementsByTagName(XMLLabel::tagFrame);

  // find the target tag
  QDomElement frame;
  for (int i = 0; i < frameList.length(); ++i) {
    frame = frameList.item(i).toElement();
    if (frame.attribute(XMLLabel::attrNum).toInt() == iFrameNum) {
      break;
    }
  }

  if (frame.isNull()) {
    return ROIs;
  }

  // reserve capacity
  int iNumOfRecord = frame.attribute(XMLLabel::attrRecords).toInt();
  ROIs.reserve(iNumOfRecord);

  // go through all record
  const auto records = frame.elementsByTagName(XMLLabel::tagRecord);
  for (int i = 0; i < records.length(); ++i) {
    auto record = records.item(i).toElement();
    // convert record to ROI object
    QSharedPointer<ROIBase> pROI;
    {
      int iSN = record.attribute(XMLLabel::attrSN).toInt();
      QString sShape = record.firstChildElement(XMLLabel::tagShape).text();
      int x = 0, y = 0;
      // use regular expression split to digit
      QString SP = record.firstChildElement(XMLLabel::tagStartPoint).text();
      QStringList list = SP.split(",", QString::SkipEmptyParts);
      x = list.at(0).toInt();
      y = list.at(1).toInt();
      int iHeight = record.firstChildElement(XMLLabel::tagHeight).text().toInt();
      int iWidth = record.firstChildElement(XMLLabel::tagWidth).text().toInt();
      switch (stringShape[sShape]) {
      case Shapes::RECTANGLE:
        pROI = QSharedPointer<RectROI>::create(iSN, x, y, iWidth, iHeight);
        break;
      case Shapes::ELLIPSE:
        pROI = QSharedPointer<EllipROI>::create(iSN, x, y, iWidth, iHeight);
        break;
      case Shapes::CIRCLE:
        pROI = QSharedPointer<CirROI>::create(iSN, x, y, iWidth);
        break;
      default:
        pROI = QSharedPointer<ROIBase>();
        pROI.clear();
        break;
      }
    }

    if (pROI) {
      ROIs.append(pROI);
    }
  }

  return ROIs;
}

void ResultManager::reset()
{
  m_Document = QDomDocument();
  m_sDefaultPath.clear();
  m_sFilePath.clear();
  m_Frames.clear();
  m_bTempFileSaved = false;
}

QDomElement ResultManager::createTextTag(const QString& tagName, const QString& text)
{
  auto newTag = m_Document.createElement(tagName);
  auto newTagStr = m_Document.createTextNode(text);
  newTag.appendChild(newTagStr);
  return newTag;
}

IOResult ResultManager::saveTo(const QString& path)
{
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    file.close();
    return IOResult::SAVE_FAILED;
  }

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream.setGenerateByteOrderMark(true);
  stream << m_Document.toString();
  file.close();

  return IOResult::SUCCESSFUL;
}

void ResultManager::readXML()
{
  const auto root =  m_Document.firstChildElement(XMLLabel::tagRoot);
  const auto info = root.firstChildElement(XMLLabel::tagInfo);
  if (info.isNull()) {
    return;
  }

  auto sType = info.firstChildElement(XMLLabel::tagType).text();
  m_Info.Type = StrType[sType];
  switch (m_Info.Type) {
  case SrcType::IMAGE:
    m_Info.CreationDate = info.firstChildElement(XMLLabel::tagCreationDate).text();
    m_Info.iFirstNum = info.firstChildElement(XMLLabel::tagFirstNum).text().toInt();
    m_Info.iEndNum = info.firstChildElement(XMLLabel::tagEndNum).text().toInt();
    m_Info.iPaddingLength = info.firstChildElement(XMLLabel::tagPaddingLength).text().toInt();
    m_Info.sExtension = info.firstChildElement(XMLLabel::tagExt).text();

    for (auto i = m_Info.iFirstNum; i <= m_Info.iEndNum; ++i) {
      m_Frames.insert(i);
    }
    break;
  case SrcType::VIDEO:
    break;
  default:
    break;
  }
}

void ResultManager::modifyInfoTag(const QDomElement& tag)
{
  auto infoTag = m_Document.firstChildElement(XMLLabel::tagRoot).firstChildElement(XMLLabel::tagInfo);
  auto result = infoTag.elementsByTagName(tag.tagName());
  if (result.isEmpty()) {
    infoTag.appendChild(tag);
  } else {
    infoTag.replaceChild(tag, result.item(0));
  }
}

void ResultManager::create(void)
{
  //setting XML declaration
  auto declaration = m_Document.createProcessingInstruction(
        tr("xml"),
        tr("version=\"1.0\" encoding=\"UTF-8\""));
  m_Document.appendChild(declaration);

  //setting root tag
  QDomElement rootTag = m_Document.createElement(XMLLabel::tagRoot);

  // create information tag
  QDomElement infoTag = m_Document.createElement(XMLLabel::tagInfo);

  // set type tag
  QDomElement typeTag = m_Document.createElement(XMLLabel::tagType);
  infoTag.appendChild(typeTag);

  // set create date tag
  QString sDate = QDate::currentDate().toString("yyyy/MM/dd");
  QDomElement createDateTag = createTextTag(XMLLabel::tagCreationDate, sDate);
  infoTag.appendChild(createDateTag);

  // add information tag
  rootTag.appendChild(infoTag);

  //setting date set tag
  QDomElement dataSetTag = m_Document.createElement(XMLLabel::tagDataSet);
  rootTag.appendChild(dataSetTag);

  m_Document.appendChild(rootTag);
}
