#pragma once

#include "ZfsPrjBasic.h"

#include "config.h"

#ifdef SUPPORT_PRJ_UNDO
#include "../zftools/undo.h"
#endif

#include <list>
#include <memory>
#include <string>

#include "../Common/def.h"
SWITCH_WARNINGS_OFF
#include <QObject>
#include <QPointer>
SWITCH_WARNINGS_ON

class ZFProject;
class BackgroundImageParameter;

namespace zfprj {

	class Group;
	class GroupItem;
	class GroupList;

	//#####################################################################################

#ifdef SUPPORT_PRJ_UNDO

#define UNDO_ADD_GROUP L"UndoAddGroup"
#define UNDO_DELETE_GROUP L"UndoDeleteGroup"
#define UNDO_ADD_VP_TO_GROUP L"UndoAddViewpointToGroup"
#define UNDO_DELETE_VP_FROM_GROUP L"UndoDeleteViewpointFromGroup"
#define UNDO_RENAME_GROUP L"UndoRenameGroup"
#define UNDO_RENAME_FOLDER L"UndoRenameFolder"

/**
 * @brief Undo for add group.
 */
	class UndoAddGroup : public zftools::BaseUndoItem {
		std::wstring group;

	public:
		UndoAddGroup() : zftools::BaseUndoItem() {}
		UndoAddGroup(std::wstring group) : zftools::BaseUndoItem(), group(group) {}
		virtual ~UndoAddGroup() {}

		virtual std::wstring getId() override { return UNDO_ADD_GROUP; }
		virtual std::wstring getMessage() override {
			return QObject::tr("Undo add group %1")
				.arg(QString::fromStdWString(group))
				.toStdWString();
		}
		virtual void
			getUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void
			setUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void undoItem() override;

		static std::shared_ptr<UndoItem> Create() {
			return std::make_shared<UndoAddGroup>();
		}
	};

	//#####################################################################################

	/**
	 * @brief Undo for delete group.
	 */
	class UndoDeleteGroup : public zftools::BaseUndoItem {
		std::wstring group;
		std::string matrix;
		bool groupVisible;

	public:
		UndoDeleteGroup() : zftools::BaseUndoItem() {}
		UndoDeleteGroup(std::wstring group, std::string matrix, bool visible)
			: zftools::BaseUndoItem(), group(group), matrix(matrix),
			groupVisible(visible) {}
		virtual ~UndoDeleteGroup() {};

		virtual std::wstring getId() override { return UNDO_DELETE_GROUP; }
		virtual std::wstring getMessage() override {
			return QObject::tr("Undo delete group %1")
				.arg(QString::fromStdWString(group))
				.toStdWString();
		}
		virtual void
			getUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void
			setUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void undoItem() override;

		static std::shared_ptr<UndoItem> Create() {
			return std::make_shared<UndoDeleteGroup>();
		}
	};

	//#####################################################################################
	/**
	 * @brief Undo for add viewpoint to group.
	 */
	class UndoAddViewpointToGroup : public zftools::BaseUndoItem {
		std::wstring group;
		std::wstring viewpoint;

	public:
		UndoAddViewpointToGroup() : zftools::BaseUndoItem() {}
		UndoAddViewpointToGroup(std::wstring group, std::wstring viewpoint)
			: zftools::BaseUndoItem(), group(group), viewpoint(viewpoint) {}
		virtual ~UndoAddViewpointToGroup() {}

		virtual std::wstring getId() override { return UNDO_ADD_VP_TO_GROUP; }
		virtual std::wstring getMessage() override {
			return QObject::tr("Undo add scan position to group %1")
				.arg(QString::fromStdWString(group))
				.toStdWString();
		}
		virtual void
			getUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void
			setUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void undoItem() override;

		static std::shared_ptr<UndoItem> Create() {
			return std::make_shared<UndoAddViewpointToGroup>();
		}

		void afterUndo(std::list<int>& actions);
	};

	//#####################################################################################

