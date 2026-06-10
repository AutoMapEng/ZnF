#pragma once

#include "zfsapi.h"
#include "src/thread.h"

#include <list>
#include <vector>

class ZFSLinebyline;



class ZFS_API ZFLinebylineList
{
	friend class ZFSLinebyline;

    ZFThreadMutex zfslist_mtx;                      ///< Serialize access to zfslist

public:
	ZFLinebylineList();
	virtual ~ZFLinebylineList();

	typedef std::list <ZFSLinebyline*> lbl_list_t;	
	typedef lbl_list_t::iterator list_it;	///< Shortcut for iterator

protected:
    lbl_list_t zfslist;				///< list of all documents (scans)
//	ZFSLinebyline* current;	

	void Cb(ZFSLinebyline*,int); ///< call all callbacks

public:
    // Callbacks
	enum { ADD, REMOVE };
    typedef void (*cb_t)( int typ, ZFSLinebyline* scan ); // 
	std::vector<cb_t> cb;	///< open/close/add callback list

	int N();

    lbl_list_t getList();

	ZFSLinebyline* operator [] (unsigned int idx);
//	ZFSLinebyline* operator()() { return current; }

protected:
	void Add( ZFSLinebyline* );
	void Remove( ZFSLinebyline* );
};


extern ZFS_API ZFLinebylineList linebyline_list;

