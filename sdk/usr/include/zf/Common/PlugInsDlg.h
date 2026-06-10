#if !defined(AFX_PLUGINSDLG_H__6DEAE09D_82FC_4986_B2DB_6F230CC7A844__INCLUDED_)
#define AFX_PLUGINSDLG_H__6DEAE09D_82FC_4986_B2DB_6F230CC7A844__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlugInsDlg.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CPlugInsDlg 

class CPlugInsDlg : public CDialog
{
// Konstruktion
public:
	CPlugInsDlg( long req, CWnd* pParent = NULL);   // Standardkonstruktor
	~CPlugInsDlg();

    CString m_szPlugIn;
    long    m_require;

// Dialogfelddaten
	//{{AFX_DATA(CPlugInsDlg)
	enum { IDD = IDD_DIALOG_PLUGINS };
	CStatic	m_description;
	CListBox	m_list;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CPlugInsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CPlugInsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeList();
	afx_msg void OnDblclkList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_PLUGINSDLG_H__6DEAE09D_82FC_4986_B2DB_6F230CC7A844__INCLUDED_