	/**
	 * @brief Undo for delete viewpoint from group.
	 */
	class UndoDeleteViewpointFromGroup : public zftools::BaseUndoItem {
		std::wstring group;
		std::wstring viewpoint;

	public:
		UndoDeleteViewpointFromGroup() : zftools::BaseUndoItem() {}
		UndoDeleteViewpointFromGroup(std::wstring group, std::wstring viewpoint)
			: zftools::BaseUndoItem(), group(group), viewpoint(viewpoint) {}
		virtual ~UndoDeleteViewpointFromGroup() {};

		virtual std::wstring getId() override { return UNDO_DELETE_VP_FROM_GROUP; }
		virtual std::wstring getMessage() override {
			return QObject::tr("Undo remove scan position from group %1")
				.arg(QString::fromStdWString(group))
				.toStdWString();
		}
		virtual void
			getUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void
			setUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void undoItem() override;

		static std::shared_ptr<UndoItem> Create() {
			return std::make_shared<UndoDeleteViewpointFromGroup>();
		}
	};

	//#####################################################################################

	/**
	 * @brief Undo group renaming.
	 */
	class UndoRenameGroup : public zftools::BaseUndoItem {
		std::wstring oldName;
		std::wstring newName;

	public:
		UndoRenameGroup() : zftools::BaseUndoItem() {}
		UndoRenameGroup(std::wstring oldName, std::wstring newName)
			: zftools::BaseUndoItem(), oldName(oldName), newName(newName) {}
		virtual ~UndoRenameGroup() {};

		virtual std::wstring getId() override { return UNDO_RENAME_GROUP; }
		virtual std::wstring getMessage() override {
			return QObject::tr("Undo rename group from %1 to %2")
				.arg(QString::fromStdWString(oldName))
				.arg(QString::fromStdWString(newName))
				.toStdWString();
		}
		virtual void
			getUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void
			setUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void undoItem() override;

		static std::shared_ptr<UndoItem> Create() {
			return std::make_shared<UndoRenameGroup>();
		}
	};

	//#####################################################################################

	/**
	* @brief Undo group renaming.
	*/
	class UndoRenameFolder : public zftools::BaseUndoItem {
		std::wstring oldName;
		std::wstring newName;

	public:
		UndoRenameFolder() : zftools::BaseUndoItem() {}
		UndoRenameFolder(std::wstring oldName, std::wstring newName)
			: zftools::BaseUndoItem(), oldName(oldName), newName(newName) {}
		virtual ~UndoRenameFolder() {};

		virtual std::wstring getId() override { return UNDO_RENAME_FOLDER; }
		virtual std::wstring getMessage() override {
			return QObject::tr("Undo set group folder from %1 to %2")
				.arg(QString::fromStdWString(oldName))
				.arg(QString::fromStdWString(newName))
				.toStdWString();
		}
		virtual void
			getUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void
			setUndoAttributes(zftools::undo_attributes_t& attributes) override;
		virtual void undoItem() override;

		static std::shared_ptr<UndoItem> Create() {
			return std::make_shared<UndoRenameFolder>();
		}
	};

#endif

	class GroupList;
	class Group;

	//#####################################################################################
	/**
	 * @brief Base class for all items of groups.
	 */

	class ZFSPRJ_API GroupItem : public QObject {
		Q_OBJECT

			friend class GroupList;
	protected:
		bool visible = true; // true => Das Item gehoert zu einer Gruppe die angezeigt
						// wird. //ToDO Die Variable wird im Planview gesetzt eventull
						// as setzten in der Group Klasse machen.

#pragma warning(push)
#pragma warning(disable : 4251)       // DLL Schnittstelle
		std::list<QPointer<const Group>> groupsList; // List of all groups of this item
#pragma warning(pop)
		virtual void setVisible(bool _visible, bool);
	public:

#pragma warning(push)
#pragma warning(disable : 4251)       // DLL Schnittstelle
		std::wstring id;                    // Name of the item
#pragma warning(pop)

		GroupItem(std::wstring name) : QObject() {
			id = name;
			visible = true;
		}
		virtual ~GroupItem() {}


