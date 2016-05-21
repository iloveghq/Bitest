// BitestDlg.h : 头文件
//

#pragma once
#include "GrandwayReg.h"
#include "VARIABLE.h"
#include "Util.h"
#include "DATASTRUCT.h"
#include "SORDATA.h"
#include "afxwin.h"
#include "OtdrTrace.h"
#include "afxcmn.h"
#include "global.h"
#include "PageOpen.h"
#include "PageSetup.h"
#include "GaborDllUtil.h"

	//void DRWAPRINTER(CDC *pDC,CPrintInfo *pInfo,void *pVoid);
// CBitestDlg 对话框
class CBitestDlg : public CDialog
{
// 构造
public:
	CBitestDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CBitestDlg();
// 对话框数据
	enum { IDD = IDD_BITEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnOpenFile();
	afx_msg void OnRadioShow(UINT uID);    //select show type

//自定义
private:
	afx_msg void OnButtonHelp();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedButtonOrig();
	afx_msg void OnBnClickedButtonZoomin();
	afx_msg void OnBnClickedButtonZoomout();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedLang();
	afx_msg void OnChs();
	afx_msg void OnEng();
	afx_msg void OnBnClickedButtonPrint();
	afx_msg void OnPrint();
	afx_msg void OnPreview();
	afx_msg void OnBnClickedButtonZoominH();
	afx_msg void OnBnClickedButtonZoomoutH();
	afx_msg void OnBnClickedButtonZoominV();
	afx_msg void OnBnClickedButtonZoomoutV();
	afx_msg void OnSetup();
	afx_msg void OnBnClickedRadioAb();
	afx_msg void OnBnClickedRadioAorb();
	afx_msg void OnBnClickedAddEvent();
	afx_msg void OnBnClickedDelEvent();
	afx_msg void OnBnClickedCheckCursorab();

	LRESULT OnBeginPrinting(WPARAM wParam,LPARAM lParam);
	LRESULT OnEndPrinting(WPARAM wParam,LPARAM lParam);
	BOOL SaveLangSetting();
	INT GetZoomType();

	void resize();
	void SetEvents(TestResultInfo& testResultInfo, CGraphElement* graphElement);
	void OnOpenFileOk();
	void InitList();//单向列表初始化
	void InitList2Way();//双向测试事件列表初始化
	void OnBnClickedButtonZoomAny(INT nIndex);
	void OnInitPositon();
	void OnInitBtnIcon();
	void LoadLangSetting();
	void SetLocaleString();
	//计算距离,以c/2nf公式计算.xpoint:点位置加1，Frequency:频率， Refraction:折射率
	float GetDistance(unsigned int xpoint, FLOAT Frequency, FLOAT Refraction);
	//处理一组数据,将sorData转换为可以显示的数据
	void DealFullData(OTDR_ALL_UPLOAD_DATA* allData, SORDATA* sorData, ShowType myType);
	//获取事件的类型
	CString GetEventType(UINT iType, char chType);	
	//显示一条曲线
	void ShowTrace(ShowType myType);
	//显示列表
	void ShowEvents();
	//显示列表,双向所有的事件
	void ShowEvents2Way();	
	//给曲线图绘制坐标
	void SetCoordinate(CoordinateCoef xyCoef);
	//双向测试后，处理一下事件的信息，形成双向测试事件
	void GetEvent2Way(TestResultInfo *pInfo2Way,      //双向测试事件
					  TestResultInfo *infoA2B,     //A->B测试事件
					  TestResultInfo *infoB2A);    //B->A测试事件

