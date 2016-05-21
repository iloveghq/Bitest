#if !defined(AFX_PRINTVIEW_H__2AF9CEDE_8D19_4688_94ED_3995497B2427__INCLUDED_)
#define AFX_PRINTVIEW_H__2AF9CEDE_8D19_4688_94ED_3995497B2427__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintView.h : header file
//
#include "DATASTRUCT.h"
#include "Util.h"
/////////////////////////////////////////////////////////////////////////////
// CPrintView view
// CMyView 视图
const float FONTSIZE_RATIO[5] = {1.0, 0.8, 0.6, 0.45, 0.4};  //报表输出时用的字体的缩放比例，[0]表示单报表，[1]表示2图/页，[2]表示4图/页，[3]表示6图/页，[4]表示8图每页

typedef struct PrintQuest 
{	
	CString strTitle;//任务标题
	CString strID;//任务编号
	CString strContractor; //承包商
	CString strCustomer;  //用户
	CString strSorFileName; //文件名
	CString strTestDate;   //测试日期
	CString strPlayer;   //操作员

	//value
	CString QuestID;    //任务编号
	CString Contractor; //承包商
	CString Customer;  //用户
	CString SorFileName; //文件名
	CString TestDate;   //测试日期
	CString Player;   //操作员
};

typedef struct PrintConfig 
{	
	//name
	CString strTitle;  //标题文字
	CString strWavelength;   //测量波长
	CString strPulse;    //脉宽
	CString strOrigin;    //起点
	CString strOriCable;   //起点光缆号
	CString strOriFiber;   //起点光纤号
	CString strOriColor;   //起点关纤颜色

	CString strRange;    //距离
	CString strAverage;  //平均的次数
	CString strEnd;     //终点
	CString strEndCable;   //终点光缆号
	CString strEndFiber;   //终点光纤号
	CString strEndColor;   //终点颜色
	CString strOriLocation;//起点位置
	CString strEndLocation;   //终点位置

	CString strBackScatter;  //背向散射系数
	CString strLossThreshold;  //衰减门限
	CString strRefThreshold;  //反射门限
	CString strEndThreshold;  //光纤末端门限
	CString strIndex;    //测量时用的折射率
	CString strResolution;  //解析度
	CString strTitleAB;  //AB曲线
	CString strTitleBA;  //BA曲线
	//value
	CString OriCable;   //起点光缆号
	CString OriFiber;   //起点光纤号
	CString OriColor;   //起点关纤颜色
	CString EndCable;   //终点光缆号
	CString EndFiber;   //终点光纤号
	CString EndColor;   //终点颜色
	CString OriLocation;//起点位置
	CString EndLocation;   //终点位置
	CString Wavelength;   //测量波长
	CString Index;    //测量时用的折射率
	CString Pulse;    //脉宽
	CString Range;    //距离
	CString Average;  //平均的次数
	CString Resolution;  //解析度
	CString BackScatter;  //背向散射系数
	CString LossThreshold;  //衰减门限
	CString RefThreshold;  //反射门限
	CString EndThreshold;  //光纤末端门限
};

typedef struct PrintComment 
{
	CString strTitle; //注释标题文字
	CString strComment;   //注释
};

typedef struct PrintTrace
{	
	CString strTitle;  //轨迹线标题
	CString LocationA;   //A标位置
	CString LossA;    //A标处损耗
	CString LocationB;  //B标位置
	CString LossB;  //B标处损耗
	CString DistanceAB;  //AB位置间隔
	CString LossAB;  //AB损耗差
	CString SlopeAB;  //AB段损耗斜率
};

struct WaveConfig 
{
	COLORREF ColorCurve1;  //曲线1颜色
	COLORREF ColorCurve2;  //曲线2颜色
	COLORREF ColorBack;  //背景颜色
	COLORREF ColorGrid;  //栅格颜色
	COLORREF ColorEvent;  //事件颜色
	COLORREF ColorCursor;  //光标颜色
	BOOL ShowGrid; //是否显示网格
	BOOL ShowCursor; //是否显示光标
	BOOL ShowDoubleCursor; //是否显示双光标
	BOOL MultiMode; //是否为多曲线显示模式
	//Unit Page
	int UnitDbFig; //Dt刻度单位精度5\4\3
	int UnitDistFig; //距离刻度单位精度
	CString DistUnit; //距离单位
};

typedef struct PrintEvent 
{	
	CString strTitle;
	int EventColumn;    //事件列表列数
	int EventCount; //事件行数
};

//所有的打印信息
struct PrintDetail
{
	CString strTitle;
	CString strLogo;
	PrintQuest stPrintQuest;
	PrintComment stComment;
	PrintConfig stPrintConfig; //配置信息
	PrintTrace stPrintTrace;
	WaveConfig stWaveConfig;
	PrintEvent stPrintEvent;
	TestResultInfo stTestResultInfo[3];
	INT curSelection;//文件列表当前选中哪个
	INT totalCurveNumber;
	CString WaveList;
};


class CPrintView : public CView
{
public:
	CPrintView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPrintView)

// Operations
public:
	void OnMyPrint();
	void OnMyPrintPreview();
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrintView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Attributes
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
public:
	void DrawGridLine(CDC* pDC, CRect& rect);//绘制栅格
	void DrawPrintContent(CDC* pDC, CRect rectDraw,INT nCurSel = 0, BOOL bSingle = TRUE); //绘制打印预览波形
	void DrawScale(CDC *pDC, CRect &rect, FLOAT fYmax = 0, FLOAT fXmax = 0);//绘制刻度
	void DrawCurve(CDC* pDC, CRect& rect, INT nCurveIndex);	
	void DrawTrace(CDC *pDC, CRect rc, INT nCurSel, FLOAT fontRatio = 1.0); //绘制带坐标曲线
	void DrawConfig(CDC* pDC, CRect rectDraw,INT &nBottom,INT nCurSel);
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
	void PrintPageHeader(CDC* pDC, UINT nPageNumber, CPrintInfo* pInfo);
	void DrawPrintContentPage2(CDC* pDC, CRect rectDraw, INT nCurSel, BOOL bSingle = TRUE);
	void DrawEventListPage2(CDC* pDC, INT nLeft, INT nTop, INT nVspace, CRect rectDraw, INT nCurSel);//绘制事件列表
	void DrawEventList2WayPage2(CDC* pDC, INT nLeft, INT nTop , INT nVspace , CRect rectDraw, INT nCurSel);//绘制双向事件列表	

public:
	void MemcpyResInfo(TestResultInfo *pResultInfo);
	// Generated message map functions
public:
	//{{AFX_MSG(CPrintView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTVIEW_H__2AF9CEDE_8D19_4688_94ED_3995497B2427__INCLUDED_)
