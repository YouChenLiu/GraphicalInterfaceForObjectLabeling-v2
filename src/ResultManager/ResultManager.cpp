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
#include <QDebug>

ResultManager::ResultManager(QObject *parent) : QObject(parent)
{
  m_sTempPath = QDir::tempPath() + "/temp.xml";
  create();
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
    m_sFilePath = QFileDialog::getSaveFileName(nullptr, tr("Save File"));
  }
  return saveTo(m_sFilePath);
}

IOResult ResultManager::saveTempFile(void)
{
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

  QFile file(m_sFilePath);
  QFile tempFile(m_sTempPath);
  if (m_sFilePath.isEmpty()) {
    force = true;
  }
  if (!force) {
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
      } else {
        qDebug() << "same hash";
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

void ResultManager::addROIs(int iFrameNum, const QList<QSharedPointer<const ROIBase> >& ROIs)
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
    auto shapeText = m_Document.createTextNode(roi->getShapeString());
    auto shapeElement = m_Document.createElement(XMLLabel::tagShape);
    shapeElement.appendChild(shapeText);
    recordElement.appendChild(shapeElement);

    // start point
    auto pt = roi->topLeft();
    auto SPText = m_Document.createTextNode(QString("%1, %2").arg(pt.x()).arg(pt.y()));
    auto SPElement = m_Document.createElement(XMLLabel::tagStartPoint);
    SPElement.appendChild(SPText);
    recordElement.appendChild(SPElement);

    // width
    auto widthText = m_Document.createTextNode(QString::number(roi->width()));
    auto widthElement = m_Document.createElement(XMLLabel::tagWidth);
    widthElement.appendChild(widthText);
    recordElement.appendChild(widthElement);

    // height
    auto height = roi->height();
    auto heightText = m_Document.createTextNode(QString::number(height));
    auto heightElement = m_Document.createElement(XMLLabel::tagHeight);
    heightElement.appendChild(heightText);
    recordElement.appendChild(heightElement);

    // add record to frame element
    newFrame.appendChild(recordElement);
  }

  // check the document has the same frame or not
  if (hasFrame(iFrameNum)) {
    // the document has the old one
    // find it and replace by new one
    auto dataSet = m_Document.firstChildElement(XMLLabel::tagRoot).firstChildElement(XMLLabel::tagDataSet);
    auto frameList = dataSet.elementsByTagName(XMLLabel::tagFrame);
    for (int i = 0; i < frameList.length(); ++i) {
      auto& frame = frameList.item(i).toElement();
      if (frame.attribute(XMLLabel::attrNum).toInt() == iFrameNum) {
        dataSet.replaceChild(newFrame, frame);
        break;
      }
    }
  } else {
    // the document dosn't have the frame
    // append it and add number to set
    auto dataSet = m_Document.documentElement().firstChildElement(XMLLabel::tagDataSet);
    dataSet.appendChild(newFrame);
    m_Frames.insert(iFrameNum);
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
  m_Info.Type = info.firstChildElement(XMLLabel::tagType).text();
  m_Info.CreationDate = info.firstChildElement(XMLLabel::tagCreationDate).text();
  m_Info.iFirstNum = info.firstChildElement(XMLLabel::tagFirstNum).text().toInt();
  m_Info.iEndNum = info.firstChildElement(XMLLabel::tagEndNum).text().toInt();
  m_Info.iPaddingLength = info.firstChildElement(XMLLabel::tagPaddingLength).text().toInt();
  m_Info.sExtension = info.firstChildElement(XMLLabel::tagExt).text();

  for (int i = m_Info.iFirstNum; i <= m_Info.iEndNum; ++i) {
    m_Frames.insert(i);
  }
}

void ResultManager::create(void)
{
  //setting XML declaration
  auto declaration = m_Document.createProcessingInstruction(tr("xml"),
                                                        tr("version=\"1.0\" encoding=\"UTF-8\""));
  m_Document.appendChild(declaration);
  using namespace XMLLabel;
  //setting video name tag
//  QDomText videoNameText = result.createTextNode("");
//  QDomElement videoNameTag = result.createElement(tag);
//  videoNameTag.appendChild(videoNameText);
  //setting create date tag
  QDomElement createDateTag = m_Document.createElement(tagCreationDate);
  QDate currentDate = QDate::currentDate();
  QDomText createDateText = m_Document.createTextNode(currentDate.toString("yyyy/MM/dd"));
  createDateTag.appendChild(createDateText);
  //setting lastest modify date tag
  QDomElement lastModifyDateTag = m_Document.createElement(tagModifyDate);
  //setting file description tag
  QDomElement infoTag = m_Document.createElement(tagInfo);
//  infoTag.appendChild(videoNameTag);
  infoTag.appendChild(createDateTag);
  infoTag.appendChild(lastModifyDateTag);
  //setting header tag with frame 0
//  QDomElement headerTag = result.createElement(tr(HEADER_TAG));
//  headerTag.setAttribute(tr(FRAME_NUMBER_ATTRIBUTE), 0);
//  headerTag.setAttribute(tr(TOTAL_RECORD_ATTRIBUTE), 0);
  //setting date set tag
  QDomElement dataSetTag = m_Document.createElement(tagDataSet);
//  dataSetTag.appendChild(headerTag);
  //setting root tag
  QDomElement rootTag = m_Document.createElement(tagRoot);
  rootTag.appendChild(infoTag);
  rootTag.appendChild(dataSetTag);
  m_Document.appendChild(rootTag);
}
