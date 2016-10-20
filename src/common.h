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

enum class SrcType
{
  IMAGE,
  VIDEO
};

enum class IOResult
{
  SUCCESSFUL,
  FILE_NOT_FOUND,
  SAVE_FAILED,
  TEMP_FILE_ERROR,
  CANCEL_OPERATION,
  FILE_NOT_SAVED,
  OPEN_FAILED,
  SETTING_NOT_CLEAR
};

const QMap<QString, Shapes> stringShape = {
  {QString("unknow"), Shapes::UNDEFINE},
  {QString("rectangle"), Shapes::RECTANGLE},
  {QString("ellipse"), Shapes::ELLIPSE},
  {QString("circle"), Shapes::CIRCLE}
};

namespace XMLLabel {
const QString tagRoot("Root");
const QString tagInfo("Information");
const QString tagType("Type");
const QString tagFirstNum("FirstNumber");
const QString tagEndNum("EndNumber");
const QString tagPaddingLength("PaddingLength");
const QString tagExt("Extension");
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
  const Qt::Key del = Qt::Key_Delete;
  const Qt::Key next = Qt::Key_Period;
  const Qt::Key prevuous = Qt::Key_Comma;
}

namespace Colors {
  const Qt::GlobalColor normal = Qt::green;
  const Qt::GlobalColor disabled = Qt::gray;
}

namespace LineStyle {
  const Qt::PenStyle normal = Qt::SolidLine;
  const Qt::PenStyle disabled = Qt::DotLine;
}

#endif // COMMON_H
