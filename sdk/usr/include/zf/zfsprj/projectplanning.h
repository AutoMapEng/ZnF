#pragma once
#include "ZfsPrjBasic.h"
#include <zfcore/compiler.h>
#include <Common/def.h>
SWITCH_WARNINGS_OFF
#include <QFileInfo>
#include <QMutex>
#include <QString>
#include <QUrl>
SWITCH_WARNINGS_ON

#include <list>
#include <memory>

#include "metadata.h"
#include "obj.h"
#include "project_params.h"

#ifdef ZFPRJ_WINDOWS
#include "../zftools/undo.h"
#endif

class ZFProject;

#define PLAN_NAME "name"
#define PLAN_TYPE "type"
#define PLAN_GROUP L"AnnotationGroup"
#define PLAN_SCANPOS L"ScanPos"
#define PLAN_SCANITEM L"ScanItem"
#define PLAN_TASK L"Task"
#define PLAN_ITEM L"AnnotationItem"
#define PLAN_LABEL L"AnnotationLabel"
#define PLAN_RECT L"AnnotationRect"
#define PLAN_LINK L"AnnotationLink"
#define PLAN_FILE L"AnnotationFile"
#define PLAN_CIRCLE L"AnnotationEllipse"
#define PLAN_POLYLINE L"AnnotationPolyline"
#define PLAN_POLYLINE_SEGMENT L"AnnotationPolylineSegment"
#define PLAN_POLYGON L"AnnotationPolygon"
#define PLAN_POLYGON_SEGMENT L"AnnotationPolygonSegment"

#define PLAN_LIST "AnnotationList"
#define PLAN_ITEMREF L"AnnotationItemRef"
#define PLAN_MESSAGE L"AnnotationMessage"

#define ITEM_UUID "UUID"
#define ITEM_UUID_REF "UUID_REF"
#define ITEM_NAME "name"
#define ITEM_SCAN "scan"

#define ITEM_ICON_TYPE "icontype"
#define ITEM_ICON "icon"

#define ITEM_SCANNED_BY "scannedBy"
#define ITEM_STATUS "status"
#define ITEM_TEXT "text"
#define ITEM_ID "id"
#define ITEM_LABEL "label"

#define ITEM_COLOR "color"
#define ITEM_BACKGROUND_COLOR "backgroundcolor"

#define ITEM_OPERATOR "operator"
#define ITEM_READ "read"
#define ITEM_DATE "date"

#define ITEM_TASK_STATUS "taskStatus"

#define ITEM_TASK_PLANNED L"planned"
#define ITEM_TASK_INPROGRESS L"inprogress"
#define ITEM_TASK_FINISHED L"finished"

#define ITEM_MATRIX "matrix"

#define ITEM_RESOLUTION "resolution"
#define ITEM_QUALITY "quality"
#define ITEM_COLOR_IMAGES "color_images"
#define ITEM_THERMAL_IMAGES "thermal_images"
#define ITEM_TARGETS "targets"
#define ITEM_COMPENSATOR "compensator"
#define ITEM_USE_MATRIX "matrix"

namespace zfprj {
class ZFProjectPlan;
class AnnotationItem;
class AnnotationItemComposite;

/**
 * Type for a list of plan item pointers.
 */
typedef std::list<std::shared_ptr<AnnotationItem>> plan_list_t;
typedef std::map<std::string, std::wstring> plan_attributes_t;

//#####################################################################################

#ifdef SUPPORT_PRJ_UNDO

#define UNDO_ADD_PLANNED_ITEM L"UndoAddPlannedItem"
#define UNDO_CHANGE_PLANNED_ITEM L"UndoChangedPlannedItem"
#define UNDO_MOVE_POLYLINE_ITEM L"UndoMovePolyLineItem"

/**
 * @brief Undo for add position.
 */
class ZFSPRJ_API UndoAddPlannedItem : public zftools::BaseUndoItem {
  std::wstring UUID;

public:
  UndoAddPlannedItem() : zftools::BaseUndoItem() {}
  UndoAddPlannedItem(std::wstring _UUID)
      : zftools::BaseUndoItem(), UUID(_UUID) {}
  virtual ~UndoAddPlannedItem() {}

  virtual std::wstring getId() override { return UNDO_ADD_PLANNED_ITEM; }

  virtual std::wstring getMessage() override {
    return QObject::tr("Undo add plan item").toStdWString();
  }

  virtual void
  getUndoAttributes(zftools::undo_attributes_t &attributes) override;
  virtual void
  setUndoAttributes(zftools::undo_attributes_t &attributes) override;
  virtual void undoItem() override;

