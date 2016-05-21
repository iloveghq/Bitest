// MyView.cpp : 实现文件
//

#include "stdafx.h"
#include "Bitest.h"
#include "MyView.h"
#include "MyPreviewView.h"
#include "global.h"
#include <math.h>
#include "BitestDlg.h"
// CMyView

extern LANG g_iLang;
extern INT g_nTraceYmax;
extern CursorInfo g_cursorInfo;


IMPLEMENT_DYNCREATE(CMyView, CScrollView)

CMyView::CMyView()
{
	m_nMapMode = MM_TEXT;
	m_bPrint = TRUE;
	m_bPrintCurve = TRUE;
	m_bPrintHead = TRUE;
	m_stPrintDetail.curSelection = 0;
	m_stPrintDetail.totalCurveNumber = 0;
	InitPrintText();
}

CMyView::~CMyView()
{
	//以下删除数据
	for(INT i = 0; i < 3; i++)
	{
		m_stPrintDetail.stTestResultInfo[i].EventList.vEventInfo.clear();
	}
}

BEGIN_MESSAGE_MAP(CMyView, CScrollView)
//	ON_COMMAND(ID_PRINT, &CMyView::OnPrint)
END_MESSAGE_MAP()


// CMyView 绘图

void CMyView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CMyView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码

	DrawPic(pDC);
}

// CMyView 诊断

#ifdef _DEBUG
void CMyView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyView 消息处理程序
void CMyView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类
	pDC->SetMapMode(MM_ANISOTROPIC); 
	//转换坐标映射方式 
	CSize   size=CSize(800,   560); 
	pDC->SetWindowExt(size);             //确定窗口大小 
	//得到实际设备每逻辑英寸的像素数量 
	INT   xLogPixPerInch=pDC->GetDeviceCaps(LOGPIXELSX);   
	INT   yLogPixPerInch=pDC->GetDeviceCaps(LOGPIXELSY); 
	//得到设备坐标和逻辑坐标的比例 
	long   xExt=(long)size.cx*xLogPixPerInch/96; 
	long   yExt=(long)size.cy*yLogPixPerInch/96; 
	pDC->SetViewportExt(xExt,yExt);
	CScrollView::OnPrepareDC(pDC, pInfo);
}

BOOL CALLBACK _AfxMyPreviewCloseProc(CFrameWnd* pFrameWnd)
{
	ASSERT_VALID(pFrameWnd);
	CMyPreviewView* pView = (CMyPreviewView*) pFrameWnd->GetDlgItem(AFX_IDW_PANE_FIRST);
	ASSERT_KINDOF(CPreviewView, pView);
	pView->OnPreviewClose();
	return FALSE;
}

void CMyView::OnFilePrintPreview(void)
{
	CPrintPreviewState* pState = new CPrintPreviewState;
	pState->lpfnCloseProc =_AfxMyPreviewCloseProc;//设置打印预览窗口关闭时的调用函数；
	if(!DoPrintPreview(AFX_IDD_PREVIEW_TOOLBAR, this,RUNTIME_CLASS(CMyPreviewView), pState))
	{
		TRACE0("Error: DoPrintPreview failed.\n");
		AfxMessageBox(_T("really??"));
		delete pState;
	}
}

void CMyView::DrawPic(CDC* pDC)//自定义画图函数 
{
	//CBrush ColorBrush(RGB(100,100,0));
//	pDC->SelectObject(&ColorBrush);
	//pDC->Ellipse(0,0,1000,1000);
}

void CMyView::OnMyPrint()
{
	GetParent()->ShowWindow(SW_SHOWMINIMIZED);
	m_bPrint = TRUE;
	CView::OnFilePrint();
}

void CMyView::OnMyPrintPreview()
{
	GetParent()->ShowWindow(SW_SHOWMAXIMIZED);
	m_bPrint=FALSE;
	CView::OnFilePrintPreview();
}

BOOL CMyView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	//打印单个报表用竖打，打印多个报表用横打 lzy2014.8.28
	//INT nCurveNumber = _wtoi(g_sorFileArray.reportConfig.WaveList);	
	LPDEVMODE lpDevMode;
	PRINTDLG pd = pInfo->m_pPD->m_pd;
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd))
	{
		lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);
		if(lpDevMode)
		{
			lpDevMode->dmPaperSize = DMPAPER_A4;
			if((!m_nCurveNumber ) || (2 == m_nCurveNumber) || m_bPrintCurve ) //单个报表,4个报表
			{
				lpDevMode->dmOrientation = DMORIENT_PORTRAIT; //纵向打印
			}
			else  //多个报表
			{
				lpDevMode->dmOrientation = DMORIENT_LANDSCAPE; //横向打印
			}			
		}
		GlobalUnlock(pd.hDevMode);
	}	
	
	if(DoPreparePrinting(pInfo))
		return TRUE;
	else
	{
     //   GetParent()->DestroyWindow();
		return FALSE;
	}
}

void CMyView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnBeginPrinting(pDC, pInfo);
	INT iEventNum = 0;
	CMyFrame *pFrame =(CMyFrame *)GetParent();
	//pFrame->m_pMainDlg->SendMessage(WM_BEGIN_PRINTING,(WPARAM) pDC, (LPARAM) pInfo);

	InitFont(pDC);

	TEXTMETRIC tm;
	CFont* pOldFont = pDC->SelectObject (&m_fontPrinter);
	pDC->GetTextMetrics (&tm);
	m_cyPrinter = tm.tmHeight + tm.tmExternalLeading;
	CSize size = pDC->GetTextExtent (_T ("---------1---------2---------3---------4---------5---------6---------7---------8---"), 81);
	pDC->SelectObject (pOldFont);
	m_cxWidth = size.cx;

	m_nLinesPerPage = (pDC->GetDeviceCaps (VERTRES) -
		(m_cyPrinter * (3 + (2 * PRINTMARGIN)))) / m_cyPrinter;

	if(m_stPrintDetail.stTestResultInfo[0].DataPointsNum>0&&m_stPrintDetail.stTestResultInfo[1].DataPointsNum>0)
	{
        GetEvent2Way(&m_stPrintDetail.stTestResultInfo[2], &m_stPrintDetail.stTestResultInfo[0], &m_stPrintDetail.stTestResultInfo[1]);
		iEventNum = m_stPrintDetail.stTestResultInfo[2].EventList.vEventInfo.size();
	}
	else if(m_stPrintDetail.stTestResultInfo[0].DataPointsNum>0&&m_stPrintDetail.stTestResultInfo[1].DataPointsNum<=0)
	    iEventNum = m_stPrintDetail.stTestResultInfo[0].EventList.vEventInfo.size();
	else
	    iEventNum = m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo.size();

	INT maxPage=iEventNum>EVENT_LIST_PRINT_MAX?2:1;//配置页数

	pInfo->SetMaxPage (maxPage);


	m_cxOffset = (pDC->GetDeviceCaps (HORZRES) - size.cx) / 2;
	//设置打印区域
	m_rectDraw.SetRect(0,0,pDC->GetDeviceCaps (HORZRES), pDC->GetDeviceCaps (VERTRES));
}

void CMyView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnEndPrinting(pDC, pInfo);

	CMyFrame *pFrame =(CMyFrame *)GetParent();
	//pFrame->m_pMainDlg->SendMessage(WM_END_PRINTING,(WPARAM) pDC, (LPARAM) pInfo);
	//ghq 2016.5.3 print end but not close maindlg
	//if(m_bPrint)		//print not preview
	//	GetParent()->DestroyWindow();

	if(m_fontPrinter.m_hObject!=NULL)
		m_fontPrinter.DeleteObject ();

	if(m_dataFont.m_hObject!=NULL)
		m_dataFont.DeleteObject();

	if(m_FontVR.m_hObject!=NULL)
		m_FontVR.DeleteObject();
}

void CMyView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CMyFrame *pFrame =(CMyFrame *)GetParent();
	//pFrame->m_pMainDlg->SendMessage(WM_MY_PRINT,(WPARAM) pDC, (LPARAM) pInfo) ;
	if(m_bPrintHead)
		PrintPageHeader(pDC,pInfo->m_nCurPage,  pInfo);
	InnerDraw(pDC,pInfo->m_nCurPage);
}

void CMyView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView) 
{
	CView::OnEndPrintPreview(pDC, pInfo, point, pView);

	GetParent()->DestroyWindow();
	//恢复窗口最大化
	//MAIN->ShowReport(TRUE);
	//打印
	//OnMyPrint();//zll 2013.6.26
}

void CMyView::PrintPageHeader(CDC* pDC, UINT nPageNumber, CPrintInfo* pInfo)
{
	CFont *pOldFont;
	CFont fontHeader;
	fontHeader.CreatePointFont(100,_T("FixedSys"),pDC);//
	//fontHeader.CreatePointFont(100,_T("宋体"),pDC);//
	pOldFont=(CFont *)(pDC->SelectObject(&fontHeader));
	INT nPosY,nPosX;
	nPosY=m_cyPrinter;
	nPosX=m_cxOffset;
	pDC->TextOut(nPosX,nPosY,CString(_T("")));
	//CString str;
	//str.Format(_T("第 %d 页 / 共 %d 页"),nPageNumber,pInfo->GetMaxPage());
	//CSize size=pDC->GetTextExtent(str);
	//nPosX=m_cxOffset+m_cxWidth-size.cx;
	//pDC->TextOut(nPosX,nPosY,str);
	//pDC->MoveTo(m_cxOffset,1.5*m_cyPrinter);
	//pDC->LineTo(m_cxOffset+m_cxWidth,1.5*m_cyPrinter);
	pDC->SelectObject(pOldFont);
	fontHeader.DeleteObject();
}

//新报表用的函数，画报表内容。lzy 2014.8.11
void CMyView::DrawPrintContent(CDC* pDC, CRect rectDraw, INT nCurSel, BOOL bSingle)
{
	//画线
	INT nXoffset = rectDraw.left;
	INT nYoffset = rectDraw.top;
	INT nVspace = rectDraw.Height() / 64;
	INT nCurHeight = nYoffset+ nVspace * 3;
	INT nHleftspace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	INT nHmidspace = rectDraw.Width() / 20;
	INT nHinterval = nVspace / 4;   //矩形框垂直间隔
	INT nHMargin = nHleftspace / 2;   //矩形框内文字与边缘的距离
	INT nHSubMargin = nHleftspace / 2;  //子项目的缩进量
	INT n1 = nXoffset + nHleftspace + nHMargin;  //左边第一项的起始X坐标
	INT n2 = n1 + (rectDraw.Width() - 2 * (n1 - nXoffset)) / 3;   //中间项的起始X坐标
	INT n3 = n2 + (rectDraw.Width() - 2 * (n1 - nXoffset)) / 3;   //最右边项的起始X坐标
	INT nBottom = 0;   //保存矩形框的下边纵坐标
	CRect textRect;
	CString strText , strTemp;	

	//标题	
	CFont* pOldFont = pDC->SelectObject (&m_fontPrinter);
	CSize sizeText = pDC->GetTextExtent(m_stPrintDetail.strTitle);
	textRect.SetRect(nXoffset + rectDraw.Width() / 2 - sizeText.cx / 2, nCurHeight-nVspace ,nXoffset + rectDraw.Width() / 2 + sizeText.cx / 2 ,  nCurHeight-nVspace + sizeText.cy );
	pDC->DrawText(m_stPrintDetail.strTitle, textRect, DT_CENTER);  //写报表抬头
	nBottom = textRect.bottom;
	
	//以下写第一行	
	{
		pDC->SelectObject (&m_dataFont);
		nCurHeight = nBottom + nVspace; 		
		sizeText = pDC->GetTextExtent(m_stPrintDetail.strLogo);
		nBottom = nCurHeight + sizeText.cy;
		textRect.SetRect(nXoffset + nHleftspace, nCurHeight, nXoffset + nHleftspace + sizeText.cx, nCurHeight + sizeText.cy);
		if(m_bPrintLogo)
			pDC->DrawText(m_stPrintDetail.strLogo, textRect, DT_LEFT);  //写软件版本及光维版权
		CTime tm = CTime::GetCurrentTime();
		CString strCurTime = tm.Format(_T("%Y-%m-%d %H:%M:%S"));
		sizeText = pDC->GetTextExtent(strCurTime);
		textRect.SetRect(nXoffset + rectDraw.Width() - nHleftspace - sizeText.cx ,nCurHeight, nXoffset + rectDraw.Width() - nHleftspace, nCurHeight + sizeText.cy);
		pDC->DrawText(strCurTime, textRect, DT_LEFT);  //写当前打印的时刻
	}
	
	TEXTMETRIC  tm;
	pDC->GetTextMetrics(&tm);
	INT nCharWidth = tm.tmMaxCharWidth;  //最大字符宽度
	strText = _T("Test char height 测试字体");
	sizeText = pDC->GetTextExtent(strText);
	INT nCharHeight = sizeText.cy;    //内容字体的高度
	INT nLineInterval = nCharHeight / 2 ;  //0.5倍行距
    //以下写任务信息内容
	if(m_bPrintQuest)
	{
		DrawQuestInfo(pDC,rectDraw,nBottom,nCurSel);
	}
	
    //以下画配置信息
	if(m_bPrintConfig)
	{
		DrawConfig(pDC,rectDraw,nBottom,nCurSel);
#if 0
		nCurHeight = nBottom + nVspace / 2;	
		nBottom = nCurHeight + nCharHeight * 7 + nLineInterval * 8;
		pDC->Rectangle(nXoffset + nHleftspace, nCurHeight, nXoffset + rectDraw.Width() - nHleftspace, nBottom);
		pDC->SelectObject(&m_fontGroup);
		//strText = _T(" Configuration info ");
		strText = _T(" ") + m_stPrintDetail.stPrintConfig.strTitle + _T(" ");
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(nXoffset + (rectDraw.Width() - sizeText.cx) / 2, nCurHeight - sizeText.cy / 2, nXoffset +(rectDraw.Width() + sizeText.cx) / 2, nCurHeight + sizeText.cy / 2);
		pDC->DrawText(strText, textRect, DT_LEFT);
		pDC->SelectObject(&m_dataFont);	
		nCurHeight += nLineInterval;
		//strText = _T("Wavelength(nm): ") + m_stPrintDetail.stPrintConfig.Wavelength;  //写wavelength
		strText = m_stPrintDetail.stPrintConfig.strWavelength + _T("(nm): ") + m_stPrintDetail.stPrintConfig.Wavelength;  //写wavelength
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n1, nCurHeight, n1 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);		
		//strText = _T("Range(m): ") + m_stPrintDetail.stPrintConfig.Range;  //写Range
		strText = m_stPrintDetail.stPrintConfig.strRange + _T("(m): ") + m_stPrintDetail.stPrintConfig.Range;  //写Range
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2 , nCurHeight, n2 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("Backscatter coeff(dB): ") + m_stPrintDetail.stPrintConfig.BackScatter;  //写Backscatter coeff 3
		strText = m_stPrintDetail.stPrintConfig.strBackScatter + _T("(dB): ") + m_stPrintDetail.stPrintConfig.BackScatter;  //写Backscatter coeff 3
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n3 , nCurHeight, n3 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);

		nCurHeight = textRect.bottom + nLineInterval;
		//strText = _T("Pulse(ns): ") + m_stPrintDetail.stPrintConfig.Pulse;  //写Pulse
		strText = m_stPrintDetail.stPrintConfig.strPulse + _T("(ns): ") + m_stPrintDetail.stPrintConfig.Pulse;//写PULSE
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n1, nCurHeight, n1 + sizeText.cx , nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("Average: ") + m_stPrintDetail.stPrintConfig.Average;  //写Average counts
		strText = m_stPrintDetail.stPrintConfig.strAverage + _T(": ") + m_stPrintDetail.stPrintConfig.Average;  //写Average counts  
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2 , nCurHeight, n2 + sizeText.cx , nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("Loss threshold(dB): ") + m_stPrintDetail.stPrintConfig.LossThreshold;  //写LossThreshold 3
		strText = m_stPrintDetail.stPrintConfig.strLossThreshold + _T("(dB): ") + m_stPrintDetail.stPrintConfig.LossThreshold;  //写LossThreshold 3
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n3 , nCurHeight, n3 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		
		nCurHeight = textRect.bottom + nLineInterval;
		//strText = _T("ORIGIN");  //写ORIGIN 
		strText = m_stPrintDetail.stPrintConfig.strOrigin;//写起始点
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n1, nCurHeight, n1 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
        //strText = _T("END");  //写END 2
		strText = m_stPrintDetail.stPrintConfig.strEnd;  //写结束点
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2, nCurHeight, n2 + sizeText.cx , nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("Reflection threshold(dB): ") + m_stPrintDetail.stPrintConfig.RefThreshold;  //写Reflection threshold
		strText = m_stPrintDetail.stPrintConfig.strRefThreshold + _T("(dB): ") + m_stPrintDetail.stPrintConfig.RefThreshold;  //写Reflection threshold
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n3 , nCurHeight, n3 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);

		nCurHeight = textRect.bottom + nLineInterval;
		//strText = _T("Origin location: ") + m_stPrintDetail.stPrintConfig.OriLocation;  //写Origin location 1
		strText = m_stPrintDetail.stPrintConfig.strOriLocation + _T(": ") + m_stPrintDetail.stPrintConfig.OriLocation;  //写Origin location 1
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n1 + nHSubMargin, nCurHeight, n1 + nHSubMargin + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("End location: ") + m_stPrintDetail.stPrintConfig.EndLocation;  //写 End location 2
		strText = m_stPrintDetail.stPrintConfig.strEndLocation + _T(": ") + m_stPrintDetail.stPrintConfig.EndLocation;  //写 End location 2
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2 + nHSubMargin , nCurHeight, n2 + nHSubMargin + sizeText.cx , nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("End threshold(dB): ") + m_stPrintDetail.stPrintConfig.EndThreshold;  //写Fiber end threshold
		strText = m_stPrintDetail.stPrintConfig.strEndThreshold + _T("(dB): ") + m_stPrintDetail.stPrintConfig.EndThreshold;  //写Fiber end threshold
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n3 , nCurHeight, n3 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);

		nCurHeight = textRect.bottom + nLineInterval;
		//strText = _T("Origin cable: ") + m_stPrintDetail.stPrintConfig.OriCable;  //写Origin Cable
		strText = m_stPrintDetail.stPrintConfig.strOriCable + _T(": ") + m_stPrintDetail.stPrintConfig.OriCable;  //写Origin Cable
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n1 + nHSubMargin, nCurHeight, n1 + nHSubMargin + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("End cable: ") + m_stPrintDetail.stPrintConfig.EndCable;  //写End Cable
		strText = m_stPrintDetail.stPrintConfig.strEndCable + _T(": ") + m_stPrintDetail.stPrintConfig.EndCable;  //写End Cable
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2 + nHSubMargin , nCurHeight, n2 + nHSubMargin + sizeText.cx , nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("Index: ") + m_stPrintDetail.stPrintConfig.Index;  //写 折射率
		strText = m_stPrintDetail.stPrintConfig.strIndex + _T(": ") + m_stPrintDetail.stPrintConfig.Index;  //写 折射率
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n3, nCurHeight, n3 + sizeText.cx , nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);

		nCurHeight = textRect.bottom + nLineInterval;
		//strText = _T("Origin fiber: ") + m_stPrintDetail.stPrintConfig.OriFiber;  //写Origin Fiber
		strText = m_stPrintDetail.stPrintConfig.strOriFiber + _T(": ") + m_stPrintDetail.stPrintConfig.OriFiber;  //写Origin Fiber
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n1 + nHSubMargin, nCurHeight, n1 + nHSubMargin + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("End fiber: ") + m_stPrintDetail.stPrintConfig.EndFiber;  //写 End Fiber
		strText = m_stPrintDetail.stPrintConfig.strEndFiber + _T(": ") + m_stPrintDetail.stPrintConfig.EndFiber;  //写 End Fiber
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2 + nHSubMargin, nCurHeight, n2 + nHSubMargin + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);

		nCurHeight = textRect.bottom + nLineInterval;
		//strText = _T("Origin color: ") + m_stPrintDetail.stPrintConfig.OriColor;  //写Origin Color
		strText = m_stPrintDetail.stPrintConfig.strOriColor + _T(": ") + m_stPrintDetail.stPrintConfig.OriColor;  //写Origin Color		
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n1 + nHSubMargin, nCurHeight, n1 + nHSubMargin + sizeText.cx, nCurHeight +sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		//strText = _T("End color: ") + m_stPrintDetail.stPrintConfig.EndColor;  //写 End color
		strText = m_stPrintDetail.stPrintConfig.strEndColor + _T(": ") + m_stPrintDetail.stPrintConfig.EndColor;  //写 End color		
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2 + nHSubMargin, nCurHeight, n2 + nHSubMargin + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);		
#endif
	}

