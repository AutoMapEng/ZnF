#pragma once

/**
	base class for objects
*/

#include "ZfsPrjBasic.h"
#include "config.h"

#include <list>
#include <string>

#include <zfcore/filename.h>
#include "../zfs/zfbase.h"

#include "../Common/def.h"
SWITCH_WARNINGS_OFF
#include <QEvent>
#include <QMutex>
#include <QString>
#include <QObject>
SWITCH_WARNINGS_ON

class ZFProject;

namespace tinyxml2 {
	class XMLNode;
	class XMLElement;
} // namespace tinyxml2

class QPixmap;
class ZFTRMatrix;

namespace zfprj {
	class ZFPrjObjNode;
	class ZFPrjObjRoot;
	class Layer;

	/**
	 *   Basisklasse zur uebergabe von Objekten nach aussen. Es gibt zwei varianten
	 *   1. Lese zurgiff : Solange das Objekt exisitert werden alle schreib
	 * opperatironen verhindert.
	 *   2. Lese/schreib zugriff: Solange das Objekt existiert werden alle lese und
	 * schreib opperationen verhindert. Wird das Objekt kopiert wird das
	 * urspruengliche Objekt geloescht. Zur uebergabe der Objekte muss die Klasse
	 * fuer den jeweiligen Typ abgeleitet werden.
	 */
	class ZFSPRJ_API ZFPrjMutex {
	private:
		ZFProject* mProject = nullptr; // Zeiger auf das Projekt das das Objekt enthaelt
		bool mReadWrite = false;     // True => lese/schreib zugriff. False => schreib zugriff

	public:
		ZFPrjMutex(ZFProject* project, bool readwrite);
		virtual ~ZFPrjMutex();

		void acquire();
		void release();

	protected:
		virtual void clear();

	private:
		ZFPrjMutex& operator=(const ZFPrjMutex&) { return *this; }
		ZFPrjMutex(const ZFPrjMutex*) {}
	};

	//=========================================================================================================

	class ZFSPRJ_API ZFPrjMsgEvent : public QEvent {
	protected:
		static Type myType;

	public:
		QString msg;
		static Type myregisteredEventType() { return myType; }
		ZFPrjMsgEvent(const QString& txt) : QEvent(myregisteredEventType()) {
			msg = txt;
		}
	};

	//=========================================================================================================

	/**
	\brief The base class for "objects" that can be stored in the project.

	Objects can often be added by the user to add further information
	to the project. Examples are measurements, targets, labels etc...

	The objects can be stored in the projects xml-file.
	Each object belongs to a Layer which may determine its color and
	visibility.
	*/
	class ZFSPRJ_API ZFPrjObj : public ZFBase {
		DEFINE_ZFBASE(ZFBase)
			friend class ZFPrjObjRoot;

	public:
		ItemStatus  statuss = ItemStatus::ITEM_OK;      // Status of the scans

		bool isVisible; ///< If this is false, the Painter2D and Painter3D will not
						///< draw the object. The value will not be saved in the
						///< project and should only be used to temporarily hide a
						///< object.

	protected:
		ZFPrjObjNode* parent;
		Layer* layer; ///<  Layer in dem das object plaziert wurde
		zf::FileName mName;
		QPixmap* pix;
		bool mExport; ///< Flag ob das Objekt beim Export beruecksichtigt werden soll

		bool isSelected; ///< A flag that is true if the user selected the object. By
						 ///< clicking on the listview item or its graphical
						 ///< representation in one of the views.

	private:
		unsigned char defaultRed;
		unsigned char defaultGreen;
		unsigned char defaultBlue;

	public:
		ZFPrjObj(ZFPrjObjNode*);
		virtual ~ZFPrjObj();

		virtual std::string Xml() const = 0;
		virtual const QPixmap* GetPixmap() { return NULL; }

		const wchar_t* NameW() const;
		const char* getNameC();
		void SetName(const std::wstring& n);
		bool IsName(const std::wstring&)
			const;           ///< Compare names and return true if identically
		int HasName() const; ///< Return length of name

		/// return true if object is deleted
		bool isStatusDeleted() const { return statuss & ITEM_DELETED; }

		void setExport(bool bo) { mExport = bo; }
		bool getExport() const { return mExport; }