  virtual void afterUndo(std::list<int> &actions) override;

  static std::shared_ptr<UndoItem> Create() {
    return std::make_shared<UndoAddPlannedItem>();
  }
};

/**
 * @brief Undo for any attribut changes of a planned item.
 */
class ZFSPRJ_API UndoChangedPlannedItem : public zftools::BaseUndoItem {
  plan_attributes_t itemAttributes;

public:
  UndoChangedPlannedItem() : zftools::BaseUndoItem() {}
  UndoChangedPlannedItem(plan_attributes_t _attributes)
      : zftools::BaseUndoItem(), itemAttributes(_attributes) {}
  virtual ~UndoChangedPlannedItem() {}

  virtual std::wstring getId() override { return UNDO_CHANGE_PLANNED_ITEM; }

  virtual std::wstring getMessage() override {
    return QObject::tr("Undo change item").toStdWString();
  }

  virtual void
  getUndoAttributes(zftools::undo_attributes_t &attributes) override;
  virtual void
  setUndoAttributes(zftools::undo_attributes_t &attributes) override;
  virtual void undoItem() override;

  virtual void afterUndo(std::list<int> &actions) override;

  static std::shared_ptr<UndoItem> Create() {
    return std::make_shared<UndoChangedPlannedItem>();
  }
};

/**
 * @brief Undo for polygon movement
 */
class ZFSPRJ_API UndoMovePolyLineItem : public zftools::BaseUndoItem {
  plan_attributes_t itemAttributes;

public:
  UndoMovePolyLineItem() : zftools::BaseUndoItem() {}
  UndoMovePolyLineItem(plan_attributes_t _attributes)
      : zftools::BaseUndoItem(), itemAttributes(_attributes) {}
  virtual ~UndoMovePolyLineItem() {}

  virtual std::wstring getId() override { return UNDO_MOVE_POLYLINE_ITEM; }

  virtual std::wstring getMessage() override {
    return QObject::tr("Undo move polyline").toStdWString();
  }

  virtual void
  getUndoAttributes(zftools::undo_attributes_t &attributes) override;
  virtual void
  setUndoAttributes(zftools::undo_attributes_t &attributes) override;
  virtual void undoItem() override;

  virtual void afterUndo(std::list<int> &actions) override;

  static std::shared_ptr<UndoItem> Create() {
    return std::make_shared<UndoMovePolyLineItem>();
  }
};
#endif

//#####################################################################################

class ZFSPRJ_API EstimatedTime {

public:
  int setupTime;
  int imageTime;
  int scanTime;

  EstimatedTime() : setupTime(0), imageTime(0), scanTime(0) {}

  static QString getTimeString(int timeSec);

  int getOverallTime();

  QString getTimeString();
};

//#####################################################################################

class ZFSPRJ_API LinkFileHelper {

  std::vector<std::string> audioType;
  std::vector<std::string> imageType;
  std::vector<std::string> scanType;

public:
  LinkFileHelper();

  bool isScanFileLink(const std::wstring &_filename);

  bool isImageFileLink(const std::wstring &_filename);

  bool isAudioFileLink(const std::wstring &_filename);

  bool isWebLink(const std::wstring &_filename);

  QPixmap getPixmap(std::shared_ptr<zfprj::AnnotationItem> item,
      int size = 24);
};

//#####################################################################################

/**
 * @brief Class to define a plan item for a project.
 */
class ZFSPRJ_API AnnotationItem : public ZFPrjObj {
  DEFINE_ZFBASE(ZFPrjObj)

  plan_attributes_t attributes;

public:

  enum AnnotationItemStatus {
    ITEM_PLANED = 0,  // planned but not scanned
    ITEM_SCANNED = 1, // scanned
    ITEM_DELETED = 2, // deleted
    ITEM_COVERED = 3  // covered by another scan
  };

  std::wstring getStatusText(AnnotationItemStatus status) const;

  AnnotationItemStatus getItemStatus(std::wstring status) const {
    if (status.compare(L"0") == 0) {
      return ITEM_PLANED;
    } else if (status.compare(L"1") == 0) {
      return ITEM_SCANNED;
    } else if (status.compare(L"2") == 0) {
      return ITEM_DELETED;
    } else if (status.compare(L"3") == 0) {
      return ITEM_COVERED;
    }

    return ITEM_PLANED;
  }

  void openLink();
  QString getLinkFilename();

  void updatePosition(double x, double y, double z);

