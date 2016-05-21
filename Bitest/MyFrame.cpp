// MyFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "Bitest.h"
#include "MyFrame.h"


// CMyFrame

IMPLEMENT_DYNCREATE(CMyFrame, CFrameWnd)

CMyFrame::CMyFrame(CBitestDlg* pWnd,INT nType)
{
	m_pMainDlg= pWnd;
	m_iShowType = nType;
	if(!Create(NULL,_T("Print Preview"),WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE,CRect(0,0,0,0)))
	{
		TRACE0("Failed to create view window\n");
		AfxMessageBox(_T("Failed to create view window"));
	}
}

CMyFrame::CMyFrame()
{
	m_iShowType = 0;
}

CMyFrame::~CMyFrame()
{
	//m_pMainDlg->ShowWindow(SW_SHOW);
}


BEGIN_MESSAGE_MAP(CMyFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()




int CMyFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CCreateContext context;	
	context.m_pNewViewClass = RUNTIME_CLASS(CMyView);	
	context.m_pCurrentFrame = this;
	context.m_pCurrentDoc = NULL;
	context.m_pLastView = NULL;	
	m_pView = STATIC_DOWNCAST(CMyView, CreateView(&context)); 
	if(m_pView != NULL)		
	{   		
		m_pView->ShowWindow(SW_HIDE);		
		SetActiveView(m_pView);		
	}	
	SetIcon(m_pMainDlg->GetIcon(FALSE),FALSE);
	SetIcon(m_pMainDlg->GetIcon(TRUE),TRUE);
	//ShowWindow(SW_MAXIMIZE);	
	CWinApp *pApp=AfxGetApp();
	pApp->m_pMainWnd=this;	
	m_pView->MemcpyQuestInfo(&m_pMainDlg->m_printQuest);
	m_pView->MemcpyResInfo(&m_pMainDlg->m_testResult[0]);

	if(m_iShowType != 0)
	{
		m_pView->ShowWindow(SW_SHOW);		
		ShowWindow(SW_MAXIMIZE);
	    m_pView->OnFilePrintPreview();	
	}
	//m_pView->SendMessage(WM_COMMAND, ID_FILE_PRINT);//直接打印
	m_pMainDlg->ShowWindow(SW_SHOW);
	return 0;
}