#if 1
	//以下画Comment
	if(m_bPrintComment && bSingle)
	{
		//TEXTMETRIC  tm;
		//pDC->GetTextMetrics(&tm);
		INT cxChar = tm.tmAveCharWidth * 1.5;
		INT cyChar = tm.tmHeight;
		INT nOneLine = (rectDraw.Width() - nHleftspace * 2 - nHMargin * 2) / cxChar;  //每行多少字符
		INT nchars = m_stPrintDetail.stPrintQuest.Comment.GetLength();
		sizeText = pDC->GetTextExtent( m_stPrintDetail.stPrintQuest.Comment);
		INT nLines = sizeText.cx / (rectDraw.Width() - n1 * 2 );  //共有多少行
		INT nRemain = sizeText.cx % (rectDraw.Width() - n1 * 2 ); //余数
		nLines = nRemain > 0 ? ++nLines : nLines;
		
		nCurHeight = nBottom + nVspace / 2;
		nBottom = nCurHeight + nCharHeight * nLines + nLineInterval * (nLines + 1);
		pDC->Rectangle(nXoffset + nHleftspace, nCurHeight, nXoffset + rectDraw.Width() - nHleftspace, nBottom);
		pDC->SelectObject(&m_fontGroup);
		//strText = _T(" Comment ");
		strText = _T(" ") + m_stPrintDetail.stPrintQuest.strComment + _T(" ");
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(nXoffset + (rectDraw.Width() - sizeText.cx) / 2, nCurHeight - sizeText.cy / 2, nXoffset + (rectDraw.Width() + sizeText.cx) / 2, nCurHeight + sizeText.cy / 2);
		pDC->DrawText(strText, textRect, DT_LEFT);
		pDC->SelectObject(&m_dataFont);
		strTemp = m_stPrintDetail.stPrintQuest.Comment;
		INT nCharCount = 0; //显示的字符的个数
		strText.Empty();
		for(INT i=0; i<nchars; i++)
		{
			strText += strTemp.GetAt(i);
			sizeText = pDC->GetTextExtent(strText);
			if((sizeText.cx >= rectDraw.Width() - (n1-nXoffset) * 2 ) || i == nchars - 1)  //写一行
			{
				nCurHeight += nLineInterval;
                textRect.SetRect(n1, nCurHeight, n1 + sizeText.cx, nCurHeight  + sizeText.cy);
                pDC->DrawText(strText, textRect, DT_LEFT); 
                nCurHeight += nCharHeight;
				strText.Empty();
			}
		}
    }
#endif

	//曲线
	if(m_bPrintTrace)
	{
		nCurHeight = nBottom + nVspace / 2;	
		nBottom = nCurHeight + nVspace * 24;
		pDC->SelectObject(&m_fontGroup);
		//strText = _T(" Trace ");
		strText = _T(" ") + m_stPrintDetail.stPrintTrace.strTitle + _T(" ");
		sizeText = pDC->GetTextExtent(strText);
		pDC->Rectangle(nXoffset + nHleftspace, nCurHeight , nXoffset + rectDraw.Width() - nHleftspace, nBottom);
		textRect.SetRect(nXoffset + (rectDraw.Width() - sizeText.cx) / 2, nCurHeight - sizeText.cy /2, nXoffset + (rectDraw.Width() + sizeText.cx) / 2, nCurHeight + sizeText.cy /2);
		pDC->DrawText(strText, textRect, DT_LEFT);
		pDC->SelectObject(&m_dataFont);
		
		//大矩形
		nCurHeight += nHMargin + nLineInterval;
		CRect curveRect;
		curveRect.SetRect(nXoffset + nHleftspace*1.5 + nHmidspace/2, nCurHeight, nXoffset + rectDraw.Width() - 1.5 * nHleftspace - nHmidspace/2, nBottom - nVspace * 1.5);
		DrawTrace(pDC, curveRect, nCurSel);
	}

#if 0
	//曲线参数	
	if(m_bPrintTrace && bSingle)
	{
		pDC->SelectObject(&m_dataFont);
		strText = _T("A: ") + m_stPrintDetail.stPrintTrace.LocationA; // + _T(" m");  //写A标位置
		sizeText = pDC->GetTextExtent(strText);
		nCurHeight = nBottom + nVspace / 4;
		nBottom = nCurHeight + nLineInterval * 4  + nCharHeight * 3 ;		
		pDC->Rectangle(nXoffset + nHleftspace, nCurHeight, nXoffset + rectDraw.Width() - nHleftspace, nBottom);        
		
		nCurHeight += nLineInterval;
		textRect.SetRect(n1, nCurHeight, n1 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		strText = _T("B: ") + m_stPrintDetail.stPrintTrace.LocationB; //+ _T(" m");;  //写B标位置
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2 , nCurHeight, n2 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		strText = _T("A-B: ") + m_stPrintDetail.stPrintTrace.DistanceAB + _T("km");;  //写AB标距离
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n3 , nCurHeight, n3 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		nCurHeight = textRect.bottom + nLineInterval;
		strText = _T("    ") + m_stPrintDetail.stPrintTrace.LossA; //+ _T(" dB");;  //写A标损耗
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n1 , nCurHeight, n1 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		strText = _T("    ") + m_stPrintDetail.stPrintTrace.LossB; //+ _T(" dB");;  //写B标损耗
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n2 , nCurHeight, n2 + sizeText.cx , nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		strText = _T("        ") + m_stPrintDetail.stPrintTrace.LossAB + _T("dB");;  //写AB标损耗差
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n3 , nCurHeight, n3 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
		nCurHeight = textRect.bottom + nLineInterval;		
		strText = _T("        ") + m_stPrintDetail.stPrintTrace.SlopeAB + _T("dB/km");;  //写AB段损耗斜率
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(n3 , nCurHeight, n3 + sizeText.cx, nCurHeight + sizeText.cy);
		pDC->DrawText(strText, textRect, DT_LEFT);
	}
#endif

	//事件列表
	if(m_bPrintEvent)
	{
		nCurHeight = nBottom + nVspace / 2;
		//绘制事件列表表头
		DrawEventHead(pDC, nXoffset + nHleftspace, nCurHeight + nLineInterval, rectDraw,nCurSel);
		//绘制事件体
		if(nCurSel != SHOW_ALL) //为了区分单曲线与多曲线，此处改为根据自定义曲线编号来显示事件。lzy2014.8.30
			DrawEventList(pDC, nXoffset + nHleftspace, nCurHeight + nLineInterval, nLineInterval , rectDraw, m_stPrintDetail.curSelection);
		else
			DrawEventList2Way(pDC, nXoffset + nHleftspace, nCurHeight + nLineInterval, nLineInterval , rectDraw, nCurSel);

        
		//画矩形框
		pDC->SelectObject(&m_dataFont);
		//strText = _T(" Event info ");
		strText = _T(" ") + m_stPrintDetail.stPrintEvent.strTitle + _T(" ");
		sizeText = pDC->GetTextExtent(strText);
		nBottom =  nCurHeight + (m_stPrintDetail.stPrintEvent.EventCount + 1) * (nLineInterval + nCharHeight) + nLineInterval;
		textRect.SetRect(nXoffset + nHleftspace , nCurHeight,nXoffset + rectDraw.Width() - nHleftspace, nBottom);
		CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		CBrush *pOldBrush = pDC->SelectObject(pBrush);
		pDC->Rectangle(textRect);  //画矩形框
		pDC->SelectObject(pOldBrush);
        
		//写title
		pDC->SelectObject(&m_fontGroup);
		sizeText = pDC->GetTextExtent(strText);
		textRect.SetRect(nXoffset + (rectDraw.Width() - sizeText.cx) / 2, nCurHeight - sizeText.cy /2, nXoffset+(rectDraw.Width() + sizeText.cx) / 2, nCurHeight + sizeText.cy /2);
		pDC->DrawText(strText, textRect, DT_LEFT);

		//画横线
		for(INT i = 1; i<m_stPrintDetail.stPrintEvent.EventCount + 1; i++)
		{		
			pDC->MoveTo(nXoffset + nHleftspace , nCurHeight + nLineInterval / 2 + i * (nLineInterval + nCharHeight));
			pDC->LineTo(rectDraw.right - nHleftspace, nCurHeight + nLineInterval / 2 + i * (nLineInterval + nCharHeight));
		}
	}	

	pDC->SelectObject (pOldFont);
}

//绘制带坐标曲线，带有AB标线 lzy2014.8.26
void CMyView::DrawTrace(CDC *pDC, CRect rc, INT nCurSel, float fontRatio)
{	
	float fLength=0;
	float fOffsetX=0;

	InitFont(pDC);  
	pDC->SelectObject(&m_dataFont);
	pDC->Rectangle(&rc);//大矩形

	DrawGridLine(pDC, rc);//绘制栅格 

	if(nCurSel!=SHOW_ALL)
	{
        fLength=m_stPrintDetail.stTestResultInfo->Length;
	    DrawScale(pDC, rc, 0, fLength);//绘制刻度
	    DrawCurve(pDC, rc, nCurSel);//绘制曲线
		DrawEvent(pDC, rc, nCurSel);//绘制事件
	}
	else
	{
  	    //获取双向测试的事件
	    //GetEvent2Way(&m_stPrintDetail.stTestResultInfo[2], &m_stPrintDetail.stTestResultInfo[0], &m_stPrintDetail.stTestResultInfo[1]);
		DrawCurve2Way(pDC, rc, nCurSel);
        fLength=m_stPrintDetail.stTestResultInfo[2].Length;
		fOffsetX=m_stPrintDetail.stTestResultInfo[2].YOffset;	
	    DrawScale2Way(pDC, rc, fOffsetX, fLength);
	}
	//绘制光标AB
	DrawCursorAB(pDC, rc);  //lzy2014.7.23
}