	//显示曲线
	void ShowOnceData(POTDR_ALL_UPLOAD_DATA pOtdrAllUploadData);
	//显示双向数据
	void ShowBidirectional();
	//更新事件列表
	void FreshEventList();
	//创建弹出窗口
	void CreateAboutDlg();
    void CreateOpenFileDlg();
	void CreateSetupDlg();
	void SetABType(INT nType);
	void GaborResultToEvent(TestResultInfo* pCurveData);
	float Get_XCoord(unsigned int xpoint,TestResultInfo* pData);
	void EventToGaborResult(TestResultInfo* pCurveData);
    void GetEvent2WayIndex(TestResultInfo *infoA2B, TestResultInfo *infoB2A,INT nPoints,INT &nDelIndex1,INT &nDelIndex2);
	INT GetEvent2WayIndex2(INT nIndex);
public:
	//数据处理，产生显示的曲线，以及事件的信息
	void DataProcess(OTDR_ALL_UPLOAD_DATA* allData, TestResultInfo* testInfo, ShowType myType);
	void OnDown(UINT nID);
	void OnUp(UINT nID);
	void SetFileText(CString strFileName,INT nIndex);
	void OnSetQuestInfo(PrintQuest printQuest);
	void ChangeABStatus(BOOL bSelected);
	void OnResetEventList();
	void ResetEventButtonStatus();	
private:
	POINT    m_ptMin;
	CButton m_btnFile;
	CButton m_btnPrint;
	CButton m_btnLang;
	CButton m_btnSetup;
	CButton m_btnAbout;
	CButton m_btnExit;

	CButton m_btnOriginal;
	CButton m_btnZoomin;
	CButton m_btnZoomout;
	CButton m_btnZoomInH;
	CButton m_btnZoomOutH;
	CButton m_btnZoomInV;
	CButton m_btnZoomOutV;

	CRect m_rtCurve;
	CRect m_rtEvent;

	CRect m_rtBtnFile;
	CRect m_rtBtnPrint;
	CRect m_rtBtnLang;
	CRect m_rtBtnSetup;
	CRect m_rtBtnAbout;
	CRect m_rtBtnExit;

	CRect m_rtFile1;
	CRect m_rtFile2;

	CRect m_rtBtnReset;
	CRect m_rtBtnZoominB;
	CRect m_rtBtnZoomoutB;
	CRect m_rtBtnZoominH;
	CRect m_rtBtnZoomoutH;
	CRect m_rtBtnZoominV;
	CRect m_rtBtnZoomoutV;

	CRect m_rtBtnAddevent;
	CRect m_rtBtnDelevent;

	CRect m_rtRadioA2B;
	CRect m_rtRadioB2A;
	CRect m_rtRadioBoth;

	CRect m_rtCheckAB;
	CRect m_rtRadioAandB;
	CRect m_rtRadioAorB;

	BOOL m_bZoomin;
	BOOL m_bZoomout;
	BOOL m_bZoominH;
	BOOL m_bZoomoutH;
	BOOL m_bZoominV;
	BOOL m_bZoomoutV;

	BOOL	m_bPrintData;
	BOOL	m_bPrintImage;
	BOOL	m_bPrintHead;
	UINT m_cxWidth;
	UINT m_cxOffset;
	UINT m_nLinesPerPage;
	UINT m_cyPrinter;
	CFont		m_ListFont;
	CFont		m_fontPrinter;
	CButton m_radioAB;
	CButton m_radioBA;
	CButton m_radioBoth;
	CStatic m_txtFile1;
	CStatic m_txtFile2;

	CToolTipCtrl m_Mytip;
	CEventList m_listEvent;
	CImageList m_listImage;
	CPageOpen  *m_pdlgOpen;
	CPageSetup  *m_pdlgSetup;

	CGrandwayReg  m_reg;//注册表操作类
	OTDR_ALL_UPLOAD_DATA m_allData[3];  //0表示a->b, 1表示b->a, 2表示双向数据
	CString  m_strPath[3];  //数据文件路径, 0为A2B, 1为B2A, 2为双向数据
	COtdrTrace m_otdrTrace;// 绘制曲线的控件
	CUtil m_util;
	int m_iRadioShow;	// 显示的模式
	CoordinateCoef m_xyCoef;//坐标数据
	//CEndGabor m_endgabor;
	CSorAssist m_sorAssist;
	CGaborDllUtil g_gaborDllUtil;//ghq.2016.05.12
public:
	//解析后的数据
	TestResultInfo m_testResult[3];
	PrintQuest m_printQuest;
	BOOL m_bCursorAB;
	INT m_nSelectCursor; //Seleced标志,1选中A，2选中B, 4选中AB
};
