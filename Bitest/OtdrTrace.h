#pragma once
#include <math.h>
#include <vector>
#include "Util.h"
#include "global.h"
#include "afxcmn.h"

///////////////////////////////////////////////////////////
//CPoint 数据点
class CElementPoint//
{

public:
	double x, y;
	CElementPoint(){ x=y=0;}
	CElementPoint(double c1, double c2)
	{
		x = c1;
		y = c2;    
	}//eif
	CElementPoint& operator=(const CElementPoint& pt)
	{
		x = pt.x;  y = pt.y;
		return *this;
	}//eif
	CElementPoint (const CElementPoint& pt)
	{
		*this = pt;
	}//eif
};
///////////////////////////////////////////////////////////
//注释
class CGraphAnnotation 
{
public:
	CString   m_Caption;//文本
	COLORREF m_Color, m_BkColor;//颜色
	BOOL m_bHorizontal;//是否水平
	BOOL m_bVisible;//是否可见
	CElementPoint place;//位置

	CGraphAnnotation () 
	{
		m_Caption = _T("");
		m_Color = RGB(255,255,255);
		m_BkColor = RGB(120,120,120);
		m_bHorizontal = TRUE;
		m_bVisible = TRUE;
		place = CElementPoint(0.5,0.5);
	}//eif
};

///////////////////////////////////////////////////////////
//曲线
class CGraphElement
{
public:
	CGraphElement()
	{
		bIsPlotAvailable = FALSE;
		m_LineColor = RGB(0,0,0);
		m_bShow = TRUE;
		//m_strName.Format(_T("Element-%d"),id);
		m_pData = NULL;
		m_pPointsToDraw = NULL;
		m_nOffsetY = 0;
		m_fOffsetX = 0;
		m_nPointsCount = 0;
	}//eif
	struct TraceEvent
	{
		INT m_nIndex;
		FLOAT m_distance;
	};
	INT  DistanceToIndex(FLOAT fDistance);//根据距离转化为对应的数据点下标,一般用来确定每次绘图的起始点及结束点
	//void DrawCurve(CDC* pDC, CRect rect);//绘制曲线	
	//void DrawCurve2Way(CDC* pDC, CRect rect);//绘制曲线, 双向测试用
	void DrawCurve(CDC* pDC, CRect rect,int nIndexCurve);
	void DrawCurve2Way(CDC* pDC, CRect rect,int nIndexCurve);
	void DrawEvents(CDC* pDC, CRect rect);//绘制事件点	
    void DrawEvents2Way(CDC* pDC, CRect rect); //绘制事件点,双向测试
	BOOL m_bShow;//是否显示
	BOOL bIsPlotAvailable;//是否可绘
	BOOL m_bShowEvent;//是否显示事件列表
	BOOL m_bSelected;//是否是选中曲线
	COLORREF m_LineColor;//线条颜色
	CString m_strName;//名称
	INT m_nPointsCount;//总共有多少个数据点,当前曲线实际点数
	//INT m_nPointsCountVirtual;//虚拟曲线点数。主要是为了多条曲线显示，以最长距离曲线为准计算当前曲线应该有多少个点(比当前实际的点数大)时，得到的长度等于最长距离曲线长度
	INT m_nOffsetY;//y轴偏移量
	FLOAT m_fLength;//长度
	FLOAT m_fOffsetX;//x轴偏移量
	static FLOAT m_fRangeX[2];//保存当前绘图时最小最大距离,相对x轴，针对多条曲线
	static FLOAT m_fRangeY[2];//保存当前绘图时y轴范围，针对多条曲线
	static FLOAT m_fMaxDistance;//最长距离，针对多条曲线
	FLOAT m_fDataSpacing;//理解为两点间距离,曲线两点间距离
	FLOAT* m_pData;//点集合
	CPoint* m_pPointsToDraw;//
	std::vector<TraceEvent> m_vAllEvents;