//初始化字体 lzy2014.9.3
void CMyView::InitFont(CDC *pDC)
{
	m_fontPrinter.DeleteObject();
	m_dataFont.DeleteObject();
	m_fontGroup.DeleteObject();
	m_FontVR.DeleteObject();

	//获取打印逻辑尺寸 lzy2014.8.15
	INT nLogicX = pDC->GetDeviceCaps(LOGPIXELSX);
	INT nLogicY = pDC->GetDeviceCaps(LOGPIXELSY);
	//获取物理打印尺寸
	INT nPhysicalX = pDC->GetDeviceCaps(PHYSICALWIDTH);
	INT nPhysicalY = pDC->GetDeviceCaps(PHYSICALHEIGHT);
	//CString str;
	//str.Format(L"lx = %d;ly=%d;px=%d;py=%d", nLogicX, nLogicY, nPhysicalX, nPhysicalY);
	//	AfxMessageBox(str);
	float fScale = nLogicX / 300.0;
	INT nCurveNumber = _wtoi(m_stPrintDetail.WaveList);  //当前选择每页打印的曲线数
	if(nCurveNumber > 4 || nCurveNumber < 0 || m_bPrintCurve)
		nCurveNumber = 0;

	if(m_fontPrinter.m_hObject==NULL)
	{
		m_fontPrinter.CreateFont(54 * fScale * FONTSIZE_RATIO[nCurveNumber], 0, 0, 0, FW_BOLD,//从上往下2700
			0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH,_T("Arial")); 
	}
	
	if(m_dataFont.m_hObject==NULL)
	{
		m_dataFont.CreateFont(36 * fScale * FONTSIZE_RATIO[nCurveNumber], 0, 0, 0, FW_NORMAL,//从上往下2700
			0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH,_T("Arial")); 
	}
	//m_dataFont.CreatePointFont(80,_T("宋体"),pDC);
	if(m_fontGroup.m_hObject == NULL)
	{
		m_fontGroup.CreateFont(36 * fScale * FONTSIZE_RATIO[nCurveNumber], 0, 0, 0, FW_BOLD,//从上往下2700
			0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH,_T("Arial")); 
	}
	//m_fontGroup.CreatePointFont(70,_T("宋体"),pDC);
	if(m_FontVR.m_hObject==NULL)
		m_FontVR.CreateFont(32 * fScale * FONTSIZE_RATIO[nCurveNumber], 0, 900, 900, FW_NORMAL,//从上往下2700
		0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH,_T("Arial"));	
}

//绘制刻度lzy2014.7.17
void CMyView::DrawScale(CDC *pDC, CRect &rect, float fYmax, float fXmax)
{
	if (m_stPrintDetail.stWaveConfig.ShowGrid)
	{
		//float flo_Length = fXmax;
		INT ndx = rect.Width () / COOR_SPLIT_COUNT_H;
		INT ndy = rect.Height () / COOR_SPLIT_COUNT_V;
		CString strText;
		CSize sizeText;
		CRect rectText;

		INT iYmin = 0;//M_FMAXDB0;
		INT iYmax = M_FMAXDB0 - M_FSTARTDB0;
		INT itemp;
		INT bottom = rect.bottom;
		INT left = rect.left;
		INT iOffset = min(10, rect.Height() / 20);
				
		//横坐标刻度
		rectText.top = rect.bottom + iOffset;		
		for ( INT i = 0; i< COOR_SPLIT_COUNT_H; i++)
		{			
			itemp = left + ndx * i;
			strText.Format(_T("%.1f"), 0.001 * fXmax * i / COOR_SPLIT_COUNT_H);
			sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
			rectText.left = itemp - sizeText.cx / 2;
			rectText.right = rectText.left + sizeText.cx;
			rectText.bottom = rectText.top + sizeText.cy;			
			pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);			
		}
		strText = _T("km");
		sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
		rectText.right = rect.right - 10;
		rectText.left = rectText.right - sizeText.cx;		
		pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);

		//纵坐标刻度
		//pDC->SetTextAlign(TA_LEFT | TA_TOP);
		strText = _T("0000");
		sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
		INT iText = sizeText.cx;  //文字宽度
		for (INT i=1; i<COOR_SPLIT_COUNT_V; i++)
		{
			//画Y轴刻度
			if(iYmax >= 0 && iYmax <= 100)
			{
				rectText.left = rect.left - iText - 5;		

				for(INT i = 0; i < COOR_SPLIT_COUNT_V; i++)
				{
					itemp = bottom - ndy * i;
					strText.Format(_T("%.1f"), (M_FMAXDB0 - M_FSTARTDB0) * i / COUNT_GRIDLINE_VERTICAL);
					sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
					rectText.top = itemp - sizeText.cy / 2;
					rectText.bottom = rectText.top + sizeText.cy;
					rectText.right = rectText.left + sizeText.cx;
					pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
				}

				strText = _T("dB");
				sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
				rectText.top = rect.top + 5;
				rectText.bottom = rectText.top + sizeText.cy;
				rectText.right = rectText.left + sizeText.cx;
				pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
			}
		}
	}
}

//ghq.2016.4.26
void CMyView::DrawScale2Way(CDC *pDC, CRect &rect, float fXmin, float fXmax)
{
	if (m_stPrintDetail.stWaveConfig.ShowGrid)
	{
		//float flo_Length = fXmax;
		INT ndx = rect.Width () / COOR_SPLIT_COUNT_H;
		INT ndy = rect.Height () / COOR_SPLIT_COUNT_V;
		CString strText;
		CSize sizeText;
		CRect rectText;

		INT iYmin = 0;//M_FMAXDB0;
		INT iYmax = M_FMAXDB0 - M_FSTARTDB0;
		INT itemp;
		INT bottom = rect.bottom;
		INT left = rect.left;
		INT iOffset = min(10, rect.Height() / 20);
				
		//横坐标刻度
		rectText.top = rect.bottom + iOffset;		
		for ( INT i = 0; i< COOR_SPLIT_COUNT_H; i++)
		{			
			itemp = left + ndx * i;
			strText.Format(_T("%.1f"), 0.001 * fXmax * i / COOR_SPLIT_COUNT_H -0.001 * fXmin);
			sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
			rectText.left = itemp - sizeText.cx / 2;
			rectText.right = rectText.left + sizeText.cx;
			rectText.bottom = rectText.top + sizeText.cy;			
			pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);			
		}
		strText = _T("km");
		sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
		rectText.right = rect.right - 10;
		rectText.left = rectText.right - sizeText.cx;		
		pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);

		//纵坐标刻度
		//pDC->SetTextAlign(TA_LEFT | TA_TOP);
		strText = _T("0000");
		sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
		INT iText = sizeText.cx;  //文字宽度
		for (INT i=1; i<COOR_SPLIT_COUNT_V; i++)
		{
			//画Y轴刻度
			if(iYmax >= 0 && iYmax <= 100)
			{
				rectText.left = rect.left - iText - 5;		

				for(INT i = 0; i < COOR_SPLIT_COUNT_V; i++)
				{
					itemp = bottom - ndy * i;
					strText.Format(_T("%.1f"), (M_FMAXDB0 - M_FSTARTDB0) * i / COUNT_GRIDLINE_VERTICAL);
					sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
					rectText.top = itemp - sizeText.cy / 2;
					rectText.bottom = rectText.top + sizeText.cy;
					rectText.right = rectText.left + sizeText.cx;
					pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
				}

				strText = _T("dB");
				sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
				rectText.top = rect.top + 5;
				rectText.bottom = rectText.top + sizeText.cy;
				rectText.right = rectText.left + sizeText.cx;
				pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
			}
		}
	}
}

void CMyView::DrawEventHead(CDC* pDC, INT nLeft, INT nTop, CRect rectDraw,INT nCurSel)//绘制事件列表
{
	CFont* pOldFont = pDC->SelectObject (&m_dataFont);
	//绘制标题栏
	LV_COLUMN lvColumn;
	TCHAR uin[50];
	lvColumn.pszText=uin;
	lvColumn.cchTextMax=50;
	lvColumn.mask=LVCF_TEXT;
	CString header=_T("");
	INT nLen = 0;//得到列数
	INT nVspace = 0;
	INT nLeftSpace = 0;
	INT nHspace = 0;  //lzy2014.8.12
	CRect textRect;

	if(nCurSel == SHOW_ALL)
	{
		nLen = EVENT_LIST_COL;//得到列数
	}
	else
	{
		nLen = EVENT_LIST_COL-1;//得到列数
	}

	m_stPrintDetail.stPrintEvent.EventColumn = nLen;   //保存列数
	nVspace = rectDraw.Height() / 42;
	nLeftSpace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	nHspace = ( rectDraw.Width() - 2 * nLeftSpace)  / nLen;  //lzy2014.8.12
	for(INT i=0;i<nLen;i++)
	{
		textRect.left = nLeft + i * nHspace;
		textRect.top = nTop;
		textRect.right = nLeft + (i+1) * nHspace;
		textRect.bottom = nTop + nVspace;
		//if(m_dlgParent->m_pEventFormView->m_Eventlist.GetColumn(i,&lvColumn))

		if(nCurSel == SHOW_ALL)
		    header=strListHead2Way[i];
		else
			header=strListHead[i];
		pDC->DrawText(header, &textRect,DT_CENTER);//文字
		TRACE(_T("left=%d;right=%d;offset=%d\n"), textRect.left, textRect.right, nLeft);
	}
	pDC->SelectObject (pOldFont);
}

