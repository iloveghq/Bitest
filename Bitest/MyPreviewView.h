#include <afxpriv.h>
// CMyPreviewView 视图
#include "MyFrame.h"
#include "MyView.h"

class CMyPreviewView : public CPreviewView
{
	DECLARE_DYNCREATE(CMyPreviewView)

protected:
	CMyPreviewView();           // 动态创建所使用的受保护的构造函数
	virtual ~CMyPreviewView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPreviewClose();
	afx_msg void OnPreviewPrint();
	virtual void OnDraw(CDC* /*pDC*/);

	CMyView * m_pView;
};