		virtual bool isVisible() const = 0;

		virtual int nrVisible() const = 0;


		/**
		*	If one group from the gropusList ist visible the item is visible.
		* */
		virtual void checkVisbility();

		void addGroup( const Group* group, bool update);
		void removeGroup( const Group* group);
		const std::list<QPointer<const Group>>& getGroupsList() const { return groupsList; }

	};

	typedef std::list<GroupItem*> group_item_list_t;

	/**
	 * @brief Group class.
	 */
	class ZFSPRJ_API Group : public QObject {

		friend class GroupList;
#pragma warning(push)
#pragma warning(disable : 4251) // DLL Schnittstelle
		std::wstring id;              // Name of the group
		std::wstring folder;          // Grouping folder
		std::string matrix;           // Matrix-String

		std::shared_ptr<BackgroundImageParameter> params;
#pragma warning(pop)

		double matrixArray[16]; // Matrix as Array
		const GroupList* grps;  // Parent-GroupList
		bool deleted;

	public:
#pragma warning(push)
#pragma warning(disable : 4251) // DLL Schnittstelle
		group_item_list_t groupItems; // List of all GroupItems in the group
#pragma warning(pop)

	protected:
		bool visible = true; // Flag, if the group items (i.e. VPs) should be visible

	public:
		Group(const GroupList*, std::wstring name, std::string matrix = "",
			bool visible = true);
		virtual ~Group();
		/**
		 * @brief Set the group visibility.
		 *
		 * @param isVisible Flag if the group should be displayed or not
		 */
		void setVisible(bool isVisible, bool cb);

		/**
		 * @brief Add the GroupItem to the group.
		 *
		 * @param item das Gruppen-Item das hinzugefuegt werden soll
		 * \param useUndo Aktion in die Undoliste eintragen.
		 * \param updateItems Visibiliti aller Items aktualisieren. Z.B. Scans im Viewpoint.
		 * @return true if the item was added, false otherwise
		 */
		bool addItem(GroupItem* item, bool useUndo, bool upadteItems);

		/**
		 * @brief Delete GroupItem with the given name from the group.
		 *
		 * @param itemName Name of the item to be deleted
		 */
		void deleteItem(std::wstring itemName, bool useUndo = true);

		/**
		 * Delete the given GroupItem from the group.
		 *
		 * \param item => Name of the item to be deleted
		 * \param useUndo => Use the undo function.
		 */
		void deleteItem(GroupItem* item, bool useUndo = true);

		/**
		 * @brief Delete all GroupItems from the group.
		 */
		void clear();

		/**
		 * @brief The the name of the group.
		 *
		 * @param name the new name of the group
		 */
		void setName(const std::wstring& name);

		/**
		 * @brief Get the name of the group.
		 *
		 * @return the name of the group
		 */
		std::wstring getName() const { return id; }

		/**
		* @brief The the folder to which the group belongs.
		*
		* @param name the new folder
		*/
		void setFolder(const std::wstring& name);

		/**
		* @brief Get the folder name of the group.
		*
		* @return the name of the folder the group belongs to
		*/
		std::wstring getFolder() const { return folder; }

		/**
		 * @brief Get the group matrix.
		 *
		 * @return the matrix of the group
		 */
		std::string getMatrix() { return matrix; }

		/**
		 * @brief Get the group visibility.
		 *
		 * @return the group visibility
		 */
		bool isVisible() const;


		/**
		* Number of visible group items. It's posible that a group item contains more
		* than one Items.
		*/
		int nrVisible() const;

		/**
		 * @brief Set if group is deleted
		 *
		 * @param isDeleted Flag if the group should be marked as deleted
		 */
		void setDeleted(bool isDeleted) { deleted = isDeleted; }

		/**
		 * @brief Get the group deleted state.
		 *
		 * @return true, if the group is marked as deleted
		 */
		bool isDeleted() { return deleted; }