/****************************************************************
函数名称:             DrawEventList()                                                                               
函数说明:             画曲线事件列表体                                                                             
入口参数:             nLeft 左起点， nTop 上起点                                                                   
出口参数:             无                                                                                        
建立时间:             2011-07-24                                                                  
Write By:             XJF                                                                                          
****************************************************************/
void CMyView::DrawEventList(CDC* pDC, INT nLeft, INT nTop , INT nVspace , CRect rectDraw, INT nCurSel)//绘制事件列表
{
	CFont* pOldFont = pDC->SelectObject (&m_dataFont);
	INT nLeftSpace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	INT nHspace = (rectDraw.Width() - 2 * nLeftSpace) / (EVENT_LIST_COL-1);  //lzy2014.8.12
	INT nStartLine = 1;

	float preDistance=0;

	//TestResultInfo* pData = g_sorFileArray.sorFileList[g_sorFileArray.curSelection].stTestResultInfo;
	TestResultInfo* pData = &m_stPrintDetail.stTestResultInfo[nCurSel];  //lzy2014.8.30

	if (NULL == pData)
		return;

	CString strText=_T("Test Height");
	CSize sizeText = pDC->GetTextExtent(strText);
	INT nCharHeight = sizeText.cy;  //获取字体高度
	CRect textRect;
	std::vector<EventInfo> ve = pData->EventList.vEventInfo;//速度??
	INT nSize=0;
	    
	if(ve.size()>EVENT_LIST_PRINT_MAX)
		nSize = EVENT_LIST_PRINT_MAX;
	else
		nSize = ve.size();

	m_stPrintDetail.stPrintEvent.EventCount = nSize;   //保存行数
	for (INT i=0;i<nSize;i++)//
	{
		// 序号
		{
			textRect.left = nLeft;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace; lzy 2014.8.15

			//类型
			strText.Format(_T("%d"),i+1);
			if (g_iLang == CHS)
			{
				if (0 == ve[i].nEventType)
				{
					strText += _T(" 非反射");//0代表非反射事件符号
				}
				else if (1 == ve[i].nEventType)
				{
					strText += _T(" 反射");;//1代表反射事件符号
				}
			}
			else if (g_iLang == ENG)
			{
				if (0 == ve[i].nEventType)
				{
					strText += _T(" NoReflect");//0代表非反射事件符号
				}
				else if (1 == ve[i].nEventType)
				{
					strText += _T(" Reflect");;//1代表反射事件符号
				}
			}

			if (_T('S')==ve[i].cEventType)
			{
				strText+=_T("(S)");
			}
			else if (_T('E')==ve[i].cEventType)
			{
				strText+=_T("(E)");
			}
			else if (_T('A')==ve[i].cEventType)
			{
				strText+=_T("(A)");
			}
			else if (_T('F')==ve[i].cEventType)
			{
				strText+=_T("(F)");
			}
			else if (_T('O')==ve[i].cEventType)
			{
				strText+=_T("(O)");
			}

			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 事件类型
		//{
		//	textRect.left = nLeft + nHspace;
		//	textRect.top = nTop + nStartLine * nVspace;
		//	textRect.right = nLeft + 2 * nHspace;
		//	textRect.bottom = nTop + (nStartLine+1) * nVspace;

		//	//类型
		//	if (0 == ve[i].nEventType)
		//	{
		//		strText = _T("开始事件");
		//	}
		//	else if (1 == ve[i].nEventType)
		//	{
		//		strText = _T("反射事件");
		//	}
		//	else if (2 == ve[i].nEventType)
		//	{
		//		strText = _T("非反射事件");
		//	}
		//	else if (3 == ve[i].nEventType)
		//	{
		//		strText = _T("反射事件");
		//	}
		//	else if (4 == ve[i].nEventType)
		//	{
		//		strText = _T("结束事件");
		//	}
		//	else
		//	{
		//		strText = _T("--");
		//	}
		//	pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		//}
		// 距离
		{
			textRect.left = nLeft + nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 2 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			float m_float_zll=0.0f;
			if (0==i)
			{
				strText.Format(_T("%.3f"),0.0f);//距离
			}
			else
			{
				//if(_T("m")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为km
					strText.Format(_T("%.3f"),ve[i].fLocation/1000);//距离
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 段
		{
			textRect.left = nLeft + 2 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 3 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			float a,b=0.0f;
			if (0==i)
			{
				strText.Format(_T("%.3f"),0.0f);//段
			}
			else
			{
				//if(_T("m")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为km
					strText.Format(_T("%.3f"),(ve[i].fLocation-preDistance)/1000);//段,两事件点间距离
			}
			if (0==i)
			{
				preDistance = 0.0f;
			}
			else
			{
				preDistance = ve[i].fLocation;
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 损耗
		{
			textRect.left = nLeft + 3 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 4 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].fEventloss)//为0时显示"-"wcq2012.07.09
			{
				strText.Format(_T("--"));//总损耗
			}
			else
			{
				strText.Format(_T("%.3f"), ve[i].fEventloss);//损耗
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 总损耗
		{
			textRect.left = nLeft + 4 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 5 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].fTotalLoss)//为0时显示"-"wcq2012.07.09
			{
				strText.Format(_T("--"));//总损耗
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fTotalLoss);//总损耗
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 斜率
		{
			textRect.left = nLeft + 5 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 6 * nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace;
			//斜率
			if (fabs(ve[i].fLineK)>0.5)//zll 2012.10.31
			{
				strText.Format(_T("--"));
			}
			else if (0 == ve[i].fLineK)//为0时显示"-" zll 2012.10.31
			{
				strText.Format(_T("--"));
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fLineK);//斜率
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 反射
		{
			textRect.left = nLeft + 6 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 7 * nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].nEventType)//反射
			{
				strText=_T("--");
			}
			else if (0 == ve[i].fEventHigh)//为0时显示"-"wcq2012.07.09
			{
				strText=_T("--");
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fEventHigh);//反射
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		nStartLine++; //行数加1
	}
	pDC->SelectObject (pOldFont);
}
/****************************************************************
函数名称:             DrawEventList2Way()                                                                               
函数说明:             画双向曲线事件列表体                                                                             
入口参数:             nLeft 左起点， nTop 上起点                                                                   
出口参数:             无                                                                                        
建立时间:             2016-04-26                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawEventList2Way(CDC* pDC, INT nLeft, INT nTop , INT nVspace , CRect rectDraw, INT nCurSel)//绘制事件列表
{
	CFont* pOldFont = pDC->SelectObject (&m_dataFont);
	INT nLeftSpace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	INT nHspace = (rectDraw.Width() - 2 * nLeftSpace) / EVENT_LIST_COL;  //lzy2014.8.12
	INT nStartLine = 1;

	float preDistance=0;
	float fLossAvg = 0;
	TestResultInfo* pData = &m_stPrintDetail.stTestResultInfo[nCurSel];  //lzy2014.8.30

	if (NULL == pData)
		return;

	CString strText=_T("Test Height");
	CSize sizeText = pDC->GetTextExtent(strText);
	INT nCharHeight = sizeText.cy;  //获取字体高度
	CRect textRect;
	std::vector<EventInfo> ve = pData->EventList.vEventInfo;//速度??
	INT nSize=0;
	    
	if(ve.size()>EVENT_LIST_PRINT_MAX)
		nSize = EVENT_LIST_PRINT_MAX;
	else
		nSize = ve.size();

	m_stPrintDetail.stPrintEvent.EventCount = nSize;   //保存行数

	for (INT i=0;i<nSize;i++)//
	{
		// 序号
		{
			textRect.left = nLeft;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace; lzy 2014.8.15

			//类型
			strText.Format(_T("%d"),i+1);
			if (g_iLang == CHS)
			{
				if (0 == ve[i].nEventType)
				{
					strText += _T(" 非反射");//0代表非反射事件符号
				}
				else if (1 == ve[i].nEventType)
				{
					strText += _T(" 反射");;//1代表反射事件符号
				}
			}
			else if (g_iLang == ENG)
			{
				if (0 == ve[i].nEventType)
				{
					strText += _T(" NoReflect");//0代表非反射事件符号
				}
				else if (1 == ve[i].nEventType)
				{
					strText += _T(" Reflect");;//1代表反射事件符号
				}
			}

			if (_T('S')==ve[i].cEventType)
			{
				strText+=_T("(S)");
			}
			else if (_T('E')==ve[i].cEventType)
			{
				strText+=_T("(E)");
			}
			else if (_T('A')==ve[i].cEventType)
			{
				strText+=_T("(A)");
			}
			else if (_T('F')==ve[i].cEventType)
			{
				strText+=_T("(F)");
			}
			else if (_T('O')==ve[i].cEventType)
			{
				strText+=_T("(O)");
			}

			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		{
			textRect.left = nLeft + nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 2 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			float m_float_zll=0.0f;
			if (0==i)
			{
				strText.Format(_T("%.3f"),0.0f);//距离
			}
			else
			{
				//if(_T("m")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为km
				strText.Format(_T("%.3f"),ve[i].fLocation/1000);//距离
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		//strText.Format(_T("%.3f"), 0.0);
		strText.Format(_T("--"));//为0时显示"-"

        //以下获取A->B LOSS
		textRect.left = nLeft + 2 * nHspace;
		textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
		textRect.right = nLeft + 3 * nHspace;
		textRect.bottom = textRect.top + nCharHeight;

		GetEventInfo2Way(i,strText,0);
		pDC->DrawText(strText, &textRect,DT_CENTER);//文字

		//strText.Format(_T("%.3f"), 0.0);
		strText.Format(_T("--"));//为0时显示"-"

		//以下获取B->A LOSS
		textRect.left = nLeft + 3 * nHspace;
		textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
		textRect.right = nLeft + 4 * nHspace;
		textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;

		GetEventInfo2Way(i,strText,1);
		pDC->DrawText(strText, &textRect,DT_CENTER);//文字

		//以下给出平均损耗
		textRect.left = nLeft + 4 * nHspace;
		textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
		textRect.right = nLeft + 5 * nHspace;
		textRect.bottom = textRect.top + nCharHeight;

		GetEventInfo2Way(i,strText,2);
		pDC->DrawText(strText, &textRect,DT_CENTER);//文字

		// 总损耗
		{
			textRect.left = nLeft + 5 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 6 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].fTotalLoss)//为0时显示"-"wcq2012.07.09
			{
				strText.Format(_T("--"));//总损耗
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fTotalLoss);//总损耗
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 斜率
		{
			textRect.left = nLeft + 6 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 7 * nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace;
			//斜率
			if (fabs(ve[i].fLineK)>0.5)//zll 2012.10.31
			{
				strText.Format(_T("--"));
			}
			else if (0 == ve[i].fLineK)//为0时显示"-" zll 2012.10.31
			{
				strText.Format(_T("--"));
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fLineK);//斜率
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 反射
		{
			textRect.left = nLeft + 7 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 8 * nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].nEventType)//反射
			{
				strText=_T("--");
			}
			else if (0 == ve[i].fEventHigh)//为0时显示"-"wcq2012.07.09
			{
				strText=_T("--");
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fEventHigh);//反射
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		nStartLine++; //行数加1
	}
	pDC->SelectObject (pOldFont);
}

/****************************************************************
函数名称:             DrawCurve()                                                                               
函数说明:             画测量曲线                                                                              
入口参数:             CDC                                                                                           
出口参数:             无                                                                                        
建立时间:             2011-07-24                                                                  
Write By:             XJF                                                                                          
****************************************************************/
void CMyView::DrawCurve(CDC* pDC, CRect& rect, INT nCurveIndex)
{
	TestResultInfo* pData = &m_stPrintDetail.stTestResultInfo[nCurveIndex];
	if (pData == NULL ||
		pData->DataPointsNum < NUM_VALIDE_LEAST ||
		CLEAR_DATAPOINTS_NUM == pData->DataPointsNum)//没有或数据很少
		return;

	//转化为绘图像素坐标
	if (NULL==pData->ptDrawData)
	{
		pData->ptDrawData=new CPoint[pData->DataPointsNum];
	}
	pData->StartPoint=0;
    pData->MaxPoint=pData->DataPointsNum;
	for(INT i = pData->StartPoint; i < pData->MaxPoint; i++)
	{
		pData->ptDrawData[i].x = INT(rect.left + rect.Width () * (i - pData->StartPoint)/(pData->MaxPoint - pData->StartPoint));
		//pData->ptDrawData[i].y = INT(rect.bottom - (((-pData->pDataPoints[i]/1000.0) - M_FSTARTDB0)/(M_FMAXDB0 - M_FSTARTDB0)) * rect.Height());
#ifdef COMPILE_SHOW_SMOOTH_DATA////显示时用平滑过的数据，保存时用实际数据 2013.05.12
		pData->ptDrawData[i].y = INT(rect.bottom - (((-pData->pDataPointsSmooth[i]/1000.0) - M_FSTARTDB0)/(M_FMAXDB0 - M_FSTARTDB0)) * rect.Height());
#else
		//pData->ptDrawData[i].y = INT(rect.bottom - (((-pData->pDataPoints[i]/1000.0) - M_FSTARTDB0)/(M_FMAXDB0 - M_FSTARTDB0)) * rect.Height() - 150);//zll 2013.6.3 -150 使曲线沿y轴向上移动
		pData->ptDrawData[i].y = INT(rect.bottom - (((-pData->pDataPoints[i]/1000.0) - M_FSTARTDB0)/(M_FMAXDB0 - M_FSTARTDB0)) * rect.Height());//lzy2014.7.18

#endif
		if(pData->ptDrawData[i].x > rect.right)
			pData->ptDrawData[i].x = rect.right;
		if(pData->ptDrawData[i].y < rect.top)
			pData->ptDrawData[i].y = rect.top;
		if(pData->ptDrawData[i].y > rect.bottom)
			pData->ptDrawData[i].y = rect.bottom; 
	}

	//画曲线
	pDC->Polyline(&pData->ptDrawData[pData->StartPoint], pData->MaxPoint - pData->StartPoint);
}
/****************************************************************
函数名称:             DrawEvent()                                                                               
函数说明:             画事件                                                                              
入口参数:             CDC                                                                                           
出口参数:             无                                                                                        
建立时间:             2016-04-26                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawEvent(CDC* pDC, CRect& rect, INT nCurveIndex)
{
	TestResultInfo* pData = &m_stPrintDetail.stTestResultInfo[nCurveIndex];
	if (pData == NULL ||
		pData->DataPointsNum < NUM_VALIDE_LEAST ||
		CLEAR_DATAPOINTS_NUM == pData->DataPointsNum)//没有或数据很少
		return;

	//绘事件点竖线及下方矩形框 zll 2013.5.23 晚
	INT fX,fY;
	INT prevfX = 0;//上一节点的位置
	INT offsetfx = 0; //两节点间偏距
	CRect rectTemp;
	rectTemp.SetRectEmpty();
	//TEXTMETRIC  tm;
	//pDC->GetTextMetrics(&tm);
	//INT nCharWidth = tm.tmMaxCharWidth;  //最大字符宽度
	for (UINT i=0; i<pData->EventList.vEventInfo.size(); i++)
	{		
		CRect rectLineBottom;
		CString temp;
		fX=INT(rect.left + pData->EventList.vEventInfo[i].fLocation*rect.Width()/pData->Length);
		//fY=INT(rect.bottom - (65535-pData->pDataPoints[pData->EventList.vEventInfo[i].index])/1000.0*rect.Height()/(g_nTraceYmax-OTDR_TRACE_Y_MIN) - 110);//zll 2013.6.3 -110 使事件点标示沿y轴向上移动
		fY=INT(rect.bottom - (65535-pData->pDataPoints[pData->EventList.vEventInfo[i].index])/1000.0*rect.Height()/(OTDR_TRACE_Y_MAX-OTDR_TRACE_Y_MIN));//lzy2014.7.23 使事件点文字不沿y轴移动

		if(i == 0)
		{
			prevfX = fX;
		}
		else
		{
			offsetfx = fX - prevfX;
			prevfX = fX;
		}
		//画竖直线
		INT nLineH = rect.Height() / 10; //竖线高度
		INT nSquare = nLineH / 5 ;  //小正方形的边长
		INT nLineTop = nLineH / 6;  //上起始点位置
		pDC->MoveTo(fX,fY- nLineTop);
		//pDC->LineTo(fX,fY+44+50);//zll 2013.6.3 +50 使事件点标示沿y轴向上移动
		pDC->LineTo(fX,fY+nLineH);//lzy2014.8.26 使事件点标示沿y轴向上移动
		//竖直线下面的正方形
		rectLineBottom.SetRect(fX - nSquare / 2,fY + nLineH,fX + nSquare / 2, fY + nLineH + nSquare);
		//rectLineBottom.SetRect(fX - 8-20,fY + 34+60,fX - 8 + 16+20, fY + 34 + 18+100);
		if (i>8)//两位数
		{
			rectLineBottom.right += 2;
		}
		//pDC->Rectangle(&rectLineBottom);
		//绘制衰减和相对距离、斜率
		CString strEventLosss;
		strEventLosss.Format(_T("%.3fdB"), pData->EventList.vEventInfo[i].fEventloss);
		CString strLocation;
		CString strLink;
		//距离文本
		float fValLocation = pData->EventList.vEventInfo[i].fLocation/DIVISOR_TOKM;
		float fValLink = pData->EventList.vEventInfo[i].fLineK;
		
		CFont* pOldPen = pDC->SelectObject(&m_FontVR);

		//计算要显示的字符串高度
		INT height = pDC->DrawText( strEventLosss, &rectLineBottom, DT_WORDBREAK | DT_CENTER | DT_CALCRECT );
		CSize sizeText = pDC->GetTextExtent(strEventLosss);
		//rectLineBottom.top += (rectLineBottom.Height() - height)/2;
		//损耗
		pDC->TextOut(fX - sizeText.cy, fY + nLineH +  rectLineBottom.Height() + sizeText.cx, strEventLosss);
		//pDC->TextOut(fX - 20, fY + rectLineBottom.Width() + 130, strEventLosss);
		//位置
		sizeText = pDC->GetTextExtent(strLocation);
		pDC->TextOut(fX , fY + nLineH + rectLineBottom.Height() + sizeText.cx, strLocation);
		//斜率
		//pDC->TextOut(fX + offsetfx / 2, fY + rectLineBottom.Width() + 300, strLink);//cwcq
		sizeText = pDC->GetTextExtent(strEventLosss);
		pDC->TextOut(fX - offsetfx / 2, fY + nLineH*2 + rectLineBottom.Height() + sizeText.cx, strLink);//lzy2014.8.26
		pDC->SelectObject(pOldPen);
	}
}
/****************************************************************
函数名称:             DrawCurve2Way()                                                                               
函数说明:             画双向测量曲线                                                                              
入口参数:             CDC                                                                                           
出口参数:             无                                                                                        
建立时间:             2016-04-26                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawCurve2Way(CDC* pDC, CRect& rect, INT nCurveIndex)
{
	UINT iEventLast = m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo.size() - 1;  //获取B2A线最后一个事件的位置
	INT nPointsStart = m_stPrintDetail.stTestResultInfo[1].DataPointsNum - m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo[iEventLast].index;  //获取A2B线的起始位置
	INT nPointsTotal = m_stPrintDetail.stTestResultInfo[0].DataPointsNum>m_stPrintDetail.stTestResultInfo[1].DataPointsNum?m_stPrintDetail.stTestResultInfo[0].DataPointsNum:m_stPrintDetail.stTestResultInfo[1].DataPointsNum + nPointsStart;   //数据总数
	m_stPrintDetail.stTestResultInfo[2].YOffset = m_stPrintDetail.stTestResultInfo[1].Length - m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo[iEventLast].fLocation;
	FLOAT fLengthTotal = m_stPrintDetail.stTestResultInfo[0].Length + m_stPrintDetail.stTestResultInfo[2].YOffset;
	
	WORD* pWordAB = m_stPrintDetail.stTestResultInfo[0].pDataPoints;
	WORD* pWordBA = m_stPrintDetail.stTestResultInfo[1].pDataPoints;
	std::vector<WORD> recordAB;//
	std::vector<WORD> recordBA;
	INT nPointsCountAB = m_stPrintDetail.stTestResultInfo[0].DataPointsNum;  //A到B的数据点数
	INT nPointsCountBA = m_stPrintDetail.stTestResultInfo[1].DataPointsNum;  //B到A的数据点数
	m_stPrintDetail.stTestResultInfo[2].Length = fLengthTotal;
	//先设置A2B线的数据
	for(INT j = 0; j < nPointsStart; j++) //A2B线前面一段为0值
	{
		recordAB.push_back(0);
	}
	for(INT j = nPointsStart; j < nPointsTotal; j++)  //A2B线后面一段数据为OTDR所测到的数据
	{
		recordAB.push_back(pWordAB[j - nPointsStart]);
	}

	//再设置B2A线的数据
	for(INT j = 0; j < nPointsCountBA; j++) //A2B线前面一段为0值
	{
		recordBA.push_back(pWordBA[nPointsCountBA - j]);  //数据前后倒一下
	}
	for(INT j = nPointsCountBA; j < nPointsTotal; j++)  //A2B线后面一段数据为OTDR所测到的数据
	{
		recordBA.push_back(0);
	}

	for(INT i=0;i<2;i++)
	{
		if(m_stPrintDetail.stTestResultInfo[2].pDataPoints)
		{
			delete []m_stPrintDetail.stTestResultInfo[2].pDataPoints;
			m_stPrintDetail.stTestResultInfo[2].pDataPoints = NULL;
		}
		m_stPrintDetail.stTestResultInfo[2].DataPointsNum = nPointsTotal;
		m_stPrintDetail.stTestResultInfo[2].pDataPoints = new WORD[nPointsTotal];
		if(i==0)
	        memcpy(m_stPrintDetail.stTestResultInfo[2].pDataPoints, &recordAB[0], nPointsTotal * sizeof(WORD));  //取出数据
		else
	        memcpy(m_stPrintDetail.stTestResultInfo[2].pDataPoints, &recordBA[0], nPointsTotal * sizeof(WORD));  //取出数据
		
		DrawCurve(pDC, rect, 2);
	}
	DrawEvent2Way(pDC, rect, nCurveIndex);

	for(INT i=0;i<2;i++)
	{
		if(m_stPrintDetail.stTestResultInfo[2].pDataPoints)
		{
			delete []m_stPrintDetail.stTestResultInfo[2].pDataPoints;
			m_stPrintDetail.stTestResultInfo[2].pDataPoints = NULL;
		}
	}

	//
	recordAB.clear();
	recordBA.clear();
}
/****************************************************************
函数名称:             DrawEvent2Way()                                                                               
函数说明:             画双向曲线事件                                                                              
入口参数:             CDC                                                                                           
出口参数:             无                                                                                        
建立时间:             2016-04-26                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawEvent2Way(CDC* pDC, CRect& rect, INT nCurveIndex)
{
	TestResultInfo* pData = &m_stPrintDetail.stTestResultInfo[nCurveIndex];
	if (pData == NULL ||
		pData->DataPointsNum < NUM_VALIDE_LEAST ||
		CLEAR_DATAPOINTS_NUM == pData->DataPointsNum)//没有或数据很少
		return;

	//绘事件点竖线及下方矩形框 zll 2013.5.23 晚
	INT fX,fY;
	INT prevfX = 0;//上一节点的位置
	INT offsetfx = 0; //两节点间偏距
	CRect rectTemp;
	rectTemp.SetRectEmpty();

	for (UINT i=0; i<pData->EventList.vEventInfo.size(); i++)
	{		
		CRect rectLineBottom;
		CString temp;
		fX=INT(rect.left + (pData->YOffset+pData->EventList.vEventInfo[i].fLocation)*rect.Width()/pData->Length);
		fY=INT(rect.bottom - (65535-pData->pDataPoints[pData->EventList.vEventInfo[i].index])/1000.0*rect.Height()/(OTDR_TRACE_Y_MAX-OTDR_TRACE_Y_MIN));//lzy2014.7.23 使事件点文字不沿y轴移动

		if(i == 0)
		{
			prevfX = fX;
		}
		else
		{
			offsetfx = fX - prevfX;
			prevfX = fX;
		}
		//画竖直线
		INT iTop = rect.top + rect.Height() / 6;
	    INT iBottom = rect.top + rect.Height() *5/ 6;
		INT iText = rect.top + rect.Height() *3/ 4;

		INT nLineH = rect.Height() / 10; //竖线高度
		INT nSquare = nLineH / 5 ;  //小正方形的边长
		INT nLineTop = nLineH / 6;  //上起始点位置
		//pDC->MoveTo(fX,fY- nLineTop);
		//pDC->LineTo(fX,fY+nLineH);
		pDC->MoveTo(fX,iTop);
		pDC->LineTo(fX,iBottom);
		//竖直线下面的正方形
		rectLineBottom.SetRect(fX - nSquare / 2,fY + nLineH,fX + nSquare / 2, fY + nLineH + nSquare);
		if (i>8)//两位数
		{
			rectLineBottom.right += 2;
		}
		//绘制衰减和相对距离、斜率
		CString strEventLosss;
		GetEventInfo2Way(i,strEventLosss,2);
		//strEventLosss.Format(_T("%.3fdB"), pData->EventList.vEventInfo[i].fEventloss);
		//CString strLocation;
		//CString strLink;
		//距离文本
		//float fValLocation = pData->EventList.vEventInfo[i].fLocation/DIVISOR_TOKM;
		//float fValLink = pData->EventList.vEventInfo[i].fLineK;
		
		CFont* pOldPen = pDC->SelectObject(&m_FontVR);

		//计算要显示的字符串高度
		INT height = pDC->DrawText( strEventLosss, &rectLineBottom, DT_WORDBREAK | DT_CENTER | DT_CALCRECT );
		CSize sizeText = pDC->GetTextExtent(strEventLosss);
		//损耗
		//pDC->TextOut(fX - sizeText.cy, fY + nLineH +  rectLineBottom.Height() + sizeText.cx, strEventLosss);
		pDC->TextOut(fX , iText, strEventLosss);
		//位置
		//strLocation.Format(_T("%.3fdBkm"), fValLocation);
		//sizeText = pDC->GetTextExtent(strLocation);
		//pDC->TextOut(fX , fY + nLineH + rectLineBottom.Height() + sizeText.cx, strLocation);
        //pDC->TextOut(fX , iText, strLocation);
		//斜率
		//sizeText = pDC->GetTextExtent(strEventLosss);
		//pDC->TextOut(fX - offsetfx / 2, fY + nLineH*2 + rectLineBottom.Height() + sizeText.cx, strLink);//lzy2014.8.26
		//pDC->TextOut(fX , iText, strLink);
		pDC->SelectObject(pOldPen);
	}
}

void CMyView::InnerDraw(CDC* pDC,INT nCurPage)
{
	INT nCurSel = 0;

	if (m_bPrintCurve) //打印当前选中曲线
	{
		if(m_stPrintDetail.stTestResultInfo[0].DataPointsNum>0&&m_stPrintDetail.stTestResultInfo[1].DataPointsNum>0)
		    nCurSel = SHOW_ALL;
		else if(m_stPrintDetail.stTestResultInfo[0].DataPointsNum>0&&m_stPrintDetail.stTestResultInfo[1].DataPointsNum<=0)
			nCurSel = SHOW_A2B;
		else
			nCurSel = SHOW_B2A;

		if(nCurPage == 1)//第一页
		    DrawPrintContent(pDC, m_rectDraw, nCurSel); 
		else
			DrawPrintContentPage2(pDC, m_rectDraw, nCurSel); 
		return;
	}
}

void CMyView::DrawGridLine(CDC* pDC, CRect& rect)
{
	if(!m_bShowGrid)//不显示栅格
	{
		return;
	}//eif

	CPen penGridLine;
	//penGridLine.CreatePen (PS_DOT/*虚线*/,1, g_sorFileArray.waveConfig.ColorGrid);//改为下面的cwcq2012.10.31
	penGridLine.CreatePen (PS_DOT/*虚线*/, 1, RGB(0xcc,0xcc,0xcc) );//wcq2012.10.31
	CPen* pOldPen = pDC->SelectObject (&penGridLine);

	if (m_bShowGrid)
	{
		//横坐标刻度
		INT ndx = rect.Width () / COOR_SPLIT_COUNT_H;
		INT ndy = rect.Height () / COOR_SPLIT_COUNT_V;
		pDC->SelectObject (&penGridLine);
		for ( INT i = 1; i< COOR_SPLIT_COUNT_H; i++)
		{	
			//横坐标刻度
			pDC->MoveTo(rect.left + ndx * i,rect.bottom );
			pDC->LineTo(rect.left + ndx * i,rect.top );
		}

		//纵坐标刻度
		pDC->SetTextAlign(TA_LEFT | TA_TOP);
		for (INT i=1; i<COOR_SPLIT_COUNT_V; i++)
		{
			pDC->MoveTo(rect.left , rect.bottom - ndy * i);
			pDC->LineTo(rect.right, rect.bottom - ndy * i);
		}
	}

	//恢复
	pDC->SelectObject(pOldPen);
	//释放资源
	DeleteObject(penGridLine.GetSafeHandle());
}

void CMyView::InitPrintText()
{
	//
    m_stPrintDetail.strTitle=m_util.ReadLangString(_T("PrintInfo"),_T("PRINT_TITLE"));
	m_stPrintDetail.strLogo=m_util.ReadLangString(_T("PrintInfo"), _T("PRINT_LOGO"));
	m_stPrintDetail.stPrintQuest.strTitle=m_util.ReadLangString(_T("PrintInfo"), _T("QUEST_TITLE"));
	m_stPrintDetail.stPrintQuest.strID=m_util.ReadLangString(_T("PrintInfo"), _T("QUEST_ID"));
	m_stPrintDetail.stPrintQuest.strContractor=m_util.ReadLangString(_T("PrintInfo"), _T("QUEST_CONTRACTOR"));
	m_stPrintDetail.stPrintQuest.strCustomer=m_util.ReadLangString(_T("PrintInfo"), _T("QUEST_USER"));
	m_stPrintDetail.stPrintQuest.strSorFileName=m_util.ReadLangString(_T("PrintInfo"), _T("QUEST_FILE"));
	m_stPrintDetail.stPrintQuest.strTestDate=m_util.ReadLangString(_T("PrintInfo"), _T("QUEST_DATE"));
	m_stPrintDetail.stPrintQuest.strPlayer=m_util.ReadLangString(_T("PrintInfo"), _T("QUEST_PLAYER"));

	m_stPrintDetail.stPrintQuest.strComment=m_util.ReadLangString(_T("PrintInfo"), _T("COMMENT_TITLE"));

	m_stPrintDetail.stPrintConfig.strTitle=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_TITLE"));
	m_stPrintDetail.stPrintConfig.strWavelength=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_WAVELEN"));
	m_stPrintDetail.stPrintConfig.strPulse=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_PULSE"));
	m_stPrintDetail.stPrintConfig.strOrigin=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ORIGIN"));
	m_stPrintDetail.stPrintConfig.strOriLocation=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ORILOCATION"));
	m_stPrintDetail.stPrintConfig.strOriCable=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ORICABLE"));
	m_stPrintDetail.stPrintConfig.strOriFiber=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ORIFIBER"));
	m_stPrintDetail.stPrintConfig.strOriColor=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ORICOLOR"));

	m_stPrintDetail.stPrintConfig.strRange=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_DIST"));
	m_stPrintDetail.stPrintConfig.strAverage=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_AVG"));
	m_stPrintDetail.stPrintConfig.strEnd=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_END"));
	m_stPrintDetail.stPrintConfig.strEndLocation=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ENDLOCATION"));
	m_stPrintDetail.stPrintConfig.strEndCable=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ENDCABLE"));
	m_stPrintDetail.stPrintConfig.strEndFiber=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ENDFIBER"));
	m_stPrintDetail.stPrintConfig.strEndColor=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ENDCOLOR"));

	m_stPrintDetail.stPrintConfig.strBackScatter=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_RBS"));
	m_stPrintDetail.stPrintConfig.strLossThreshold=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_LOSSTH"));
	m_stPrintDetail.stPrintConfig.strRefThreshold=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_REFLECTH"));
	m_stPrintDetail.stPrintConfig.strEndThreshold=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_ENDFIBERTH"));
	m_stPrintDetail.stPrintConfig.strIndex=m_util.ReadLangString(_T("PrintInfo"), _T("CONFIG_INDEX"));
	m_stPrintDetail.stPrintTrace.strTitle=m_util.ReadLangString(_T("PrintInfo"), _T("TRACE_TITLE"));
	m_stPrintDetail.stPrintEvent.strTitle=m_util.ReadLangString(_T("PrintInfo"), _T("EVENT_TITLE"));
	m_stPrintDetail.stPrintConfig.strTitleAB=m_util.ReadLangString(_T("MainDlg"), _T("IDS_MAIN_DLG_RADIO_SHOWAB"));
	m_stPrintDetail.stPrintConfig.strTitleBA=m_util.ReadLangString(_T("MainDlg"), _T("IDS_MAIN_DLG_RADIO_SHOWBA"));


	strListHead[0]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_ORDER"));
//	strListHead[1]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_TYPE"));
	strListHead[1]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_DIST"));
	strListHead[2]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_SEGMENT"));
	strListHead[3]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_LOSS"));
	strListHead[4]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_TOTAL_LOSS"));
	strListHead[5]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_SLOPE"));
	strListHead[6]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_REFLECTION"));

	strListHead2Way[0]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_ORDER"));
	strListHead2Way[1]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_DIST"));
	strListHead2Way[2]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_LOSS_AB"));
	strListHead2Way[3]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_LOSS_BA"));
	strListHead2Way[4]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_AVG_LOSS"));
	strListHead2Way[5]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_TOTAL_LOSS"));
	strListHead2Way[6]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_SLOPE"));
	strListHead2Way[7]=m_util.ReadLangString(_T("MainDlg"), _T("IDS_EVENT_LIST_EVENT_MAX_REFLECTION"));
}

void CMyView::MemcpyResInfo(TestResultInfo *pResultInfo)
{
	INT nTemp = 0;

	for(INT i=0;i<3;i++)
	{
		if(pResultInfo->DataPointsNum<=0) 
		{
			pResultInfo++;
			continue;
		}

		nTemp = pResultInfo->EventList.vEventInfo.size();
		if (NULL == m_stPrintDetail.stTestResultInfo[i].pDataPoints)
		{
			m_stPrintDetail.stTestResultInfo[i].pDataPoints = new WORD[pResultInfo->DataPointsNum];
			memcpy(m_stPrintDetail.stTestResultInfo[i].pDataPoints,pResultInfo->pDataPoints,pResultInfo->DataPointsNum * 2);
#ifdef COMPILE_SHOW_SMOOTH_DATA////显示时用平滑过的数据，保存时用实际数据 wcq2013.04.26
			if (NULL == testResultInfo.pDataPointsSmooth)
			{
				testResultInfo.pDataPointsSmooth = new WORD[sorData.vAllData[0].nCount];
			}
#endif
		}
		else//已有数据
		{
			if (m_stPrintDetail.stTestResultInfo[i].DataPointsNum < pResultInfo->DataPointsNum)//已有数据长度比将要赋值的少
			{
				delete[] m_stPrintDetail.stTestResultInfo[i].pDataPoints;
				m_stPrintDetail.stTestResultInfo[i].pDataPoints = new WORD[pResultInfo->DataPointsNum];
#ifdef COMPILE_SHOW_SMOOTH_DATA////显示时用平滑过的数据，保存时用实际数据 wcq2013.04.26
				delete[] testResultInfo.pDataPointsSmooth;
				testResultInfo.pDataPointsSmooth = new WORD[sorData.vAllData[0].nCount];
#endif
			}//eif
			memcpy(m_stPrintDetail.stTestResultInfo[i].pDataPoints,&pResultInfo->pDataPoints,pResultInfo->DataPointsNum * 2);
		}//eif
		m_stPrintDetail.stTestResultInfo[i].DataPointsNum = pResultInfo->DataPointsNum;

		//
		m_stPrintDetail.stTestResultInfo[i].Refraction = pResultInfo->Refraction;
		m_stPrintDetail.stTestResultInfo[i].PulseWidth = pResultInfo->PulseWidth;
		m_stPrintDetail.stTestResultInfo[i].WaveLength = pResultInfo->WaveLength;
		m_stPrintDetail.stTestResultInfo[i].Frequency = pResultInfo->Frequency;
		m_stPrintDetail.stTestResultInfo[i].DataSpacing = pResultInfo->DataSpacing;
		m_stPrintDetail.stTestResultInfo[i].Length = pResultInfo->Length;

		m_stPrintDetail.stTestResultInfo[i].EventList.vEventInfo.clear();//事件列表不断增加 wcq2012.08.13
		//nTemp = 50;
		for(INT j=0; j<nTemp; j++)
		{	
			EventInfo eventInfo;

			memcpy(&eventInfo,&pResultInfo->EventList.vEventInfo[j],sizeof(EventInfo));
			m_stPrintDetail.stTestResultInfo[i].EventList.vEventInfo.push_back(eventInfo);
		}//efor

		for(INT k=0;k<nTemp;k++)
		{
			m_stPrintDetail.stTestResultInfo[i].EventList.vEventInfo[k].fTotalLoss = pResultInfo->EventList.vEventInfo[k].fTotalLoss;
		}//efor
		m_stPrintDetail.stTestResultInfo[i].BackscatterCoefficient = pResultInfo->BackscatterCoefficient;
		m_stPrintDetail.stTestResultInfo[i].LossThreshold = pResultInfo->LossThreshold;
		m_stPrintDetail.stTestResultInfo[i].ReflectanceThreshold = pResultInfo->ReflectanceThreshold;
		m_stPrintDetail.stTestResultInfo[i].EndOfFiberThreshold = pResultInfo->EndOfFiberThreshold;
		m_stPrintDetail.stTestResultInfo[i].Range = pResultInfo->Range;//将长度作为测量范围，单位是m
		m_stPrintDetail.stTestResultInfo[i].strTestTime = pResultInfo->strTestTime;//将长度作为测量范围，单位是m
		m_stPrintDetail.stTestResultInfo[i].strFileName = pResultInfo->strFileName;
		pResultInfo++;
    }
}

void CMyView::GetEvent2Way(TestResultInfo *pInfo2Way, TestResultInfo *infoA2B, TestResultInfo *infoB2A)
{
	std::vector<EventInfo *> vEventInfo;

	pInfo2Way->EventList.vEventInfo.clear();

	UINT uiEvents = infoB2A->EventList.vEventInfo.size();  //B->A线的总事件数

	UINT uiOffset = infoB2A->DataPointsNum - infoB2A->EventList.vEventInfo[uiEvents - 1].index;   //总点数-最后事件的索引
	
	for(UINT i = 0; i != infoA2B->EventList.vEventInfo.size(); i++ ) //先把第一条线的事件放入
	{
		pInfo2Way->EventList.vEventInfo.push_back(infoA2B->EventList.vEventInfo[i]);
		pInfo2Way->EventList.vEventInfo[ i ].index += uiOffset;  //将数据点平移一下
	}	
	
	float fOffset = infoB2A->Range -  infoB2A->EventList.vEventInfo[uiEvents -1].fLocation;   //总距离-最后事件的位置
	for(UINT i = 0; i != uiEvents; i++ )  //再把第二条线的数据放入临时数组
	{
		EventInfo *pEvent = new EventInfo();//&(infoB2A->EventList.vEventInfo[uiEvents - 1 - i]);//
		*pEvent = infoB2A->EventList.vEventInfo[uiEvents - 1 - i];
		pEvent->fLocation = infoB2A->EventList.vEventInfo[uiEvents -1].fLocation -  infoB2A->EventList.vEventInfo[uiEvents -1 - i].fLocation;  //第一个事件为0
		pEvent->index = infoB2A->DataPointsNum - infoB2A->EventList.vEventInfo[uiEvents - 1 - i].index;	 //倒向了，第一个事件不为0	      
		
		//pInfo2Way->EventList.vEventInfo.push_back(*pEvent);
		vEventInfo.push_back(pEvent);
	}

	INT iDiff = 0;   //位置差如果较小，可认为是一个事件
	INT iCount = 0;
	for(std::vector<EventInfo *>::iterator it = vEventInfo.begin(); it != vEventInfo.end(); ++it)  //全部放一起
	{
		BOOL bSel = FALSE;
        //插入入到某个位置
		for(std::vector<EventInfo>::iterator it2 = pInfo2Way->EventList.vEventInfo.begin(); it2 != pInfo2Way->EventList.vEventInfo.end(); ++it2)
		{
			iDiff = (*it)->index - ((EventInfo)*it2).index ;
			
			if(iDiff <= 0) //满足此条件插入到前面
			{
				pInfo2Way->EventList.vEventInfo.insert(it2, **it);
				bSel = TRUE;
				break;
			}
		}

		if(!bSel)
			pInfo2Way->EventList.vEventInfo.push_back(**it);
	}

	//以下处理合并相关的事件
	std::vector<EventInfo>::iterator it;
/*	UINT uiLen =  pInfo2Way->EventList.vEventInfo.size();
	if(uiLen>1)
	{
		it = pInfo2Way->EventList.vEventInfo.begin() + 1;
		pInfo2Way->EventList.vEventInfo.erase(it);  //删除第二个元素，即第一条曲线的首个事件
		if(uiLen>3)
		{
		    it = pInfo2Way->EventList.vEventInfo.end() - 2;
		    pInfo2Way->EventList.vEventInfo.erase(it);  //删除最后第二个元素，即第二条曲线的最后一个事件
		}
	}
*/
    UINT uiFirst = pInfo2Way->EventList.vEventInfo[0].index;
	UINT uiTemp;
	float fEventHigh = pInfo2Way->EventList.vEventInfo[0].fEventHigh;
	//EventInfo eventTemp = ((EventInfo)*it);
	for(it = pInfo2Way->EventList.vEventInfo.begin() + 1; it < pInfo2Way->EventList.vEventInfo.end() - 1; ++it)  //循环合并事件
	{
		uiTemp = (*it).index;	
		if(abs(1.0 * uiTemp - uiFirst) <= EVENT_2WAY_OFFSET)
		{
			fEventHigh = max(fEventHigh, ((EventInfo)*it).fEventHigh);   //取两个的较大的值保留

			it = pInfo2Way->EventList.vEventInfo.erase(it);
			//(*it).fEventHigh = fEventHigh;
			(*(--it)).fEventHigh = fEventHigh;
			++it;
		}
		uiFirst =  (*it).index;
		//eventTemp = ((EventInfo)*it);
		fEventHigh = (*it).fEventHigh;
	}

	std::vector<EventInfo *>::iterator it_Event;
	for(it_Event = vEventInfo.begin(); it_Event != vEventInfo.end(); it_Event++)
	{
		delete *it_Event;
		*it_Event = NULL;
	}
	vEventInfo.clear();
}
/****************************************************************
函数名称:             DrawQuestInfo()                                                                               
函数说明:             画任务信息                                                                        
入口参数:                                                                               
出口参数:             无                                                                                        
建立时间:             2016-05-01                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawQuestInfo(CDC* pDC, CRect rectDraw,INT &nBottom,INT nCurSel)
{
	INT nCurHeight=0;
	INT nXoffset = rectDraw.left;
	INT nYoffset = rectDraw.top;
	INT nVspace = rectDraw.Height() / 64;
	INT nHleftspace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	INT nHmidspace = rectDraw.Width() / 20;
	INT nHinterval = nVspace / 4;   //矩形框垂直间隔
	INT nHMargin = nHleftspace / 2;   //矩形框内文字与边缘的距离
	INT nHSubMargin = nHleftspace / 2;  //子项目的缩进量
	INT n1 = nXoffset + nHleftspace + nHMargin;  //左边第一项的起始X坐标
//	INT n2 = n1 + (rectDraw.Width() - 2 * (n1 - nXoffset)) / 3;   //中间项的起始X坐标
	//INT n3 = n2 + (rectDraw.Width() - 2 * (n1 - nXoffset)) / 3;   //最右边项的起始X坐标
	CRect textRect;

	TEXTMETRIC  tm;
	pDC->GetTextMetrics(&tm);
	INT nCharWidth = tm.tmMaxCharWidth;  //最大字符宽度
	CString strText = _T("Test char height 测试字体");
	CSize sizeText = pDC->GetTextExtent(strText);
	INT nCharHeight = sizeText.cy;    //内容字体的高度
	INT nLineInterval = nCharHeight / 2 ;  //0.5倍行距

	INT iWidth = 0;
	INT iX = 0;
	INT iY = 0;
	INT iHeight = 0;

	nCurHeight = nBottom + nHinterval;
	nBottom = nCurHeight + nLineInterval * 4 + nCharHeight * 3;
	pDC->Rectangle(nXoffset + nHleftspace, nCurHeight, nXoffset + rectDraw.Width() - nHleftspace, nBottom);	
	pDC->SelectObject(&m_fontGroup);
	strText = _T(" ") + m_stPrintDetail.stPrintQuest.strTitle + _T(" ");
	sizeText = pDC->GetTextExtent(strText);
	textRect.SetRect(nXoffset + (rectDraw.Width() - sizeText.cx) / 2, nCurHeight - sizeText.cy / 2, nXoffset + (rectDraw.Width() + sizeText.cx) / 2, nCurHeight + sizeText.cy / 2);
	pDC->DrawText(strText, textRect, DT_CENTER);  //写任务信息字样	
	pDC->SelectObject(&m_dataFont);
	nCurHeight += nLineInterval;
	strText = m_stPrintDetail.stPrintQuest.strID + _T(": ") + m_stPrintDetail.stPrintQuest.QuestID;  //写任务编号
	sizeText = pDC->GetTextExtent(strText);
	textRect.SetRect(nXoffset + nHleftspace + nHMargin, nCurHeight, nXoffset + nHleftspace + sizeText.cx + nHMargin, nCurHeight + sizeText.cy);
	pDC->DrawText(strText, textRect, DT_LEFT);		
	//strText = _T("File: ") + m_stPrintDetail.stPrintQuest.SorFileName;  //写文件名
	if(nCurSel!=SHOW_ALL)
		strText = m_stPrintDetail.stPrintQuest.strSorFileName + _T(": ") + m_stPrintDetail.stTestResultInfo[nCurSel].strFileName;  //写文件名
	else
		strText = m_stPrintDetail.stPrintQuest.strSorFileName + _T(": ") + m_stPrintDetail.stTestResultInfo[0].strFileName + _T("  ") + m_stPrintDetail.stTestResultInfo[1].strFileName;  //写文件名
	sizeText = pDC->GetTextExtent(strText);
	//textRect.SetRect(m_rectDraw.Width() / 2  - nHMargin, nCurHeight, m_rectDraw.Width() / 2 + sizeText.cx - nHMargin, nCurHeight + sizeText.cy);
	textRect.SetRect(nXoffset + rectDraw.Width() / 2  - nHMargin, nCurHeight, 2*nXoffset + rectDraw.Width() - n1, nCurHeight + sizeText.cy);
	pDC->DrawText(strText, textRect, DT_LEFT);
	nCurHeight = textRect.bottom + nLineInterval;
	//strText = _T("Contractor: ") + m_stPrintDetail.stPrintQuest.Contractor;  //写承包商
	strText = m_stPrintDetail.stPrintQuest.strContractor + _T(": ") + m_stPrintDetail.stPrintQuest.Contractor;  //写承包商
	sizeText = pDC->GetTextExtent(strText);
	textRect.SetRect(nXoffset + nHleftspace + nHMargin, nCurHeight, nXoffset + nHleftspace + sizeText.cx + nHMargin, nCurHeight + sizeText.cy);
	pDC->DrawText(strText, textRect, DT_LEFT);
	//strText = _T("Test date: ") + m_stPrintDetail.stPrintQuest.TestDate;  //写测试时间
	if(nCurSel!=SHOW_ALL)
		strText = m_stPrintDetail.stPrintQuest.strTestDate + _T(": ") + m_stPrintDetail.stTestResultInfo[nCurSel].strTestTime;  //写测试时间
	else
		strText = m_stPrintDetail.stPrintQuest.strTestDate + _T(": ") + m_stPrintDetail.stTestResultInfo[0].strTestTime + _T("  ") + m_stPrintDetail.stTestResultInfo[1].strTestTime;  //写测试时间

	sizeText = pDC->GetTextExtent(strText);
	textRect.SetRect(nXoffset + rectDraw.Width() / 2 - nHMargin, nCurHeight, nXoffset + rectDraw.Width() / 2 + sizeText.cx - nHMargin, nCurHeight + sizeText.cy);
	pDC->DrawText(strText, textRect, DT_LEFT);
	nCurHeight = textRect.bottom + nLineInterval;
	strText = m_stPrintDetail.stPrintQuest.strCustomer + _T(": ") + m_stPrintDetail.stPrintQuest.Customer;  //客户
	sizeText = pDC->GetTextExtent(strText);
	textRect.SetRect(nXoffset + nHleftspace + nHMargin, nCurHeight, nXoffset + nHleftspace + sizeText.cx + nHMargin, nCurHeight + sizeText.cy);
	pDC->DrawText(strText, textRect, DT_LEFT);
	//strText = _T("Operator: ") + m_stPrintDetail.stPrintQuest.Operator;  //写操作员
	strText = m_stPrintDetail.stPrintQuest.strPlayer + _T(": ") + m_stPrintDetail.stPrintQuest.Player;  //写操作员
	sizeText = pDC->GetTextExtent(strText);
	textRect.SetRect(nXoffset + rectDraw.Width() / 2 - nHMargin, nCurHeight, nXoffset + rectDraw.Width() / 2 + sizeText.cx - nHMargin, nCurHeight + sizeText.cy);
	pDC->DrawText(strText, textRect, DT_LEFT);
}
/****************************************************************
函数名称:             DrawConfig()                                                                               
函数说明:             画配置信息                                                                        
入口参数:                                                                               
出口参数:             无                                                                                        
建立时间:             2016-05-01                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawConfig(CDC* pDC, CRect rectDraw,INT &nBottom,INT nCurSel)
{
	INT nCurHeight=0;
	INT nXoffset = rectDraw.left;
	INT nYoffset = rectDraw.top;
	INT nVspace = rectDraw.Height() / 64;
	INT nHleftspace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	INT nHmidspace = rectDraw.Width() / 20;
	INT nHinterval = nVspace / 4;   //矩形框垂直间隔
	INT nHMargin = nHleftspace / 2;   //矩形框内文字与边缘的距离
	INT nHSubMargin = nHleftspace / 2;  //子项目的缩进量
	INT n1 = nXoffset + nHleftspace + nHMargin;  //左边第一项的起始X坐标
//	INT n2 = n1 + (rectDraw.Width() - 2 * (n1 - nXoffset)) / 3;   //中间项的起始X坐标
	//INT n3 = n2 + (rectDraw.Width() - 2 * (n1 - nXoffset)) / 3;   //最右边项的起始X坐标
	CRect textRect;

	TEXTMETRIC  tm;
	pDC->GetTextMetrics(&tm);
	INT nCharWidth = tm.tmMaxCharWidth;  //最大字符宽度
	CString strText = _T("Test char height 测试字体");
	CSize sizeText = pDC->GetTextExtent(strText);
	INT nCharHeight = sizeText.cy;    //内容字体的高度
	INT nLineInterval = nCharHeight / 2 ;  //0.5倍行距

	INT iWidth = 0;
	INT iX = 0;
	INT iY = 0;
	INT iHeight = 0;

	nCurHeight = nBottom + nVspace / 2;	
	nBottom = nCurHeight + nCharHeight * 10 + nLineInterval * 11;
	pDC->Rectangle(nXoffset + nHleftspace, nCurHeight, nXoffset + rectDraw.Width() - nHleftspace, nBottom);
	pDC->SelectObject(&m_fontGroup);
	strText = _T(" ") + m_stPrintDetail.stPrintConfig.strTitle + _T(" ");
	sizeText = pDC->GetTextExtent(strText);
	textRect.SetRect(nXoffset + (rectDraw.Width() - sizeText.cx) / 2, nCurHeight - sizeText.cy / 2, nXoffset +(rectDraw.Width() + sizeText.cx) / 2, nCurHeight + sizeText.cy / 2);
	pDC->DrawText(strText, textRect, DT_LEFT);
	pDC->SelectObject(&m_dataFont);	
	nCurHeight += nLineInterval;
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;

	iX = nXoffset + nHleftspace + iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(m_stPrintDetail.stPrintConfig.strTitleAB, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(m_stPrintDetail.stPrintConfig.strTitleBA, textRect, DT_LEFT);	
	//写wavelength
	nCurHeight += nLineInterval*2;
	strText = m_stPrintDetail.stPrintConfig.strWavelength + _T("(nm): ") ;  //写wavelength
	
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);		

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%d"),m_stPrintDetail.stTestResultInfo[0].WaveLength);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%d"),m_stPrintDetail.stTestResultInfo[1].WaveLength);
	pDC->DrawText(strText, textRect, DT_LEFT);
	//写Range
	strText = m_stPrintDetail.stPrintConfig.strRange + _T("(m): ");  //写Range
	nCurHeight = textRect.bottom + nLineInterval;	
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[0].Length);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[1].Length);
	pDC->DrawText(strText, textRect, DT_LEFT);
	//写Backscatter coeff 3
	strText = m_stPrintDetail.stPrintConfig.strBackScatter + _T("(dB): ");  //写Backscatter coeff 3
	nCurHeight = textRect.bottom + nLineInterval;	
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;	
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[0].BackscatterCoefficient);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[1].BackscatterCoefficient);
	pDC->DrawText(strText, textRect, DT_LEFT);

	//写Pulse
	strText = m_stPrintDetail.stPrintConfig.strPulse + _T("(ns): ");//写PULSE
	nCurHeight = textRect.bottom + nLineInterval;	
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;		
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%ld"),m_stPrintDetail.stTestResultInfo[0].PulseWidth);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%ld"),m_stPrintDetail.stTestResultInfo[1].PulseWidth);
	pDC->DrawText(strText, textRect, DT_LEFT);
	//写Average counts
	strText = m_stPrintDetail.stPrintConfig.strAverage + _T(": ");  //写Average counts  
	nCurHeight = textRect.bottom + nLineInterval;	
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;		
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[0].Average);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[1].Average);
	pDC->DrawText(strText, textRect, DT_LEFT);
	//写LossThreshold 3
	strText = m_stPrintDetail.stPrintConfig.strLossThreshold + _T("(dB): ");  //写LossThreshold 3
	nCurHeight = textRect.bottom + nLineInterval;	
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;		
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[0].LossThreshold);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[1].LossThreshold);
	pDC->DrawText(strText, textRect, DT_LEFT);

	//写Reflection threshold
	strText = m_stPrintDetail.stPrintConfig.strRefThreshold + _T("(dB): ");  //写Reflection threshold
	nCurHeight = textRect.bottom + nLineInterval;	
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;		
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[0].ReflectanceThreshold);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[1].ReflectanceThreshold);
	pDC->DrawText(strText, textRect, DT_LEFT);

	//写Fiber end threshold
	strText = m_stPrintDetail.stPrintConfig.strEndThreshold + _T("(dB): ");  //写Fiber end threshold
	nCurHeight = textRect.bottom + nLineInterval;	
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;		
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[0].EndOfFiberThreshold);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[1].EndOfFiberThreshold);
	pDC->DrawText(strText, textRect, DT_LEFT);

	//写 折射率
	strText = m_stPrintDetail.stPrintConfig.strIndex + _T(": ");  //写 折射率
	nCurHeight = textRect.bottom + nLineInterval;	
	iWidth = (nXoffset + rectDraw.Width() - nHleftspace)/3;
	iX = nXoffset + nHleftspace;
	iY = nCurHeight;
	iHeight = sizeText.cy;		
	textRect.SetRect(n1,iY,iX+iWidth,iY+iHeight);
	pDC->DrawText(strText, textRect, DT_LEFT);

	iX += iWidth;
	iY = nCurHeight;
	iHeight = sizeText.cy;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[0].Refraction);
	pDC->DrawText(strText, textRect, DT_LEFT);	

	iX += iWidth;
	textRect.SetRect(iX,iY,iX+iWidth,iY+iHeight);
	strText.Format(_T("%.3f"),m_stPrintDetail.stTestResultInfo[1].Refraction);
	pDC->DrawText(strText, textRect, DT_LEFT);
}
/****************************************************************
函数名称:             DrawPrintContentPage2()                                                                               
函数说明:             画双向曲线内容第二页                                                                            
入口参数:             nLeft 左起点， nTop 上起点                                                                   
出口参数:             无                                                                                        
建立时间:             2016-05-01                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawPrintContentPage2(CDC* pDC, CRect rectDraw, INT nCurSel, BOOL bSingle)
{
	//画线
	INT nXoffset = rectDraw.left;
	INT nYoffset = rectDraw.top;
	INT nVspace = rectDraw.Height() / 64;
	INT nCurHeight = nYoffset+ nVspace * 3;
	INT nHleftspace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	INT nHmidspace = rectDraw.Width() / 20;
	INT nHinterval = nVspace / 4;   //矩形框垂直间隔
	INT nHMargin = nHleftspace / 2;   //矩形框内文字与边缘的距离
	INT nHSubMargin = nHleftspace / 2;  //子项目的缩进量
	INT n1 = nXoffset + nHleftspace + nHMargin;  //左边第一项的起始X坐标
	INT n2 = n1 + (rectDraw.Width() - 2 * (n1 - nXoffset)) / 3;   //中间项的起始X坐标
	INT n3 = n2 + (rectDraw.Width() - 2 * (n1 - nXoffset)) / 3;   //最右边项的起始X坐标
	INT nBottom = 0;   //保存矩形框的下边纵坐标
	CRect textRect;
	CString strText , strTemp;	
	CSize sizeText;	
	CFont* pOldFont = pDC->SelectObject (&m_dataFont);

	TEXTMETRIC  tm;
	pDC->GetTextMetrics(&tm);
	INT nCharWidth = tm.tmMaxCharWidth;  //最大字符宽度
	strText = _T("Test char height 测试字体");
	sizeText = pDC->GetTextExtent(strText);
	INT nCharHeight = sizeText.cy;    //内容字体的高度
	INT nLineInterval = nCharHeight / 2 ;  //0.5倍行距
	//以下写第一行	
	{
		pDC->SelectObject (&m_dataFont);
		nCurHeight = nBottom + nVspace; 		
		sizeText = pDC->GetTextExtent(m_stPrintDetail.strLogo);
		nBottom = nCurHeight + sizeText.cy;
		textRect.SetRect(nXoffset + nHleftspace, nCurHeight, nXoffset + nHleftspace + sizeText.cx, nCurHeight + sizeText.cy);
		if(m_bPrintLogo)
			pDC->DrawText(m_stPrintDetail.strLogo, textRect, DT_LEFT);  //写软件版本及光维版权
		CTime tm = CTime::GetCurrentTime();
		CString strCurTime = tm.Format(_T("%Y-%m-%d %H:%M:%S"));
		sizeText = pDC->GetTextExtent(strCurTime);
		textRect.SetRect(nXoffset + rectDraw.Width() - nHleftspace - sizeText.cx ,nCurHeight, nXoffset + rectDraw.Width() - nHleftspace, nCurHeight + sizeText.cy);
		pDC->DrawText(strCurTime, textRect, DT_LEFT);  //写当前打印的时刻
	}

	//事件列表
	if(m_bPrintEvent)
	{
		nCurHeight = nBottom + nVspace / 2;
		//绘制事件列表表头
		DrawEventHead(pDC, nXoffset + nHleftspace, nCurHeight + nLineInterval, rectDraw,nCurSel);
		//绘制事件体
		if(nCurSel != SHOW_ALL) //为了区分单曲线与多曲线，此处改为根据自定义曲线编号来显示事件。lzy2014.8.30
			DrawEventListPage2(pDC, nXoffset + nHleftspace, nCurHeight + nLineInterval, nLineInterval , rectDraw, m_stPrintDetail.curSelection);
		else
			DrawEventList2WayPage2(pDC, nXoffset + nHleftspace, nCurHeight + nLineInterval, nLineInterval , rectDraw, nCurSel);

		//画矩形框
		pDC->SelectObject(&m_dataFont);
		//strText = _T(" Event info ");
		strText = _T(" ") + m_stPrintDetail.stPrintEvent.strTitle + _T(" ");
		sizeText = pDC->GetTextExtent(strText);
		nBottom =  nCurHeight + (m_stPrintDetail.stPrintEvent.EventCount + 1) * (nLineInterval + nCharHeight) + nLineInterval;
		textRect.SetRect(nXoffset + nHleftspace , nCurHeight,nXoffset + rectDraw.Width() - nHleftspace, nBottom);
		CBrush *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		CBrush *pOldBrush = pDC->SelectObject(pBrush);
		pDC->Rectangle(textRect);  //画矩形框
		pDC->SelectObject(pOldBrush);

		//画横线
		for(INT i = 1; i<m_stPrintDetail.stPrintEvent.EventCount +1; i++)
		{		
			pDC->MoveTo(nXoffset + nHleftspace , nCurHeight + nLineInterval / 2 + i * (nLineInterval + nCharHeight));
			pDC->LineTo(rectDraw.right - nHleftspace, nCurHeight + nLineInterval / 2 + i * (nLineInterval + nCharHeight));
		}
	}	

	pDC->SelectObject (pOldFont);
}

/****************************************************************
函数名称:             DrawEventList2WayPage2()                                                                               
函数说明:             画双向曲线事件列表体第二页                                                                            
入口参数:             nLeft 左起点， nTop 上起点                                                                   
出口参数:             无                                                                                        
建立时间:             2016-05-01                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawEventList2WayPage2(CDC* pDC, INT nLeft, INT nTop , INT nVspace , CRect rectDraw, INT nCurSel)//绘制事件列表
{
	CFont* pOldFont = pDC->SelectObject (&m_dataFont);
	INT nLeftSpace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	INT nHspace = (rectDraw.Width() - 2 * nLeftSpace) / EVENT_LIST_COL;  //lzy2014.8.12
	INT nStartLine = 1;

	float preDistance=0;
	float fLossAvg = 0;
	//TestResultInfo* pData = g_sorFileArray.sorFileList[g_sorFileArray.curSelection].stTestResultInfo;
	TestResultInfo* pData = &m_stPrintDetail.stTestResultInfo[nCurSel];  //lzy2014.8.30

	if (NULL == pData)
		return;

	CString strText=_T("Test Height");
	CSize sizeText = pDC->GetTextExtent(strText);
	INT nCharHeight = sizeText.cy;  //获取字体高度
	CRect textRect;
	std::vector<EventInfo> ve = pData->EventList.vEventInfo;//速度??
	INT nSize=ve.size();

	if(ve.size()<=EVENT_LIST_PRINT_MAX)
		return;
	else
		nSize = ve.size();

	m_stPrintDetail.stPrintEvent.EventCount = nSize-EVENT_LIST_PRINT_MAX;   //保存行数

	for (INT i=EVENT_LIST_PRINT_MAX;i<nSize;i++)//
	{
		// 序号
		{
			textRect.left = nLeft;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace; lzy 2014.8.15

			//类型
			strText.Format(_T("%d"),i+1);
			if (g_iLang == CHS)
			{
				if (0 == ve[i].nEventType)
				{
					strText += _T(" 非反射");//0代表非反射事件符号
				}
				else if (1 == ve[i].nEventType)
				{
					strText += _T(" 反射");;//1代表反射事件符号
				}
			}
			else if (g_iLang == ENG)
			{
				if (0 == ve[i].nEventType)
				{
					strText += _T(" NoReflect");//0代表非反射事件符号
				}
				else if (1 == ve[i].nEventType)
				{
					strText += _T(" Reflect");;//1代表反射事件符号
				}
			}

			if (_T('S')==ve[i].cEventType)
			{
				strText+=_T("(S)");
			}
			else if (_T('E')==ve[i].cEventType)
			{
				strText+=_T("(E)");
			}
			else if (_T('A')==ve[i].cEventType)
			{
				strText+=_T("(A)");
			}
			else if (_T('F')==ve[i].cEventType)
			{
				strText+=_T("(F)");
			}
			else if (_T('O')==ve[i].cEventType)
			{
				strText+=_T("(O)");
			}

			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		{
			textRect.left = nLeft + nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 2 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			float m_float_zll=0.0f;
			if (0==i)
			{
				strText.Format(_T("%.3f"),0.0f);//距离
			}
			else
			{
				//if(_T("m")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为km
					strText.Format(_T("%.3f"),ve[i].fLocation/1000);//距离
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		//strText.Format(_T("%.3f"), 0.0);
		strText.Format(_T("--"));//为0时显示"-"
//以下获取A->B LOSS
		textRect.left = nLeft + 2 * nHspace;
		textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
		textRect.right = nLeft + 3 * nHspace;
		textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;

		GetEventInfo2Way(i,strText,0);
		pDC->DrawText(strText, &textRect,DT_CENTER);//文字

		//strText.Format(_T("%.3f"), 0.0);
		strText.Format(_T("--"));//为0时显示"-"

		//以下获取B->A LOSS
		textRect.left = nLeft + 3 * nHspace;
		textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
		textRect.right = nLeft + 4 * nHspace;
		textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;

		GetEventInfo2Way(i,strText,1);
		pDC->DrawText(strText, &textRect,DT_CENTER);//文字

		//以下给出平均损耗
		textRect.left = nLeft + 4 * nHspace;
		textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
		textRect.right = nLeft + 5 * nHspace;
		textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
		
		GetEventInfo2Way(i,strText,2);
		pDC->DrawText(strText, &textRect,DT_CENTER);//文字

		// 总损耗
		{
			textRect.left = nLeft + 5 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 6 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].fTotalLoss)//为0时显示"-"wcq2012.07.09
			{
				strText.Format(_T("--"));//总损耗
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fTotalLoss);//总损耗
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 斜率
		{
			textRect.left = nLeft + 6 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 7 * nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace;
			//斜率
			if (fabs(ve[i].fLineK)>0.5)//zll 2012.10.31
			{
				strText.Format(_T("--"));
			}
			else if (0 == ve[i].fLineK)//为0时显示"-" zll 2012.10.31
			{
				strText.Format(_T("--"));
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fLineK);//斜率
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 反射
		{
			textRect.left = nLeft + 7 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 8 * nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].nEventType)//反射
			{
				strText=_T("--");
			}
			else if (0 == ve[i].fEventHigh)//为0时显示"-"wcq2012.07.09
			{
				strText=_T("--");
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fEventHigh);//反射
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		nStartLine++; //行数加1
	}
	pDC->SelectObject (pOldFont);
}
/****************************************************************
函数名称:             DrawEventListPage2()                                                                               
函数说明:             画曲线事件列表体第二页                                                                            
入口参数:             nLeft 左起点， nTop 上起点                                                                   
出口参数:             无                                                                                        
建立时间:             2016-05-01                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::DrawEventListPage2(CDC* pDC, INT nLeft, INT nTop , INT nVspace , CRect rectDraw, INT nCurSel)//绘制事件列表
{
	CFont* pOldFont = pDC->SelectObject (&m_dataFont);
	INT nLeftSpace = rectDraw.Width() / MARGIN_LEFT_COUNT;
	INT nHspace = (rectDraw.Width() - 2 * nLeftSpace) / EVENT_LIST_COL;  //lzy2014.8.12
	INT nStartLine = 1;

	float preDistance=0;

	TestResultInfo* pData = &m_stPrintDetail.stTestResultInfo[nCurSel];  //lzy2014.8.30

	if (NULL == pData)
		return;

	CString strText=_T("Test Height");
	CSize sizeText = pDC->GetTextExtent(strText);
	INT nCharHeight = sizeText.cy;  //获取字体高度
	CRect textRect;
	std::vector<EventInfo> ve = pData->EventList.vEventInfo;//速度??
	INT nSize=ve.size();

	if(ve.size()<=EVENT_LIST_PRINT_MAX)
		return;
	else
		nSize = ve.size();

	m_stPrintDetail.stPrintEvent.EventCount = nSize-EVENT_LIST_PRINT_MAX;   //保存行数
	for (INT i=EVENT_LIST_PRINT_MAX;i<nSize;i++)//
	{
		// 序号
		{
			textRect.left = nLeft;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace; lzy 2014.8.15

			//类型
			strText.Format(_T("%d"),i+1);
			if (g_iLang == CHS)
			{
				if (0 == ve[i].nEventType)
				{
					strText += _T(" 非反射");//0代表非反射事件符号
				}
				else if (1 == ve[i].nEventType)
				{
					strText += _T(" 反射");;//1代表反射事件符号
				}
			}
			else if (g_iLang == ENG)
			{
				if (0 == ve[i].nEventType)
				{
					strText += _T(" NoReflect");//0代表非反射事件符号
				}
				else if (1 == ve[i].nEventType)
				{
					strText += _T(" Reflect");;//1代表反射事件符号
				}
			}

			if (_T('S')==ve[i].cEventType)
			{
				strText+=_T("(S)");
			}
			else if (_T('E')==ve[i].cEventType)
			{
				strText+=_T("(E)");
			}
			else if (_T('A')==ve[i].cEventType)
			{
				strText+=_T("(A)");
			}
			else if (_T('F')==ve[i].cEventType)
			{
				strText+=_T("(F)");
			}
			else if (_T('O')==ve[i].cEventType)
			{
				strText+=_T("(O)");
			}

			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		{
			textRect.left = nLeft + nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 2 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			float m_float_zll=0.0f;
			if (0==i)
			{
				strText.Format(_T("%.3f"),0.0f);//距离
			}
			else
			{
				//if(_T("m")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为km
					strText.Format(_T("%.3f"),ve[i].fLocation/1000);//距离
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 段
		{
			textRect.left = nLeft + 2 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 3 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			float a,b=0.0f;
			if (0==i)
			{
				strText.Format(_T("%.3f"),0.0f);//段
			}
			else
			{
				//if(_T("m")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为km
					strText.Format(_T("%.3f"),(ve[i].fLocation-preDistance)/1000);//段,两事件点间距离
			}
			if (0==i)
			{
				preDistance = 0.0f;
			}
			else
			{
				preDistance = ve[i].fLocation;
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 损耗
		{
			textRect.left = nLeft + 3 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 4 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].fEventloss)//为0时显示"-"wcq2012.07.09
			{
				strText.Format(_T("--"));//总损耗
			}
			else
			{
				strText.Format(_T("%.3f"), ve[i].fEventloss);//损耗
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 总损耗
		{
			textRect.left = nLeft + 4 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 5 * nHspace;
			textRect.bottom = textRect.top + nCharHeight;//nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].fTotalLoss)//为0时显示"-"wcq2012.07.09
			{
				strText.Format(_T("--"));//总损耗
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fTotalLoss);//总损耗
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 斜率
		{
			textRect.left = nLeft + 5 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 6 * nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace;
			//斜率
			if (fabs(ve[i].fLineK)>0.5)//zll 2012.10.31
			{
				strText.Format(_T("--"));
			}
			else if (0 == ve[i].fLineK)//为0时显示"-" zll 2012.10.31
			{
				strText.Format(_T("--"));
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fLineK);//斜率
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}
		// 反射
		{
			textRect.left = nLeft + 6 * nHspace;
			textRect.top = nTop + nStartLine * (nVspace + nCharHeight);
			textRect.right = nLeft + 7 * nHspace;
			textRect.bottom = textRect.top + nCharHeight; //nTop + (nStartLine+1) * nVspace;
			if (0 == ve[i].nEventType)//反射
			{
				strText=_T("--");
			}
			else if (0 == ve[i].fEventHigh)//为0时显示"-"wcq2012.07.09
			{
				strText=_T("--");
			}
			else
			{
				strText.Format(_T("%.3f"),ve[i].fEventHigh);//反射
			}
			pDC->DrawText(strText, &textRect,DT_CENTER);//文字
		}

		nStartLine++; //行数加1
	}
	pDC->SelectObject (pOldFont);
}

/****************************************************************
函数名称:             GetEventInfo2Way()                                                                               
函数说明:             获取双向列表时间信息                                                                            
入口参数:             nIndex 索引， strText 显示文字                                                                   
出口参数:             无                                                                                        
建立时间:             2016-05-03                                                                  
Write By:             ghq                                                                                          
****************************************************************/
void CMyView::GetEventInfo2Way(INT nIndex,CString &strText,INT nType)
{
	CString strTemp;
	float fLossAvg = 0;
	//以下获取A-B线的事件数据
	std::vector<EventInfo> vAB;
	UINT uiEvents = m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo.size();  //B->A线的总事件数
	UINT uiOffset = m_stPrintDetail.stTestResultInfo[1].DataPointsNum - m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo[uiEvents - 1].index;   //总点数-最后事件的索引
	for(UINT i = 0; i != m_stPrintDetail.stTestResultInfo[0].EventList.vEventInfo.size(); i++ ) //先把第一条线的事件放入
	{
		vAB.push_back(m_stPrintDetail.stTestResultInfo[0].EventList.vEventInfo[i]);
		vAB[i].index += uiOffset;  //将数据点平移一下
	}

	//以下获取B-A线的事件数据
	std::vector<EventInfo *> vBA;
	float fOffset = m_stPrintDetail.stTestResultInfo[1].Range -  m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo[uiEvents -1].fLocation;   //总距离-最后事件的位置
	for(UINT i = 0; i != uiEvents; i++ )  //再把第二条线的数据放入临时数组
	{
		EventInfo *pEvent = new EventInfo();
		*pEvent = m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo[uiEvents -1 - i];
		pEvent->fLocation = m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo[uiEvents -1].fLocation -  m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo[uiEvents -1 - i].fLocation;  //第一个事件为0
		pEvent->index = m_stPrintDetail.stTestResultInfo[1].DataPointsNum - m_stPrintDetail.stTestResultInfo[1].EventList.vEventInfo[uiEvents - 1 - i].index;	 //倒向了，第一个事件不为0	      

		vBA.push_back(pEvent);
	}

	//strTemp.Format(_T("%.3f"), 0.0);
	strTemp.Format(_T("--"));//为0时显示"-"

	for(std::vector<EventInfo>::size_type j=0; j!=vAB.size();j++)
	{
		if(abs(1.0 * vAB[j].index - m_stPrintDetail.stTestResultInfo[2].EventList.vEventInfo[nIndex].index) <= 5)
		{
		    if (0 == vAB[j].fEventloss)//为0时显示"-"
		    {
				strTemp.Format(_T("--"));//总损耗
			}
			else
			{
				strTemp.Format(_T("%.3f"), vAB[j].fEventloss);
			}
			fLossAvg = vAB[j].fEventloss;
			break;
		}
	}

	if(nType == 0)
		strText = strTemp;

	//strTemp.Format(_T("%.3f"), 0.0);
	strTemp.Format(_T("--"));//为0时显示"-"

	//以下获取B->A LOSS
	for(std::vector<EventInfo>::size_type j=0; j!=vBA.size();j++)
	{
		if(abs(1.0 * vBA[j]->index - m_stPrintDetail.stTestResultInfo[2].EventList.vEventInfo[nIndex].index) <= 5)
		{
		    if (0 == vBA[j]->fEventloss)//为0时显示"-"
			{
				strTemp.Format(_T("--"));//总损耗
			}
			else
			{
				strTemp.Format(_T("%.3f"), vBA[j]->fEventloss);
			}
			fLossAvg += vBA[j]->fEventloss;
			break;
		}
	}

	if(nType == 1)
		strText = strTemp;

	//以下给出平均损耗
	if (0 == (fLossAvg / 2.0))//为0时显示"-"
	{
		strTemp.Format(_T("--"));//总损耗
	}
	else
	{
		strTemp.Format(_T("%.3f"), fLossAvg / 2.0);
	}

	if(nType == 2)
		strText = strTemp;

	vAB.clear();
	vBA.clear();
}

void CMyView::MemcpyQuestInfo(PrintQuest *pQuestInfo)
{
	//
	m_stPrintDetail.stPrintQuest.QuestID = pQuestInfo->QuestID;
	m_stPrintDetail.stPrintQuest.Contractor = pQuestInfo->Contractor;
	m_stPrintDetail.stPrintQuest.Customer = pQuestInfo->Customer;
	m_stPrintDetail.stPrintQuest.Player = pQuestInfo->Player;
	m_stPrintDetail.stPrintQuest.Comment = pQuestInfo->Comment;
}

/***************************************************************
函数名称:             DrawCursorAB()                                                                               
函数说明:             画光标AB                                                                              
入口参数:             CDC                                                                                           
出口参数:             无                                                                                        
建立时间:             2014-03-03                                                                  
Write By:             XJF                                                                                          
****************************************************************/
void CMyView::DrawCursorAB(CDC* pDC, CRect& rect)
{
	if(!g_cursorInfo.bDrawAB)
		return;
	//绘制AB线
	INT nMarginTop = rect.Height() / 10;//lzy2014.8.26
	CPen linePen;
	linePen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen *oldPen = (CPen*)pDC->SelectObject(&linePen);
	CRect rectHead;  //画个方框，把AB圈起来lzy2014.7.18
	int nHead = 2;//lzy2014.7.18
	//A线
	if(g_cursorInfo.fRatioA > 0)
	{
		INT ndxA = rect.left + g_cursorInfo.fRatioA * rect.Width();
		pDC->MoveTo(ndxA, rect.top + nMarginTop);
		pDC->LineTo(ndxA, rect.bottom);
		CString strText;
		strText.Format(_T("%s"), g_cursorInfo.strA);//距离及损耗不在此处显示lzy2014.7.18, g_cursorInfo.strDistanceA,g_cursorInfo.strDBA);
		CSize sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
		CRect rectText;
		rectText.top = rect.top + nMarginTop;
		rectText.bottom = rectText.top + sizeText.cy;		

		if(ndxA + sizeText.cx + 1 <= rect.right)//文本正常显示，在右边,1：文本和竖线间隔
		{
			rectText.left = ndxA + 1;
			rectText.right = ndxA + 1 + sizeText.cx;
		}
		else//移动到右边，若文本显示不下，则把文本显示到左边
		{
			//rectText.left = ndxA - (sizeText.cx + 1);
			//rectText.right = ndxA - 1;
			
		}//eif

		rectText.left = ndxA - sizeText.cx / 2 + 1;
		rectText.right = rectText.left + sizeText.cx + 1;

		rectHead.SetRect(rectText.left - nHead, rectText.top - nHead / 2, rectText.right + nHead, rectText.bottom + nHead / 2); //lzy2014.7.18
		pDC->Rectangle(&rectHead);//lzy2014.7.18
		pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
	}
	//B线
	if(g_cursorInfo.fRatioB > 0)
	{
		//nMarginTop += 80;
		INT ndxB = rect.left + g_cursorInfo.fRatioB * rect.Width();
		pDC->MoveTo(ndxB, rect.top + nMarginTop);
		pDC->LineTo(ndxB, rect.bottom);
		CString strText;
		strText.Format(_T("%s"), g_cursorInfo.strB);//距离及损耗不在此处显示lzy2014.7.18, g_cursorInfo.strDistanceB,g_cursorInfo.strDBB);
		CSize sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
		CRect rectText;
		rectText.top = rect.top + nMarginTop;
		rectText.bottom = rectText.top + sizeText.cy;

		rectText.left = ndxB - sizeText.cx / 2 + 1;
		rectText.right = rectText.left + sizeText.cx + 1;

		rectHead.SetRect(rectText.left - nHead, rectText.top - nHead / 2, rectText.right + nHead, rectText.bottom + nHead / 2); //lzy2014.7.18
		pDC->Rectangle(&rectHead);//lzy2014.7.18
		pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
	}

	pDC->SelectObject(oldPen);
	DeleteObject(&linePen);

	/*
	g_sorFileArray.reportConfig.Trace.LocationA = g_cursorInfo.strDistanceA;   //A标位置
	g_sorFileArray.reportConfig.Trace.LossA = g_cursorInfo.strDBA;  //A标损耗
	g_sorFileArray.reportConfig.Trace.LocationB = g_cursorInfo.strDistanceB;   //B标位置
	g_sorFileArray.reportConfig.Trace.LossB = g_cursorInfo.strDBB;  //B标损耗
	float fABdistance = _wtof(g_cursorInfo.strDistanceA.GetBuffer()) - _wtof(g_cursorInfo.strDistanceB.GetBuffer()); //AB距离 
	g_cursorInfo.strDistanceA.ReleaseBuffer();
	g_cursorInfo.strDistanceB.ReleaseBuffer();
	float fABLoss = abs(_wtof(g_cursorInfo.strDBA.GetBuffer()) - _wtof(g_cursorInfo.strDBB.GetBuffer())); 
	g_cursorInfo.strDistanceA.ReleaseBuffer();
	g_cursorInfo.strDistanceB.ReleaseBuffer();
	float fABslope = fABdistance !=0 ? fABLoss / fABdistance : 0;
	g_sorFileArray.reportConfig.Trace.DistanceAB.Format(_T("%.3f"),fABdistance); //AB标的距离	
	g_sorFileArray.reportConfig.Trace.LossAB.Format(_T("%.2f"), fABLoss); //AB标间的损耗	
	g_sorFileArray.reportConfig.Trace.SlopeAB.Format(_T("%.3f"), fABslope); //AB标间的损耗斜率
	*/
}
