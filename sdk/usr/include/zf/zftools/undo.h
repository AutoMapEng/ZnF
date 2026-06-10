#pragma once
#include "zftools_api.h"
#include <zfcore/compiler.h>

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>



namespace tinyxml2 {
class XMLDocument;
}

namespace zftools {

class UndoContainer;
class UndoItem;

typedef std::list<std::shared_ptr<UndoItem>> undo_list_t;
// typedef std::list<UndoItem*> undo_list_t;
typedef std::map<std::string, std::wstring> undo_attributes_t;

//#####################################################################################

/**
 * @brief Interface class for all undo items.
 */
class ZFTOOLS_API UndoItem {

protected:
  std::wstring description; /// Undo description.

public:
  UndoItem() {}
  virtual ~UndoItem() {}

  /**
   * @brief Get the id of the undo item. This has to be unique for each unto
   * item type.
   */
  virtual std::wstring getId() = 0;

  /**
   * @brief Get the message to be displayed about this UndoItem
   */
  virtual std::wstring getMessage() { return description; }

  /**
   * @brief Set the message to be displayed about this UndoItem
   */
  virtual void setMessage(const std::wstring &message) {
    description = message;
  }

  /**
   * @brief After loading an UndoItem from file, this method is called to set
   * the stored attributes.
   *
   * @param attributes the undo_attributes_t reference containing the
   * attributes.
   */
  virtual void setUndoAttributes(undo_attributes_t &attributes) = 0;

  /**
   * @brief Get a string defining the undo action which can be stored to the
   * undo file. It has to contain all information to recreate the undo item.
   */
  virtual void getUndoAttributes(undo_attributes_t &attributes) = 0;

  /**
   * @brief Execute the undo. Implementations have to take care to undo all its
   * work.
   */
  virtual void undoItem() = 0;

  /**
   * @brief Executed after the undo is finished. This can be used to send
   * actions.
   */
  virtual void afterUndo(std::list<int> & /*actions*/){};
};

//#####################################################################################

/**
 * @brief Class for composite undo items. This is used to group unto items into
 * a single transaction.
 */
class ZFTOOLS_API UndoItemComposite : public UndoItem {
  std::wstring name;

public:
  undo_list_t undoChildItems; /// list of sub-items for this undo item

  UndoItemComposite() {}
  virtual ~UndoItemComposite();

  virtual std::wstring getId() override { return L"UndoGroup"; }

  virtual void setUndoAttributes(undo_attributes_t &attributes) override;
  virtual void getUndoAttributes(undo_attributes_t &attributes) override;

  virtual void undoItem() override;
  virtual void afterUndo(std::list<int> &actions) override;

  /**
   * @brief Adds an UndoItem as a child of this UndoItem. Undoing this item also
   * undos all child items.
   */
  virtual void addUndoItem(std::shared_ptr<UndoItem> childItem) {
    undoChildItems.push_back(childItem);
  }

  static std::shared_ptr<UndoItem> Create() {
    return std::make_shared<UndoItemComposite>();
  }
};

//#####################################################################################

/**
 * @brief Abstract class for all undo items.
 *
 * Every implementation which wants to support loading/saving must also provide
 * a static creator function.
 *
 * Example:
 *
 * <pre>
 * static std::shared_ptr<UndoItem> Create() { return
 * std::make_shared<YourUndoItem>(); }
 * </pre>
 */
class ZFTOOLS_API BaseUndoItem : public UndoItem {
public:
  BaseUndoItem() : UndoItem() {}
  virtual ~BaseUndoItem() {}

  virtual void setUndoAttributes(undo_attributes_t &) override = 0;
  virtual void getUndoAttributes(undo_attributes_t &attributes) override = 0;
  virtual void undoItem() override = 0;
  virtual std::wstring getMessage() override { return L"Undo item"; }

  // adapt this for your UndoItem:
  // static std::shared_ptr<UndoItem> Create() { return
  // std::make_shared<YourUndoItem>(); }
};

//#####################################################################################

/**
 * @brief Holds all UndoItem instances. Undos can be stored/loaded to XML
 */
class ZFTOOLS_API UndoContainer {
public:
  enum act_t {
    UNDO_UNDEFINE,
    UNDO_ADD,
    UNDO_EXECUTED,
    UNDO_RESET,
    UNDO_LOADED,
    UNDO_STORED
  };
  typedef bool (*cbAct_f)(act_t action);

private:
  std::wstring undofile; /// Filename where undos are loaded/stored
  undo_list_t undos{};     /// the current undos
  int maxUndo{20};           /// maximal undo item count
  cbAct_f cbAct{};         /// callback method to notify undo changes

