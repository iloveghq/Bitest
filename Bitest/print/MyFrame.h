#pragma once
#include "BitestDlg.h"
#include "MyView.h"

// CMyFrame 框架

class CMyFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CMyFrame)
public:
	CMyFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CMyFrame();

	CMyFrame(CBitestDlg* pWnd,INT nType);  
public:
	CBitestDlg *m_pMainDlg;//主对话框  
	CMyView * m_pView;  
	INT m_iShowType;

protected:
	DECLARE_MESSAGE_MAP()

public:  
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	virtual void PostNcDestroy();
};