	//ghq 2016.4.12
	INT m_iSelEventLine;
	BOOL SelectLine( const CPoint  pt,CRect rtCurve, INT iSelMode);
	INT GetSelectLine( );
	void SetSelectLine(INT iSelEventLine );
	void DrawScale(CDC *pDC,CRect rect,INT nType);
	INT GetSelectEventPos();
};

// COtdrTrace
class CBitestDlg;
class COtdrTrace : public CButton
{
	DECLARE_DYNAMIC(COtdrTrace)

public:
	COtdrTrace();
	virtual ~COtdrTrace();
public:
	struct CursorAB
	{
		CursorAB()
		{
			m_nPos = 0;
			m_fPos = 0;
			m_colorLine = RGB(0,0,255);//如选中A，则此时B为m_colorLine，选中B，则此时A为m_colorLine
			m_colorLabel = RGB(255,255,255);//A或B坐标旁边的文本，非事件坐标旁边的文本
		}
		INT m_nPos;//位置，针对屏幕，多少象素
		FLOAT m_fPos;//位置，多少km,针对曲线位置
		CString m_strAOrB;//"A"/"B"
		CString m_strDistance;//多少距离，如100km，光标旁显示的字符串
		CString m_strDB;// 增加 AB光标的dB值
		COLORREF m_colorLine;//竖线颜色
		COLORREF m_colorLabel;//竖线上文本，如A 100km颜色
	}m_cursorA,m_cursorB,m_cursorAminusB,m_cursorEvent;//
public:
	void InitPaint(CDC* pDC);//初始化绘图用的笔刷等
	void DoDraw(CDC* pDC);
	void DrawMouseSeleRect(CDC* pDc);//绘制鼠标拖动矩形
	void DrawCurves(CDC* pDC);//绘制所有曲线	
	void DrawCurve2Way(CDC* pDC);//绘制双向测试曲线
	void CalculateRect();//计算绘图区域
	void SetRange(float xmin, float xmax, float ymin, float ymax, BOOL bSetScrollInof = TRUE);//设置绘图界限，需要重绘
	void ReleaseAllData();//清空所有数据2013.08.14