  bool transactionOpen{}; /// Flag, if a transaction is open or not
  bool undoLocked{};      /// Flag, if undo management is locked or not

  std::list<int> actions; /// list of open callbacks to be executed

  std::shared_ptr<UndoItemComposite> undoTransaction{}; /// the current transaction composite if a transaction is
                       /// open
  bool dirty{};

protected:
  /**
   * @brief Parse the given string.
   *
   * @param s     the string to parse
   */
  void parse(const char *s);

public:
  UndoContainer();
  virtual ~UndoContainer();

  /**
   * @brief Install the callback method listening to undo changes.
   *
   * @param cb the callback method
   */
  void installCallback(cbAct_f cb);

  /**
   * @brief Set the maximal undo steps.
   *
   * @param max   the maximal undo steps
   */
  void setMaxUndo(int max);

  /**
   * @brief Load undo items from file.
   *
   * @param fn the filename
   *
   * @return true if the file was loaded, false otherwise
   */
  bool loadUndoItems(const std::wstring &fn);

  /**
   * @brief Store undo items to file.
   *
   * @param fn    the filename
   *
   * @return true if the file was saved, false otherwise
   */
  bool storeUndoItems(const std::wstring &fn);

  /**
   * @brief Check if an undo item is available.
   *
   * @return true if there is an undo item, false otherwise
   */
  bool canUndo() const { return undos.size() ? true : false; }

  /**
   * @brief Add an undo item to the container.
   */
  void addUndoItem(std::shared_ptr<UndoItem>);

  void deleteUndoItem(std::shared_ptr<UndoItem>);

  /**
   * @brief Starts a transaction. All undo items add in a transaction are undone
   * together.
   *
   * @param name the name of the transaction
   */
  void openTransaction(const std::wstring &name);

  /**
   * @brief Close an open transaction.
   */
  void closeTransaction();

  /**
   * @brief Check if an transaction is open
   */
  bool hasOpenTransaction() const { return transactionOpen; }

  /**
   * @brief Locks the undo container. This signals, that no items should be
   * added now to the container.
   *
   * This can be used to prevent the creation of undo items if a undo is
   * performed and the steps are using the same methods which would create new
   * und items.
   */
  void lockUndo() { undoLocked = true; }

  /**
   * @brief Unlock the undo container.
   */
  void unlockUndo() { undoLocked = false; }

  bool isLocked() const { return undoLocked; }

  /**
   * @brief Get the last UndoItem.
   *
   * @return the last UndoItem
   */
  std::shared_ptr<UndoItem> getUndoItem();

  /**
   * @brief Undos the last undo item.
   *
   * @return true if the undo was performed, false otherwise
   */
  bool undo();

  void lockMutex(bool lock);

  /**
   * @brief Reset the container. All undo items are deleted.
   */
  void reset();

  /**
   * @brief Get the list of open actions
   */
  std::list<int>& getActions() { return actions; }
  const std::list<int>& getActions() const { return actions; }
};

//#####################################################################################

typedef std::shared_ptr<UndoItem> (*CreateUndoItemFn)(void);

/**
 * @brief Factory to generate UndoItem instances.
 *
 * Every UndoItem which supports load/save must register this creator function.
 *
 * Example:
 *
 * <pre>
 * zftools::UndoItemFactory::getInstance()->registerUndoItemCreator("UndoCreatorName",
 * &UndoCreatorItem::Create);
 * </pre>
 */
class ZFTOOLS_API UndoItemFactory {
  typedef std::map<std::wstring, CreateUndoItemFn> UndoItemFactoryMap;

private:
  /**
   * @brief Map containing the registered UndoItemCreatorFn functions.
   */
  UndoItemFactoryMap undoItemCreators;

  UndoItemFactory();
  ~UndoItemFactory();

public:
  /**
   * @brief Access-Method to the UndoItemFactory instance.
   * @return the instance
   */
  static UndoItemFactory *getInstance() {
    static UndoItemFactory instance;
    return &instance;
  }

  /**
   * @brief Register a new CreateUndoItemFn function for UndoItems.
   */
  void registerUndoItemCreator(const std::wstring &name,
                               CreateUndoItemFn creator) {
    undoItemCreators[name] = creator;
  }

  /**
   * @brief Creates an UndoItem for the given name.
   *
   * @return A pointer the created UndoItem or NULL, if a creator with the given
   * name is unknown
   */
  std::shared_ptr<UndoItem> createUndoItem(std::wstring name);
};


//================================================================

// global undo container
ZFTOOLS_API extern UndoContainer undoContainer;

} // end namespace zftools

