// ProgressDlg.h : Header-Datei
// CG: Diese Datei wurde von der Komponente "Dialogfeld Fortschritt" hinzugefügt

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg Dialogfeld

#ifndef __PROGRESSDLG_H__
#define __PROGRESSDLG_H__

class CProgressDlg : public CDialog
{
	DECLARE_DYNCREATE(CProgressDlg)

    int     m_id; // Icon
    CString m_szStepText;
    bool    m_bCorner;
    bool    m_bAbortBtn;

// Konstruktion / Destruktion
public:
    CProgressDlg( bool abortbtn=true, bool corner=false );   // Standard-Konstruktor
    CProgressDlg( int from, int to, const char* text, bool abortbtn=true, bool corner=false );   // Standard-Konstruktor
    ~CProgressDlg();

    BOOL Create(CWnd *pParent=NULL);

    // Schaltfläche "Abbrechen" wird geprüft
    BOOL CheckCancelButton();
    // Anpassen des Dialogfelds Fortschritt
    void SetRange(int nLower=0,int nUpper=0);
    int  SetStep(int nStep);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  StepIt( int param=0 );
    void Set( int );
    void Set( int, const char* );
    void Set( const char* );
    void SetAbort( bool neu=true );
    void SetStepText( const char*, bool set=false );
        
// Dialogfelddaten
    //{{AFX_DATA(CProgressDlg)
	CButton	m_btnAbort;
	CStatic	m_staticStatus;
	CStatic	m_staticIcon;
    CProgressCtrl	m_Progress;
	//}}AFX_DATA

// Überschreibungen
    // Vom Klassen-Assistenten generierte Überschreibungen virtueller Funktionen
    //{{AFX_VIRTUAL(CProgressDlg)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
    //}}AFX_VIRTUAL

// Implementierung
protected:
    int m_nLower;
    int m_nUpper;
    int m_nStep;
    
    BOOL m_bCancel;
    BOOL m_bParentDisabled;

    void ReEnableParent();

    virtual void OnCancel();
    virtual void OnOK() {}; 
    void PumpMessages();

    // Generierte Nachrichtenzuordnungsfunktionen
    //{{AFX_MSG(CProgressDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __PROGRESSDLG_H__