		ZFPrjObjNode* Parent() { return parent; }
		const ZFPrjObjNode* Parent() const { return parent; }
		void setParent(ZFPrjObjNode* parentNode);
		const Layer* GetLayer() const {
			return layer;
		} ///< Layer in dem das object plaziert wurde
		Layer* GetLayer() {
			return layer;
		} ///< Layer in dem das object plaziert wurde
		const Layer* GetLayerConst() const {
			return layer;
		} ///< Layer in dem das object plaziert wurde
		void SetLayerByName(const std::wstring&);

		bool GetLayerFromXML(tinyxml2::XMLElement*);
		void StoreLayerToXML(tinyxml2::XMLElement&);

		void setSelected(bool b);
		bool getSelected() const;

		unsigned char red() const;   ///< return layer red or default red.
		unsigned char green() const; ///< return layer green or default green
		unsigned char blue() const;  ///< return layer blue or default blue
		void setDefaultColor(
			unsigned char red, unsigned char green,
			unsigned char blue); ///< this sets the color of the object that it has
								 ///< when it does not belong to a layer
	};

	typedef std::list<ZFPrjObj*> objlst_t;

	//=========================================================================================================

	typedef std::list<ZFPrjObjNode*> objnodelst_t;
#pragma warning(push)
#pragma warning(disable : 4251) // DLL Schnittstelle
	/**
	\brief Holds a collection of items and/or subnodes as well as for the root.
	*/
	class ZFSPRJ_API ZFPrjObjNode : public ZFBase {
		DEFINE_ZFBASE(ZFBase)
			friend class ZFPrjObjRoot;

	public:
		objlst_t objs;         ///< objecte (like: measurement-points,...)
		objnodelst_t subnodes; ///< subnodes

	protected:
		ZFProject* parent;
		std::string name; ///< name of the node
		bool mExport; ///< Flag ob der Node beim Export beruecksichtigt werden soll
		QMutex* mutObjNode;

	public:
		ZFPrjObjNode(ZFProject* p);
		virtual ~ZFPrjObjNode();

		virtual void Free(); ///< delete all and free memory. Called by destructor

		virtual bool Load() = 0; ///< load prom projectfile
		virtual bool Store(double versNr) = 0; ///< store back \param versNr => Versionsummer in
									  ///< der das Projekt abgespeichert werden soll.
		virtual QString Typ() const = 0; ///< typname like "Planes", "Layers", ...
		virtual std::string Xml() const = 0; ///< xml sektion where to store

		//ZFProject* Parent() { return parent; }
		ZFProject* Project() { return parent; }
		const ZFProject* Project() const { return parent; }


		virtual ZFPrjObj*
			AddObj(ZFPrjObj* obj); /**< Add object to list (add with operator new). If
									  there is already an object with the same name in the
									  list, the name of obj will changed to changed to a
									  name that does not exist yet. */
		ZFPrjObj* AddObjectToActiveLayerAndStore(ZFPrjObj* obj);
		ZFPrjObj* AddObjectToLayerAndStore(ZFPrjObj* obj,
			const std::wstring& layerName);
		bool DelObj(ZFPrjObj*);
		bool DelObj(const std::wstring& name);
		ZFPrjObj*
			TakeObj(const std::wstring
				& name); ///< Removes the object from the list but does not delete
						///< it. Ownership of the object goes to the caller.
		ZFPrjObj* findObject(
			const std::wstring& name); ///< Find a object in the list. The onwnership
									   ///< stays with the ZFPrjObjNode.
		void DeleteLastObject();
		bool DellObjRec(ZFPrjObj*);
		bool DellObjRec(const std::wstring& name);
		ZFPrjObj* FindObjRec(const std::wstring& name);
		const ZFPrjObj* FindObjRecConst(const std::wstring& name) const;
		ZFPrjObjNode* AddNode(ZFPrjObjNode*);
		void DelNode(ZFPrjObjNode*);
		int Objs() const { return (int)objs.size(); }
		int Subnodes() const { return (int)subnodes.size(); }

		void setExport(bool bo) { mExport = bo; }
		bool getExport() { return mExport; }

		const char* GetName();
		void SetName(std::string&);
		void SetName(const char*);

		tinyxml2::XMLNode* AddXmlGroup();        ///< remove existing and create new
		tinyxml2::XMLNode* GetXmlProjectGroup(); ///< global project
		tinyxml2::XMLNode* GetXmlGroup();        ///< get sektion
		tinyxml2::XMLNode* GetXmlGroup(const char*); ///< get sektion
		double getLastVersion();

