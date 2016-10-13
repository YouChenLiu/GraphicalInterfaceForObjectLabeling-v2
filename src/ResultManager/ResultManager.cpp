#include "ResultManager.h"
#include <QDir>
#include <QFile>
#include <QDate>
#include <QTextStream>
#include <QSharedPointer>
#include <QList>
#include <QStringList>
#include "ROI/RectROI.h"
#include "ROI/EllipROI.h"
#include "ROI/CirROI.h"

ResultManager::ResultManager(QObject *parent) : QObject(parent)
{
  tempPath = QDir::tempPath() + "/temp.xml";
  create();
}

IOResult ResultManager::openFile(const QString &path)
{
  setFilePath(path);
  QFile file(path);
  if (file.exists()) {
    QFile::copy(path, tempPath);
  } else {
    return IOResult::FILE_NOT_FOUND;
  }

  return IOResult::SUCCESSFUL;
}

IOResult ResultManager::saveFile(void)
{

  return IOResult::SUCCESSFUL;
}

IOResult ResultManager::saveTempFile(void)
{
  QFile file(tempPath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    file.close();
    return IOResult::SAVE_FAILED;
  }

  QTextStream stream(&file);
  stream.setCodec("UTF-8");
  stream.setGenerateByteOrderMark(true);
  stream << result.toString();
  file.close();

  return IOResult::SUCCESSFUL;
}

IOResult ResultManager::closeFile()
{
  // check the change of temp file
  // save to the right file or discard changes
  // delete the temp file
  QFile file(tempPath);
  file.remove();

  return IOResult::SUCCESSFUL;
}

void ResultManager::addROIs(int iFrameNum, const QList<QSharedPointer<const ROIBase> >& ROIs)
{
  // search the frame number then add the roi

  // create new frame element
  auto frameTag = result.createElement(XMLLabel::tagFrame);
  frameTag.setAttribute(XMLLabel::attrNum, iFrameNum);
  frameTag.setAttribute(XMLLabel::attrRecords, ROIs.count());

  // convert all rois to xml element and append to the frame
  for (auto roi : ROIs) {
    // create record element
    auto RecordElement = result.createElement(XMLLabel::tagRecord);
    // shape
    auto ShapeText = result.createTextNode(roi->getShapeString());
    auto ShapeElement = result.createElement(XMLLabel::tagShape);
    ShapeElement.appendChild(ShapeText);
    RecordElement.appendChild(ShapeElement);

    // start point
    auto pt = roi->topLeft();
    auto SPText = result.createTextNode(QString("%1, %2").arg(pt.x()).arg(pt.y()));
    auto SPElement = result.createElement(XMLLabel::tagStartPoint);
    SPElement.appendChild(SPText);
    RecordElement.appendChild(SPElement);

    // width
    auto widthText = result.createTextNode(QString::number(roi->width()));
    auto widthElement = result.createElement(XMLLabel::tagWidth);
    widthElement.appendChild(widthText);
    RecordElement.appendChild(widthElement);

    // height
    auto height = roi->height();
    auto heightText = result.createTextNode(QString::number(height));
    auto heightElement = result.createElement(XMLLabel::tagHeight);
    heightElement.appendChild(heightText);
    RecordElement.appendChild(heightElement);

    frameTag.appendChild(RecordElement);
  }
  auto dataSet = result.documentElement().firstChildElement(XMLLabel::tagDataSet);
  dataSet.appendChild(frameTag);
}

const QList<QSharedPointer<ROIBase>> ResultManager::getROIs(int iFrameNum) const
{
  QList<QSharedPointer<ROIBase>> ROIs;
  using namespace XMLLabel;
  // get all frames
  const auto dataSet = result.firstChildElement(tagRoot).firstChildElement(tagDataSet);
  const auto frames = dataSet.elementsByTagName(tagFrame);
  // check the result
  if (frames.isEmpty()) {
    return ROIs;
  }

  // get the frame element
  auto frame = frames.item(iFrameNum - m_ResultInfo.iFirstNum);
  if (frame.isNull()) {
    return ROIs;
  }
  // reserve space
  int iNumOfRecord = frame.toElement().attribute(attrRecords).toInt();
  ROIs.reserve(iNumOfRecord);

  auto record = frame.firstChildElement(tagRecord);
  while (!record.isNull()) {
    // convert record to ROI object by lambda function
    ROIs.append([&](const QDomElement record) -> QSharedPointer<ROIBase> {
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
      if (sShape.compare(QString("rectangle"),Qt::CaseInsensitive) == 0) {
        return QSharedPointer<RectROI>::create(iSN, x, y, iWidth, iHeight);
      } else if (sShape.compare(QString("ellipse"),Qt::CaseInsensitive) == 0) {
        return QSharedPointer<EllipROI>::create(iSN, x, y, iWidth, iHeight);
      } else if (sShape.compare(QString("circle"),Qt::CaseInsensitive) == 0) {
        return QSharedPointer<CirROI>::create(iSN, x, y, iWidth);
      } else {
        return QSharedPointer<ROIBase>();
      }
    }(record));
    record = record.nextSiblingElement();
  }

  return ROIs;
}

void ResultManager::create(void)
{
  //setting XML declaration
  auto declaration = result.createProcessingInstruction(tr("xml"),
                                                        tr("version=\"1.0\" encoding=\"UTF-8\""));
  result.appendChild(declaration);
  using namespace XMLLabel;
  //setting video name tag
//  QDomText videoNameText = result.createTextNode("");
//  QDomElement videoNameTag = result.createElement(tag);
//  videoNameTag.appendChild(videoNameText);
  //setting create date tag
  QDomElement createDateTag = result.createElement(tagCreationDate);
  QDate currentDate = QDate::currentDate();
  QDomText createDateText = result.createTextNode(currentDate.toString("yyyy/MM/dd"));
  createDateTag.appendChild(createDateText);
  //setting lastest modify date tag
  QDomElement lastModifyDateTag = result.createElement(tagModifyDate);
  //setting file description tag
  QDomElement infoTag = result.createElement(tagInfo);
//  infoTag.appendChild(videoNameTag);
  infoTag.appendChild(createDateTag);
  infoTag.appendChild(lastModifyDateTag);
  //setting header tag with frame 0
//  QDomElement headerTag = result.createElement(tr(HEADER_TAG));
//  headerTag.setAttribute(tr(FRAME_NUMBER_ATTRIBUTE), 0);
//  headerTag.setAttribute(tr(TOTAL_RECORD_ATTRIBUTE), 0);
  //setting date set tag
  QDomElement dataSetTag = result.createElement(tagDataSet);
//  dataSetTag.appendChild(headerTag);
  //setting root tag
  QDomElement rootTag = result.createElement(tagRoot);
  rootTag.appendChild(infoTag);
  rootTag.appendChild(dataSetTag);
  result.appendChild(rootTag);
}