  void updatePositionMatrix(double x, double y, double z) {
    const auto& matrix = attributes[ITEM_MATRIX];
    auto tokens = QString::fromStdWString(matrix).split(" ");
    double m[16];

    for (int j = 0; j < 16; ++j) {
      m[j] = 0;
    }

    int i = 0;

    if (tokens.size() == 16) {
      foreach (const auto& value, tokens) {
        m[i] = value.toDouble();
        ++i;
      }
    } else {
      // reset rotation and scale
      m[0] = 1.0;
      m[5] = 1.0;
      m[10] = 1.0;
      m[15] = 1.0;
    }

    m[3] = x;
    m[7] = y;
    m[11] = z;

    std::wstring matTxt =
        QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16")
            .arg(m[0], 0, 'f', 6)
            .arg(m[1], 0, 'f', 6)
            .arg(m[2], 0, 'f', 6)
            .arg(m[3], 0, 'f', 6)
            .arg(m[4], 0, 'f', 6)
            .arg(m[5], 0, 'f', 6)
            .arg(m[6], 0, 'f', 6)
            .arg(m[7], 0, 'f', 6)
            .arg(m[8], 0, 'f', 6)
            .arg(m[9], 0, 'f', 6)
            .arg(m[10], 0, 'f', 6)
            .arg(m[11], 0, 'f', 6)
            .arg(m[12], 0, 'f', 6)
            .arg(m[13], 0, 'f', 6)
            .arg(m[14], 0, 'f', 6)
            .arg(m[15], 0, 'f', 6)
            .toStdWString();

    attributes[ITEM_MATRIX] = matTxt;
  }

  virtual bool getPositionFromAttributes(double &x, double &y, double &z) {
    if (attributes.find("x") != attributes.end() &&
        attributes.find("y") != attributes.end() &&
        attributes.find("z") != attributes.end()) {
      x = QString::fromStdWString(attributes["x"]).toDouble();
      y = QString::fromStdWString(attributes["y"]).toDouble();
      z = QString::fromStdWString(attributes["z"]).toDouble();
      return true;
    } else {
      return false;
    }
  }

  bool getPositionFromMatrix(double &x, double &y, double &z) {
    if (attributes.find(ITEM_MATRIX) != attributes.end()) {
      const auto& matrix = attributes[ITEM_MATRIX];

      auto tokens = QString::fromStdWString(matrix).split(" ");
      double m[16]{ 0 };

      int i = 0;
      foreach (const auto& value, tokens) {
        m[i] = value.toDouble();
        ++i;
      }

      x = m[3];
      y = m[7];
      z = m[11];
      return true;
    } else {
      return false;
    }
  }

public:
  AnnotationItem(ZFProjectPlan *plan,
                 const std::wstring& _itemType,
                 const std::wstring& _itemName);

  AnnotationItem(ZFProjectPlan *plan);
  virtual ~AnnotationItem();

  virtual std::wstring getId() const { return PLAN_ITEM; }

  std::string Xml() const override { return PRJXML_PLANITEM; }

  /**
   * @brief After loading an AnnotationItem from file, this method is called to
   * set the stored attributes.
   *
   * @param attributes the plan_attributes_t reference containing the
   * attributes.
   */
  virtual void setAttributes(plan_attributes_t &attributes,
                             bool overrideUUID = false);

  /**
   * @brief Get a string defining the plan action which can be stored to the
   * project file. It has to contain all information to recreate the plan item.
   */
  virtual void getAttributes(plan_attributes_t &attributes) const;
};

//=============================================================================================

/**
 * @brief Plan for this project. This plan is loaded from and
 *        written to the underlying XML document.
 *
 */
class ZFSPRJ_API ZFProjectPlan : public ZFPrjObjNode {
  DEFINE_ZFBASE(ZFPrjObjNode)

private:
  /**
   * @brief Mutex to protect the project plan
   */
  QMutex planMutex;

  /**
   * @brief Comment for the project
   */
  QString comment;

  /**
   * @brief Operator of the project
   */
  QString operatorName2;

  /**
   * @brief Site for the project
   */
  QString siteName;

  /**
   * @brief Company for the project
   */
  QString companyName;

  zfprj::ZFMetadataList metadataList;

  /**
   * @brief Template definition for this project
   */
  QString templateName;

public:
  ZFProjectPlan(ZFProject *p)
      : ZFPrjObjNode(p), templateName("basic_template.xml") {
#ifdef SUPPORT_PRJ_UNDO
    zftools::UndoItemFactory::getInstance()->registerUndoItemCreator(
        UNDO_ADD_PLANNED_ITEM, &UndoAddPlannedItem::Create);
    zftools::UndoItemFactory::getInstance()->registerUndoItemCreator(
        UNDO_CHANGE_PLANNED_ITEM, &UndoChangedPlannedItem::Create);
    zftools::UndoItemFactory::getInstance()->registerUndoItemCreator(
        UNDO_MOVE_POLYLINE_ITEM, &UndoMovePolyLineItem::Create);
#endif
  }