	void SetVerScoll(); //设置竖直滚动条
	void SetHorScroll();//设置水平滚动条
	void InitScrollInfo();//初始化滚动条
	void SetMyScrollInfo(float xmin, float xmax, float ymin, float ymax);//设置滚动条范围
	void SetScrollRangeX();//重设水平方向范围
	void SetMaxDistance(FLOAT fMaxDistance);
	void SetMaxTraceY(INT nShowType);
	//ghq 2016.5.10 增加AB光标
	void OnInitAB(void);
	void DrawLineAB(CDC* pDC);
	BOOL IsInCurRangeX(FLOAT fDistance);
	BOOL IsInCurRangeY(FLOAT fDistance);
	void DrawLineABDetail(CDC* pDC, CursorAB* pCursorAB);
	void SetCurveCursorData();
	void CursorABOnMove(CPoint pointMoving);
	void CreateMyFont(CDC* pDC, CFont& font, CString fontName);
	void OnShowAB();
	INT GetABSelectState(CPoint pointClick);
	void ShowCatch(CDC *pDC, CPoint pt);
	void SetCurrentTypeAB(INT nABType);
	void SetABSelStatus(INT nType);
	INT GetCurrentCursorIndex();
	//e
public:
	CRect m_rtMouseSelect;//鼠标选择区域cwcq
	CRect m_rtClient;//
	CRect m_rtCurve; //曲线绘图区域大小
	BOOL m_bLeftButtonDown;
	BOOL m_bInitPaint;//是否已进行过初始化笔刷等
	INT m_nCurMode;//模式
	CDC m_dcGrid;//绘网格DC
	CDC m_dcCurve;//绘图DC分辨率
	CBitmap m_bmpGrid;
	CBitmap m_bmpCurve;
	FLOAT m_fResolutionY, m_fResolutionX;//(屏幕每一象素占多少个数据点/每一象素对应曲线多长距离)
	FLOAT m_fRangeY[2], m_fRangeX[2], m_fBiRangeX[2];//最大最小值
	FLOAT m_fOldRangeX[2], m_fOldRangeY[2];//原来的最大最小值,用于缩小
	std::vector<CGraphElement> m_vAllData;//所有数据
	//ghq 2016.5.10 增加AB光标
	BOOL m_bShowLineAB;//是否显示A/B光标
	BOOL m_bDrawGrid;//是否绘制格子线(GridLine)//zll 2013.4.22
	INT  m_nCurABSele;//当前是否选中AB标，如果选中，选中是A还是B
	INT  m_nABSele;//当前A还是B作为缩放中心
	INT  m_nCurveIndex;//当前选中曲线下标
	INT  m_nABType;//光标选择模式，当前只选一个光标(A/B)或同时选两个(AB,拖动一条另一条也会动)
	//e
	INT m_nCount;
public:
	CFont m_font;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
public:
	//ghq 2016.4.12
	INT m_iSelEvent;
	CUtil m_util;
	CBitestDlg *m_dlgParent;
	FLOAT m_fMaxDistance;//最长距离，针对多条曲线
	INT m_iSelMode;//当前选择单向/双向模式
    CPoint m_ptMouse;  //保存鼠标的位置lzy2014.9.9
	int m_nSelectCursor; //Seleced标志,1选中A，2选中B, 4选中AB
	int m_nPointA;//A线的位置点，横坐标 不可设为无符号型,  //始终以最长距离的曲线为准wcq
	int m_nPointB;//B线的位置点，横坐标  //始终以最长距离的曲线为准wcq
	//滚动条
	int m_nHorzPos;
	FLOAT m_nVertPos;
	int m_nHorzMaxSize;
	FLOAT m_nVertMaxSize;
	int m_nHorzPageSize;
	FLOAT m_nVertPageSize;
	int m_nHorzOneStep;
	FLOAT m_nVertOneStep;
	int m_nOneStep;
	//
	BOOL m_bShowScale;  //显示坐标lzy2014.9.9
	WORD m_wdGridH;  //水平轴分割数
	WORD m_wdGridV;  //垂直轴分割数

	afx_msg void OnSize(UINT nType, int cx, int cy);
	//ghq 2016.4.15
	void ZoomInHorizonForBig(FLOAT fPercent = OTDR_TRACE_DEFAULT_ZOOM_PERCENT);//水平放大 fPercent放大比例，默认2%
	void ZoomOutHorizonForSmall(FLOAT fPercent = OTDR_TRACE_DEFAULT_ZOOM_PERCENT);//水平缩小 fPercent放大比例，默认2%
	void ZoomInVertialForBig(FLOAT fPercent = OTDR_TRACE_DEFAULT_ZOOM_PERCENT);//竖直放大 fPercent放大比例，默认2%
	void ZoomOutVertialForSmall(FLOAT fPercent = OTDR_TRACE_DEFAULT_ZOOM_PERCENT);//竖直缩小 fPercent放大比例，默认2%
	void ZoomInBothForBig(FLOAT fPercent = OTDR_TRACE_DEFAULT_ZOOM_PERCENT);
	void ZoomOutBothForSmall(FLOAT fPercent = OTDR_TRACE_DEFAULT_ZOOM_PERCENT);
	void ZoomReset();//还原，水平竖直缩放为
	void ZoomInClickForBig(INT nPointX=0, INT nPointY=0, FLOAT fPercent = OTDR_TRACE_DEFAULT_ZOOM_PERCENT * 5);//单击放大，默认放大10%
	FLOAT GetCenterOffset();
	BOOL IsCurrentTypeAB();
	FLOAT GetYDB(FLOAT fPos);
	FLOAT GetMaxDistance();
	void SetRadioType(INT iSelMode);
	void RefreshCurve();
	void MoveToEvent();
	void DrawGrid(CDC *pDC);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

class CEventList :
	public CListCtrl
{
public:
	CEventList(void);
	~CEventList(void);
public:
	CBitestDlg *m_dlgParent;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
};


