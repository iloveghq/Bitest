#if !defined(AFX_PRINTFRAME_H__51AF2B0A_B210_4483_94F7_BDCB148AB977__INCLUDED_)
#define AFX_PRINTFRAME_H__51AF2B0A_B210_4483_94F7_BDCB148AB977__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintFrame.h : header file
//
#include "BitestDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CPrintFrame frame


#define	WM_MY_PRINT			(WM_USER+1003)
#define	WM_BEGIN_PRINTING		(WM_USER+1004)
#define	WM_END_PRINTING			(WM_USER+1005)
#define PRINTMARGIN 2
#include "PrintView.h"
class CPrintFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CPrintFrame)
public:
	CPrintFrame();           // protected constructor used by dynamic creation

// Attributes
public:
	CPrintView	*m_pView;
	CBitestDlg		*m_pMainDlg;
	CWnd		*m_pWnd;
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintFrame)
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrintFrame();

	// Generated message map functions
	//{{AFX_MSG(CPrintFrame)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTFRAME_H__51AF2B0A_B210_4483_94F7_BDCB148AB977__INCLUDED_)