  virtual ~ZFProjectPlan();

  enum class SEARCH_RESULT { ALL, FIRST };

  plan_list_t itemList;
  ProjectParams planningParameter;

  /**
   * @brief Load the plan from the XML file.
   *
   * @return true, if plan where loaded, false otherwise
   */
  virtual bool Load() override;

  virtual void Free() override {
    itemList.clear();
    objs.clear();
  }

  /**
   * @brief Stores the plan to the XML file. These is not saving the XML file!
   *        This has to be done manually.
   *
   * @return true, if plan was stored, false otherwise
   */
  virtual bool Store(double versNr) override;

  QString Typ() const override { return "Plan"; }
  std::string Xml() const override { return PRJXML_PLAN; }

  QString getTemplateName() { return templateName; }

  void setTemplateName(const QString &_template) {
    if (!_template.isEmpty()) {
      if (metadataList.createFromTemplate(_template)) {
        templateName = _template;
      }
    }
  }

  zfprj::ZFMetadataList &getMetadataList() { return metadataList; }

  void setCompanyName(const QString &_company) { companyName = _company; }

  QString getCompanyName() const { return companyName; }

  void setSiteName(const QString &_site) { siteName = _site; }

  QString getSiteName() const { return siteName; }

  void setComment(const QString &_comment) { comment = _comment; }

  QString getComment() const { return comment; }

  void setOperatorName(const QString &_operatorName) {
    operatorName2 = _operatorName;
  }

  QString getOperatorName() const { return operatorName2; }

  /*virtual zfprj::ZFPrjObj* AddObj(zfprj::ZFPrjObj* obj) override {
      auto newItem = std::make_shared<AnnotationItem>(this);
      addItem(newItem);
      return obj;
  }*/

  /**
   * @brief Add the given item to the plan.
   *
   * @param item the item to be added
   */
  void addItem(std::shared_ptr<AnnotationItem> item);

  /**
   * @brief Delete the item with the given UUID.
   *
   * @param uuid UUID of the item to be deleted
   */
  void deleteItem(const std::wstring &uuid);

  /**
   * @brief Add a reference entry from from UUID to toUUID.
   *
   * @param fromUUID start reference
   * @param toUUID   stop reference
   *
   * @return true, if the reference was created successfully
   */
  bool addItemRef(const std::wstring &fromUUID, const std::wstring &toUUID);
  bool addItemRef(std::shared_ptr<AnnotationItemComposite> parentItem,
                  const std::wstring &toUUID);

  /**
   * @brief Delete the references to toUUID. If fromUUID is given, only
   * references from this item are deleted.
   *
   * @param fromUUID start reference
   * @param toUUID   stop reference
   */
  void deleteItemRef(const std::wstring &fromUUID, const std::wstring &toUUID);

  /**
   * @brief Get the parent item of the item given with UUID.
   *
   * @param uuid  the child UUID
   *
   * @return the parent item or nullptr if the item has no parent
   */
  std::shared_ptr<AnnotationItemComposite>
  getAnnotationItemParent(const std::wstring &uuid) const;

  /**
   * @brief Lookup the item for the given UUID recursivly
   *
   * @param uuid the UUID to lookup
   *
   * @return  the item with the UUID or nullptr
   */
  std::shared_ptr<AnnotationItem>
  getAnnotationItem(const std::wstring &uuid) const;

  /**
   * @brief Lookup the item for the given UUID recursivly
   *
   * @param items the items list to be searched
   * @param uuid the UUID to lookup
   *
   * @return  the item with the UUID or nullptr
   */
  std::shared_ptr<AnnotationItem>
  getAnnotationItem(const plan_list_t &items, const std::wstring &uuid) const;

  /**
   * @brief Lookup all items having the given attributes.
   *
   * @param foundItems    the resulting list (OUT)
   * @param attributes    the attributes to be matched
   */
  void getAnnotationItemsByAttributes(
      plan_list_t &foundItems, plan_attributes_t attributes,
      SEARCH_RESULT result = SEARCH_RESULT::ALL) const;