		/**
		 *   Check if the "item" is a element of the group
		 *   \param item => Item to check
		 *   \return True if the group contains the item
		 */
		bool containsItem(const GroupItem* item);

		void
			setBackgroundParameter(std::shared_ptr<BackgroundImageParameter> _params) {
			params = _params;
		}

		std::shared_ptr<BackgroundImageParameter>& getBackgroundParameter() {
			return params;
		}
	};

	//#####################################################################################
	typedef std::list<std::shared_ptr<Group>> group_list_t;

	/**
	 * @brief Group list class.
	 */
	class ZFSPRJ_API GroupList {
		ZFProject* project;

	protected:
		/**
		 * @brief Flag to define if viewpoints are displayed or not if they are in no
		 * group
		 */
		bool masterVisibility;
#pragma warning(push)
#pragma warning(disable : 4251) // DLL Schnittstelle
		group_list_t::iterator findIterator(std::wstring& grp, bool allItems = false);

		group_list_t groupList; // list of groups
		std::wstring activeGroup;

		// Temporary save current setting
		bool tempMasterVisibility;
		std::map<std::wstring, bool> mGroupStatusMap;
		bool tempGroupSaved;

#pragma warning(pop)

	public:
		GroupList(ZFProject*);
		virtual ~GroupList();

#pragma warning(push)
#pragma warning(disable : 4251) // DLL Schnittstelle
		void setActiveGroup(std::wstring newActiveGroup) {
			activeGroup = newActiveGroup;
		}

		const std::wstring& getActiveGroup() { return activeGroup; }

		const group_list_t getGroupList(bool allGroups = false) const {
			group_list_t groups;
			foreach(auto group, groupList) {
				if (allGroups || !group->isDeleted()) {
					groups.push_back(group);
				}
			}

			return groups;
		}
#pragma warning(pop)

		/**
		*	Set visibility off all groups to vis.
		*	\param vis => true = visible
		*/
		void setAllGroupsVisible(bool vis);


		bool getMasterVisiblity() { return masterVisibility; }


		/**
		 * @brief Add group to group list.
		 *
		 * @param group		name of the new group
	   * @param matrix	    Registration matrix of the group (optional)
	   * @param visible    Flag if the group is visible
	   *
	   * @return pointer to the newly created group
	   */
		std::shared_ptr<Group> addGroup(std::wstring, std::string matrix = "",
			bool visible = true);

		/**
		 * @brief Lookup group.
		 *
		 * @param grp	name of the group
		 *
		 * @return pointer to the group or NULL
		 */
		std::shared_ptr<Group> getGroup(std::wstring grp, bool allItems = false);

		/**
		 * @brief Delete existing group.
		 *
		 * @param grp	name of the group
		 *
		 * @return true if the group was deleted, false otherwise
		 */
		bool deleteGroup(std::wstring grp, bool undo = true);

		/**
		 * @brief Delete all groups.
		 */
		void clear();

		/**
		 *   Return a list of all groups that contain the passed item.
		 *   \param item => Item to check
		 *   \return List of groups.
		 */
		group_list_t findItemGroups(const GroupItem* item);

		/**
		 * @brief update or add group and update items in the list
		 *
		 * @param groupName  name of the group to change
		 * @param list       the list of viewpoints to be added to the group
		 * @param force      create group even if scan list is empty
		 */
		void updateList(std::wstring groupName, const std::list<std::wstring>& list, bool force = false);

		/**
		 * @brief Store groups to the project document.
		 */
		void storeToXML();

		/**
		 * @brief Load groups from project document.
		 * Existing groups are deleted before loading.
		 */
		void loadFromXML();

		/**
		 *   Save current Settings. Use tempResetCurrentSetting() to reset to the
		 * current settings.
		 */
		void tempSaveCurrentSetting();

		/**
		 *   Reset to saved settings. Update signal zfaction::ACT_GROUP_SHOW muss noch
		 * gesendet werden
		 */
		void tempResetCurrentSetting();
	};

	typedef std::shared_ptr<zfprj::GroupList> project_group_list_t;

} // namespace zfprj
