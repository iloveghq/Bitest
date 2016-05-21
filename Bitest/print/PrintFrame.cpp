// PrintFrame.cpp : implementation file
//

#include "stdafx.h"

#include "PrintFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintFrame

IMPLEMENT_DYNCREATE(CPrintFrame, CFrameWnd)

CPrintFrame::CPrintFrame()
{
	m_pMainDlg = NULL;
	m_pWnd		= AfxGetApp()->m_pMainWnd;
	AfxGetApp()->m_pMainWnd = this;
}

CPrintFrame::~CPrintFrame()
{
}


BEGIN_MESSAGE_MAP(CPrintFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CPrintFrame)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintFrame message handlers

BOOL CPrintFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pView = new CPrintView();
	m_pView->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,AFX_IDW_PANE_FIRST,pContext);
    m_pView->MemcpyResInfo(&m_pMainDlg->m_testResult[0]);
	return TRUE;
}

void CPrintFrame::OnDestroy() 
{
	if(m_pView!=NULL)
	{
		m_pView->DestroyWindow();
	}
	CFrameWnd::OnDestroy();
	AfxGetApp()->m_pMainWnd = m_pWnd;
}