		static ZFTRMatrix readMatrix(const char* txt, bool& ok);
		static void writeMatrix(tinyxml2::XMLElement* elemen, const char* key,
			const ZFTRMatrix& mat);
		static void writeMatrixAttribute(tinyxml2::XMLElement* elemen,
			const char* key, const ZFTRMatrix& mat);

	protected:
		std::wstring makeNameUnique(
			const std::wstring
			& name); ///< Checks if one of the objects has the same name and
					///< returns another unique name if that is the case.
		void getHigherNameCounter(
			const std::wstring& defaultNamePrefix, const std::wstring& name,
			int& nameCounter); ///< This checks if name matches the default name
							   ///< pattern and if it does it reads the name counter
							   ///< and returns a counter that is higher than that.
		static std::wstring composeDefaultName(
			const std::wstring& defaultNamePrefix,
			int& nameCounter); ///< nameCounter will be incremented when calling this.
	};
#pragma warning(pop)
	//=========================================================================================================

	/**
	\brief Root object which holds all other objects
	*/
	class ZFSPRJ_API ZFPrjObjRoot : public ZFPrjObjNode {
		DEFINE_ZFBASE(ZFPrjObjNode)

	protected:
	public:
		ZFPrjObjRoot(ZFProject*);

		virtual bool Load() { return false; }
		virtual bool Store(double versNr);
		QString Typ() const override { return QObject::tr("Root"); }
		std::string Xml() const override { return ""; }
	};

	//=========================================================================================================

	class ZFSPRJ_API ZFPrjObjRootReadWrite : public ZFPrjMutex {
	public:
		ZFPrjObjRoot* refNode = nullptr;

	public:
		ZFPrjObjRootReadWrite(ZFPrjObjRoot* refNode_);
		~ZFPrjObjRootReadWrite();

		virtual void clear();

	private:
		ZFPrjObjRootReadWrite& operator=(const ZFPrjObjRootReadWrite&) {
			return *this;
		}
		ZFPrjObjRootReadWrite(const ZFPrjObjRootReadWrite&)
			: ZFPrjMutex(NULL, false) {}
	};

	//=========================================================================================================

	/**
	\brief Geschuetztes Objekt fuer ZFPrjObjNode Objekte.
	*   Erlaubt nur lesenden Zurgriff.
	*/
	class ZFSPRJ_API ZFPrjObjNodeRead : public ZFPrjMutex {
	private:
		bool lock;

	public:
		const ZFPrjObjNode* refNode = nullptr;

	public:
		ZFPrjObjNodeRead(ZFPrjObjNode* refNode_);
		~ZFPrjObjNodeRead();

		virtual void clear();

	private:
		ZFPrjObjNodeRead& operator=(const ZFPrjObjNodeRead&) { return *this; }
		ZFPrjObjNodeRead(const ZFPrjObjNodeRead&) : ZFPrjMutex(NULL, false) {}
	};

	//=========================================================================================================

	/**
	\brief Geschuetztes Objekt fuer ZFPrjObjNode Objekte.
	*   Erlaubt lese und schreib Zurgriff.
	*/
	class ZFSPRJ_API ZFPrjObjNodeReadWrite : public ZFPrjMutex {
	public:
		ZFPrjObjNode* refNode = nullptr;

	public:
		ZFPrjObjNodeReadWrite(ZFPrjObjNode* refNode_);
		~ZFPrjObjNodeReadWrite();

		virtual void clear();

	private:
		ZFPrjObjNodeReadWrite(const ZFPrjObjNodeReadWrite&)
			: ZFPrjMutex(NULL, false) {}
		ZFPrjObjNodeReadWrite& operator=(const ZFPrjObjNodeReadWrite&) {
			return *this;
		}
	};

	//=========================================================================================================

	class ZFSPRJ_API ZFPrjObjReadWrite : public ZFPrjMutex {
	public:
		ZFPrjObj* refNode = nullptr;

	public:
		ZFPrjObjReadWrite(ZFPrjObj* refNode_);
		~ZFPrjObjReadWrite();

		virtual void clear();

	private:
		ZFPrjObjReadWrite& operator=(const ZFPrjObjReadWrite&) { return *this; }
		ZFPrjObjReadWrite(const ZFPrjObjReadWrite&) : ZFPrjMutex(NULL, false) {}
	};
} // end namespace zfprj
