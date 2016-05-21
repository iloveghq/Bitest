#pragma once

#include "Util.h"
#include "DATASTRUCT.h"
#include "global.h"

// CMyView 视图

class CMyView : public CScrollView
{
	DECLARE_DYNCREATE(CMyView)

protected:

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
	virtual void OnInitialUpdate();     // 构造后的第一次

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);  
public:  
	virtual ~CMyView();
	CMyView();           // 动态创建所使用的受保护的构造函数
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);  
	void OnFilePrintPreview(void);  
	void DrawPic(CDC* pDC);//自定义画图函数 

public:
	BOOL	m_bPrint;
	BOOL	m_bPrintCurve;
	CRect m_rectDraw;//绘制矩形区域大小

private:
	// Print 
	BOOL m_bPrintData;
	BOOL m_bPrintImage;
	BOOL m_bPrintHead;	
	UINT m_cxWidth;
	UINT m_cxOffset;
	UINT m_nLinesPerPage;
	UINT m_PageCount;
	UINT m_cyPrinter;
	CFont m_ListFont;
	CFont m_fontPrinter;
	CFont m_dataFont;
	CFont m_FontVR;
	CFont m_fontGroup;   //组标题的字体
public:
	BOOL m_bPrintLogo; //是否打印光维版权信息
	BOOL m_bPrintQuest;  //是否打印任务
	BOOL m_bPrintConfig; //是否打印配置
	BOOL m_bPrintComment;//是否打印注释
	BOOL m_bPrintTrace;//是否打印曲线
	BOOL m_bPrintEvent;//是否打印事件
	BOOL m_bShowGrid;//是否打印事件

	//ghq 2016.4.21
	INT m_nCurveNumber;
	CUtil m_util;
	PrintDetail m_stPrintDetail;
	CString  strListHead[8];
    CString  strListHead2Way[8];
	//CBitestDlg *m_dlgParent; 
public:
	void OnMyPrint();
	void OnMyPrintPreview();
	void PrintPageHeader(CDC* pDC, UINT nPageNumber, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);  
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
	void DrawGridLine(CDC* pDC, CRect& rect);//绘制栅格
	void GetPrintInfo(INT nCurSel); //获取打印需要的参数
	void DrawPrintContent(CDC* pDC, CRect rectDraw,INT nCurSel = 0, BOOL bSingle = TRUE); //绘制打印预览波形
	void DrawScale(CDC *pDC, CRect &rect, FLOAT fYmax = 0, FLOAT fXmax = 0);//绘制刻度
	void DrawCurve(CDC* pDC, CRect& rect, INT nCurveIndex);	
	void DrawTrace(CDC *pDC, CRect rc, INT nCurSel, FLOAT fontRatio = 1.0); //绘制带坐标曲线
	void MemcpyResInfo(TestResultInfo *pResultInfo);
	void DrawConfig(CDC* pDC, CRect rectDraw,INT &nBottom,INT nCurSel);
	void MemcpyQuestInfo(PrintQuest *pQuestInfo);
	void DrawQuestInfo(CDC* pDC, CRect rectDraw,INT &nBottom,INT nCurSel);
	void DrawCursorAB(CDC* pDC, CRect& rect);
private:
	void InnerDraw(CDC* pDC,INT m_nCurPage); //内部绘图只用
	void DrawEventHead(CDC* pDC, INT nLeft, INT nTop, CRect rectDraw,INT nCurSel);//绘制事件列表表头
	void DrawEventList(CDC* pDC, INT nLeft, INT nTop, INT nVspace, CRect rectDraw, INT nCurSel);//绘制事件列表
    void DrawEventList2Way(CDC* pDC, INT nLeft, INT nTop , INT nVspace , CRect rectDraw, INT nCurSel);//绘制双向事件列表	
	void InitFont(CDC* pDC); //初始化字体
	void InitPrintText();
	void DrawCurve2Way(CDC* pDC, CRect& rect, INT nCurveIndex);
	void DrawEvent(CDC* pDC, CRect& rect, INT nCurveIndex);
	void DrawEvent2Way(CDC* pDC, CRect& rect, INT nCurveIndex);
	void GetEvent2Way(TestResultInfo *pInfo2Way, TestResultInfo *infoA2B, TestResultInfo *infoB2A);
	void DrawScale2Way(CDC *pDC, CRect &rect, FLOAT fXmin, FLOAT fXmax);

	void DrawPrintContentPage2(CDC* pDC, CRect rectDraw, INT nCurSel, BOOL bSingle = TRUE);
	void DrawEventListPage2(CDC* pDC, INT nLeft, INT nTop, INT nVspace, CRect rectDraw, INT nCurSel);//绘制事件列表
	void DrawEventList2WayPage2(CDC* pDC, INT nLeft, INT nTop , INT nVspace , CRect rectDraw, INT nCurSel);//绘制双向事件列表	
	void GetEventInfo2Way(INT nIndex,CString &strText,INT nType);
public:
//	afx_msg void OnPrint();
};


