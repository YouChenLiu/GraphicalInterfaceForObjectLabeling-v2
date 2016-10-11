#ifndef COMMON_H
#define COMMON_H

#include <QMap>
#include <QString>

constexpr int DEFAULT_WIDTH = 640;
constexpr int DEFAULT_HEIGHT = 480;

enum class Shapes
{
  UNDEFINE = -1,
  RECTANGLE = 0,
  ELLIPSE,
  CIRCLE
};

enum class IOResult
{
  SUCCESSFUL,
  FILE_NOT_FOUND,
  SAVE_FAILED
};

const QMap<QString, Shapes> shapeString = {
  {QString("unknow"), Shapes::UNDEFINE},
  {QString("rectangle"), Shapes::RECTANGLE},
  {QString("ellipse"), Shapes::ELLIPSE},
  {QString("circle"), Shapes::CIRCLE}
};

namespace XMLLabel {
const QString tagRoot("Root");
const QString tagInfo("Infomation");
const QString tagType("Type");
const QString tagFirstNum("FirstNumber");
const QString tagEndNum("EndNumber");
const QString tagLeadingLength("LeadingLength");
const QString tagLeadingChar("LeadingCharacter");
const QString tagPath("Path");
const QString tagCreationDate("CreationDate");
const QString tagModifyDate("ModifyDate");
const QString tagDataSet("DataSet");
const QString tagFrame("Frame");
const QString tagRecord("Record");
const QString tagShape("Shape");
const QString tagStartPoint("StartPoint");
const QString tagHeight("Height");
const QString tagWidth("Width");

const QString attrNum("number");
const QString attrRecords("records");
const QString attrSN("sn");
}

namespace Keys {
  const Qt::Key move = Qt::Key_Shift;
}

#endif // COMMON_H
