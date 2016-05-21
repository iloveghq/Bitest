// MyPreviewView.cpp : 实现文件
//

#include "stdafx.h"
#include "Bitest.h"
#include "MyPreviewView.h"


// CMyPreviewView

IMPLEMENT_DYNCREATE(CMyPreviewView, CPreviewView)

CMyPreviewView::CMyPreviewView()
{

}

CMyPreviewView::~CMyPreviewView()
{
}

BEGIN_MESSAGE_MAP(CMyPreviewView, CPreviewView)
	ON_COMMAND(AFX_ID_PREVIEW_CLOSE, &CMyPreviewView::OnPreviewClose)
	ON_COMMAND(AFX_ID_PREVIEW_PRINT, &CMyPreviewView::OnPreviewPrint)
END_MESSAGE_MAP()


// CMyPreviewView 消息处理程序
void CMyPreviewView::OnPreviewPrint()
{
	// TODO: 在此添加命令处理程序代码
	//m_pPrintView->SendMessage(WM_COMMAND, ID_FILE_PRINT);
	CMyFrame* pf=(CMyFrame*)::AfxGetMainWnd();
	pf->m_pView->OnMyPrint();
}

void CMyPreviewView::OnPreviewClose()
{
	// TODO: 在此添加命令处理程序代码
	CMyFrame* pf=(CMyFrame*)::AfxGetMainWnd();
	CWinApp *pApp=AfxGetApp();      
	pApp->m_pMainWnd=pf->m_pMainDlg;
	pf->DestroyWindow();
}

void CMyPreviewView::OnDraw(CDC* pDC)
{
	CPreviewView::OnDraw(pDC);	
	// TODO: 在此添加专用代码和/或调用基类
	m_pToolBar->PostMessage(WM_IDLEUPDATECMDUI, (WPARAM)TRUE);// 控制条的命令状态更新
}