  /**
   * @brief Lookup all items having the given attributes.
   *
   * @param items         the items to be searched
   * @param foundItems    the resulting list (OUT)
   * @param attributes    the attributes to be matched
   */
  void getAnnotationItemsByAttributes(
      const plan_list_t &items, plan_list_t &foundItems,
      plan_attributes_t attributes,
      SEARCH_RESULT result = SEARCH_RESULT::ALL) const;

  /**
   * @brief Lookup all items at the given position.
   *
   * @param foundItems    the resulting list (OUT)
   * @param attributes    the attributes to be matched
   * @param x             the x-Pos stored in "x"
   * @param y             the y-Pos stored in "y"
   * @param z             the z-Pos stored in "z"
   * @param x             the allowed delty
   */
  void getAnnotationItemsByPosition(plan_list_t &foundItems,
                                    plan_attributes_t attributes, double x,
                                    double y, double z, double delta) const;

  /**
   * @brief Change the attributes of the item given with UUID.
   *        Only the gibven attributes are changed. The rest remains unchanged.
   *
   * @param uuid          UUID of the item
   * @param newAttributes the new attribute set
   */
  void setItemAttributes(const std::wstring &uuid,
                         const plan_attributes_t newAttributes);

  /**
   * @brief Helper method to calculate the estimated time of an item
   *        based on the given attributes.
   */
  zfprj::EstimatedTime getScanTime(zfprj::plan_attributes_t &attributes);
};

//#####################################################################################

/**
 * @brief Class for composite plan items. This is used to group plan items.
 */
class ZFSPRJ_API AnnotationItemComposite : public AnnotationItem {
  std::wstring name;

public:
  plan_list_t planChildItems;

  AnnotationItemComposite();
  AnnotationItemComposite(ZFProjectPlan *plan);
  ~AnnotationItemComposite() override;

  std::wstring getId() const override { return PLAN_GROUP; }

  /**
   * @brief Adds an AnnotationItem as a child of this AnnotationItem.
   */
  virtual void addAnnotationItem(std::shared_ptr<AnnotationItem> childItem);

  bool getPositionFromAttributes(double &x, double &y, double &z) override;

  static std::shared_ptr<AnnotationItem> Create();

  std::shared_ptr<AnnotationItem> getFirstChildOfType(std::wstring type) const;
};

//=============================================================================================

/**
 * @brief Factory to generate AnnotationItem instances.
 *
 */
class ZFSPRJ_API AnnotationItemFactory {

private:
  AnnotationItemFactory();
  ~AnnotationItemFactory();

public:
  /**
   * @brief Access-Method to the AnnotationItemFactory instance.
   * @return the instance
   */
  static AnnotationItemFactory *getInstance() {
    static AnnotationItemFactory instance;
    return &instance;
  }

  std::shared_ptr<AnnotationItem>
  createAnnotationItem(ZFProjectPlan *plan, const std::wstring &_type);

  std::shared_ptr<AnnotationItem> createLinkItem(zfprj::ZFProjectPlan *plan);
  std::shared_ptr<AnnotationItem> createLabelItem(zfprj::ZFProjectPlan *plan);
  std::shared_ptr<AnnotationItem> createRectItem(zfprj::ZFProjectPlan *plan);
  std::shared_ptr<AnnotationItem> createEllipseItem(zfprj::ZFProjectPlan *plan);
  std::shared_ptr<AnnotationItem>
  createPolylineItem(zfprj::ZFProjectPlan *plan);
  std::shared_ptr<AnnotationItem> createMessageItem(zfprj::ZFProjectPlan *plan);

  std::shared_ptr<AnnotationItem> createScanItem(zfprj::ZFProjectPlan *plan);

  std::shared_ptr<AnnotationItem> createRefItem(zfprj::ZFProjectPlan *plan);

  std::shared_ptr<AnnotationItemComposite>
  createTaskItemComposite(zfprj::ZFProjectPlan *plan);
  std::shared_ptr<AnnotationItemComposite>
  createPolylineItemComposite(zfprj::ZFProjectPlan *plan);
  std::shared_ptr<AnnotationItemComposite>
  createPolygonItemComposite(zfprj::ZFProjectPlan *plan);
  std::shared_ptr<AnnotationItemComposite>
  createScanItemComposite(zfprj::ZFProjectPlan *plan,
                          const std::wstring &_type);

  std::shared_ptr<AnnotationItem>
  createAnnotationItemFromNode(ZFProjectPlan *plan,
                               tinyxml2::XMLElement *planItemElement);

  tinyxml2::XMLNode *createPlanNode(tinyxml2::XMLDocument *doc,
                                    std::shared_ptr<AnnotationItem> planItem);
};
} // namespace zfprj
