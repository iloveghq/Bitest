// OtdrTrace.cpp : 实现文件
//

#include "stdafx.h"
//#include "IOCP.h"
#include "OtdrTrace.h"
#include "BitestDlg.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
FLOAT CGraphElement::m_fRangeX[2];//保存当前绘图时最小最大距离，针对多条曲线
FLOAT CGraphElement::m_fRangeY[2];//保存当前绘图时y轴范围，针对多条曲线
FLOAT CGraphElement::m_fMaxDistance;//最长距离，针对多条曲线

int g_nTraceYmax;       //lzy2014.06.24
CursorInfo g_cursorInfo;

//CGraphElement
////根据距离转化为对应的数据点下标points[X],一般用来确定每次绘图的起始点及结束点
INT CGraphElement::DistanceToIndex(FLOAT fDistance)
{
	ASSERT(0 != m_fDataSpacing);

	//fDistance += m_fOffsetX;

	if(fDistance>0)
	    return fDistance / m_fDataSpacing;
	else 
		return 0;
}

//绘制曲线
#if 0
void CGraphElement::DrawCurve(CDC* pDC, CRect rect,int nIndexCurve)
{
	pDC->FillSolidRect(&rect,COLOR_BK);
	if (0 >= m_nPointsCount)
	{
		return;
	}
	int nTemp;
	INT nIndex = 0;
	if (m_pPointsToDraw)
	{
		delete[] m_pPointsToDraw;
	}//eif

	m_pPointsToDraw = new CPoint[m_nPointsCount];
	for (int i = 0; i < m_nPointsCount; i++)
	{
		m_pPointsToDraw[nIndex].x = nIndex * rect.Width() / m_nPointsCount;
		m_pPointsToDraw[nIndex].y = rect.bottom - ((m_pData[i] - m_fRangeY[0]) / (m_fRangeY[1] - m_fRangeY[0]) * rect.Height());//m_pData[i] * fTempY + fYConst;
		//TRACE(_T("\t%d \t%d \t%d\n"),nIndex,m_pPointsToDraw[nIndex].x,m_pPointsToDraw[nIndex].y);
		nIndex++;
	}//efor
	CPen pen;//(m_LineColor, PS_SOLID, 1);
	pen.CreatePen(PS_SOLID, 1, m_LineColor);//RGB(255,255,0));
	CPen* pOldPen = pDC->SelectObject(&pen);
	Polyline(pDC->GetSafeHdc(),m_pPointsToDraw, m_nPointsCount);	
	
	//绘制对应的事件列表
	DrawEvents(pDC, rect);//
}
#else
void CGraphElement::DrawCurve(CDC* pDC, CRect rect,int nIndexCurve)
 {
    INT nStartIndex= 0;//曲线的起始点
	INT nEndIndex = 0;
	INT nLineX = 0;
	INT nPointCountToDraw = 0;
	INT nIndex = 0;

	FLOAT fTempX = 0.0;//1每一点占x轴多长
	FLOAT fTempY = 0.0;
	FLOAT fYConst =  0.0;
	CPoint* pPointsToDraw = NULL;
	CPen pen;
	CPen* pOldPen = NULL;

	ASSERT(NULL != m_pData);
	//先计算需要绘制哪些点
	nStartIndex= DistanceToIndex(m_fRangeX[0]);
	nEndIndex = DistanceToIndex(m_fRangeX[1]);
	if (nStartIndex >= m_nPointsCount)
	{
		return;
	}//eif

	if (nEndIndex - nStartIndex < rect.Width())
	{
		nEndIndex++;
	}
	if (nEndIndex >= m_nPointsCount)
	{
		nEndIndex = m_nPointsCount - 1;
	}//eif

	if (nStartIndex < 0)
	{
		return;
	}//eif

	//计算当前曲线水平方向上占多少象素
	nLineX = rect.Width();//
	if (m_fRangeX[1] > m_fLength)
	{//按比例求长度
		nLineX = ((m_fLength - m_fRangeX[0]) / (m_fRangeX[1] - m_fRangeX[0])) * nLineX;
	}//
	
	//计算绘图点
	nPointCountToDraw = nEndIndex - nStartIndex + 1;//总共绘图点

	pPointsToDraw = new CPoint[nPointCountToDraw];
	fTempX = ((FLOAT)nLineX) / nPointCountToDraw;//1每一点占x轴多长
	fTempY = rect.Height() / (m_fRangeY[1] - m_fRangeY[0]);//每一点损耗占Y轴多长
	fYConst =  rect.bottom + rect.Height() * m_fRangeY[0]  / (m_fRangeY[1] - m_fRangeY[0]) + m_nOffsetY;

	for (int i = nStartIndex; i <= nEndIndex; i++)
	{
		pPointsToDraw[nIndex].x = (m_fDataSpacing * i - m_fRangeX[0])*rect.Width() / (m_fRangeX[1] - m_fRangeX[0]);
#ifdef COMPILE_SHOW_SMOOTH_DATA////显示时用平滑过的数据，保存时用实际数据
		pPointsToDraw[nIndex].y = fYConst - m_pDataSmooth[i] * fTempY;//
#else
		pPointsToDraw[nIndex].y = fYConst - m_pData[i] * fTempY;
//不用平移，这样有效显示范围会扩大。lzy2014.7.16
#endif
		nIndex++;
	}//efor

	//处理
	if (m_bSelected)
	{
		pen.CreatePen(PS_SOLID, 1, m_LineColor);
	}
	else
	{
		pen.CreatePen(PS_SOLID, 1, m_LineColor);
	}//
	pOldPen = pDC->SelectObject(&pen);
	Polyline(pDC->GetSafeHdc(),pPointsToDraw, nPointCountToDraw);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	delete[] pPointsToDraw;
	pPointsToDraw = NULL;

	//绘制对应的事件列表
	DrawEvents(pDC, rect);//
	//绘制刻度
	DrawScale(pDC, rect,SHOW_A2B );//
}
#endif

//绘制曲线, 双向测试用
#if 0
void CGraphElement::DrawCurve2Way(CDC* pDC, CRect rect)
{
	//pDC->FillSolidRect(&rect,RGB(0,0,0));
	if (0 >= m_nPointsCount)
	{
		return;
	}
	int nTemp;
	INT nIndex = 0;
	if (m_pPointsToDraw)
	{
		delete[] m_pPointsToDraw;
	}

	m_pPointsToDraw = new CPoint[m_nPointsCount];
	for (int i = 0; i < m_nPointsCount; i++)
	{
		m_pPointsToDraw[nIndex].x = nIndex * rect.Width() / m_nPointsCount;
		m_pPointsToDraw[nIndex].y = rect.bottom - ((m_pData[i] - m_fRangeY[0]) / (m_fRangeY[1] - m_fRangeY[0]) * rect.Height());//m_pData[i] * fTempY + fYConst;
		//TRACE(_T("\t%d \t%d \t%d\n"),nIndex,m_pPointsToDraw[nIndex].x,m_pPointsToDraw[nIndex].y);
		nIndex++;
	}//efor
	CPen pen;//(m_LineColor, PS_SOLID, 1);
	pen.CreatePen(PS_SOLID, 1, m_LineColor);
	CPen* pOldPen = pDC->SelectObject(&pen);
	Polyline(pDC->GetSafeHdc(),m_pPointsToDraw, m_nPointsCount);
	pDC->SelectObject(pOldPen);

	//绘制对应的事件列表
	DrawEvents2Way(pDC, rect);
}
#else
void CGraphElement::DrawCurve2Way(CDC* pDC, CRect rect,int nIndexCurve)
 {
    INT nStartIndex = 0;
	INT nEndIndex = 0;
	INT nLineX = 0;
	INT nPointCountToDraw = 0;
	CPoint* pPointsToDraw = NULL;
	FLOAT fTempX = 0.0;//1每一点占x轴多长
	FLOAT fTempY = 0.0;
	FLOAT fYConst =  0.0;
	FLOAT fRangeXMin = 0.0;
	FLOAT fRangeXMax = 0.0;
	INT nIndex = 0;
	CPen pen;
	CPen* pOldPen = NULL;

	ASSERT(NULL != m_pData);
	//先计算需要绘制哪些点
	if(m_fRangeX[0]<0)
	{
		fRangeXMin = 0;
		fRangeXMax = m_fRangeX[1] - m_fRangeX[0];
	}
	else
	{
		fRangeXMin = m_fRangeX[0];
		fRangeXMax = m_fRangeX[1];
	}
	nStartIndex= DistanceToIndex(fRangeXMin);
	nEndIndex = DistanceToIndex(fRangeXMax);
	if (nStartIndex >= m_nPointsCount)
	{
		return;
	}//eif

	if (nEndIndex - nStartIndex < rect.Width())
	{
		nEndIndex++;
	}
	if (nEndIndex >= m_nPointsCount)
	{
		nEndIndex = m_nPointsCount - 1;
	}//eif

	if (nStartIndex < 0)
	{
		return;
	}//eif

	//计算当前曲线水平方向上占多少象素
	nLineX = rect.Width();//
	if (fRangeXMax > m_fLength)
	{//按比例求长度
		nLineX = ((m_fLength - fRangeXMin) / (fRangeXMax - fRangeXMin)) * nLineX;
	}//
	
	//计算绘图点
	nPointCountToDraw = nEndIndex - nStartIndex + 1;//总共绘图点
	pPointsToDraw = new CPoint[nPointCountToDraw];
	fTempX = rect.Width() / (fRangeXMax - fRangeXMin);//1每一点占x轴多长
	fTempY = rect.Height() / (m_fRangeY[1] - m_fRangeY[0]);
	fYConst =  rect.bottom + rect.Height() * m_fRangeY[0]  / (m_fRangeY[1] - m_fRangeY[0]) + m_nOffsetY;

	for (int i = nStartIndex; i <= nEndIndex; i++)
	{
		pPointsToDraw[nIndex].x = (m_fDataSpacing * i - m_fRangeX[0])*rect.Width() / (m_fRangeX[1] - m_fRangeX[0]);//(m_fDataSpacing * i + fRangeXMin)*fTempX;
#ifdef COMPILE_SHOW_SMOOTH_DATA////显示时用平滑过的数据，保存时用实际数据
		pPointsToDraw[nIndex].y = fYConst - m_pDataSmooth[i] * fTempY;//
#else
		pPointsToDraw[nIndex].y = fYConst - m_pData[i] * fTempY;
//不用平移，这样有效显示范围会扩大。lzy2014.7.16
#endif
		nIndex++;
	}//efor
	//处理

	if (m_bSelected)
	{
		pen.CreatePen(PS_SOLID, 1, m_LineColor);
	}
	else
	{
		pen.CreatePen(PS_SOLID, 1, m_LineColor);
	}//
	pOldPen = pDC->SelectObject(&pen);
	Polyline(pDC->GetSafeHdc(),pPointsToDraw, nPointCountToDraw);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	delete[] pPointsToDraw;
	pPointsToDraw = NULL;
}
#endif

//绘制事件点
#if 0
void CGraphElement::DrawEvents(CDC* pDC, CRect rect)
{
	if (0 == m_vAllEvents.size())//无事件列表
	{
		return;
	}//
	
	int nAxisX;
	int nAxisY;
	CPen penAxisY;
	int left = 0,top = 0,right,bottom;//左上
	right = rect.Width();
	bottom = rect.Height();
		//画事件点
	int pos;//事件点下标，事件点位于第几点
	CPoint pointEventTemp,pointBegin,pointEnd;//直线两端
	CRect rectTemp,rectLineBottom;//
	rectTemp.bottom = bottom - EVENT_POINT_NUM_HEIGHT;//
	rectTemp.top = rectTemp.bottom - 15;//
	CString temp;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(COLOR_EVENT_NUM);//
	CPen penEventNum;
	penEventNum.CreatePen(PS_SOLID,1,COLOR_EVENT_RECT);
	CPen penEvent;
	penEvent.CreatePen(PS_SOLID,1,COLOR_NOSELECT_EVENT);
	CPen* pOldPen = pDC->SelectObject(&penEvent);

	CPen penSelEvent;
	penSelEvent.CreatePen(PS_SOLID,1,COLOR_SELECT_EVENT);
	for (int i=0; i<m_vAllEvents.size(); i++)
	{
		pos = m_vAllEvents[i].m_nIndex;//第几点
		//得到位置
		nAxisX = pos * rect.Width() / m_nPointsCount;
		nAxisY = rect.bottom - ((m_pData[pos] - m_fRangeY[0]) / (m_fRangeY[1] - m_fRangeY[0]) * rect.Height());//
		
		//计算坐标
		pointBegin.y = nAxisY - EVENT_POINT_HEIGHT / 2;
		pointEnd.y = nAxisY + EVENT_POINT_HEIGHT / 2;
		//ghq 2016.4.12 选中事件
		if(m_iSelEventLine == i)
			pDC->SelectObject(&penSelEvent);
		else
		    pDC->SelectObject(&penEvent);//wcq2011.12.20
		//直线
		
		pDC->MoveTo(nAxisX,pointBegin.y);
		pDC->LineTo(nAxisX,pointEnd.y);
		//下方矩形
		rectLineBottom.SetRect(nAxisX - 2,nAxisY + EVENT_POINT_HEIGHT / 2,nAxisX + 2,nAxisY + EVENT_POINT_HEIGHT / 2 + 4);

		//ghq 2016.4.12 选中事件
		if(m_iSelEventLine == i)
		{
		    pDC->FillSolidRect(&rectLineBottom,COLOR_SELECT_EVENT);
			pDC->SelectObject(&penSelEvent);//
		}
		else
		{
		    pDC->FillSolidRect(&rectLineBottom,COLOR_NOSELECT_EVENT);
			pDC->SelectObject(&penEventNum);//
		}
		
		//第几个
		rectTemp.left = nAxisX - 6;
		rectTemp.right = rectTemp.left + 13;//被加数表示矩形宽度
		if (i>8)//两位数
		{
			rectTemp.right += 8;
		}
		pDC->Rectangle(rectTemp);//后面矩形框
		rectTemp.left +=1;
		temp.Format(_T("%d"),i + 1);
		pDC->DrawText(temp,&rectTemp,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}//efor
	pDC->SelectObject(pOldPen);
}
#else
void CGraphElement::DrawEvents(CDC* pDC, CRect rect)
{
	INT nPos = 0;
	INT nLineBottomX = 0;
	INT nLineBottomY = 0;
	INT nStartIndex = 0;
	INT nEndIndex = 0;
    FLOAT fTempY = 0;
	FLOAT fYConst = 0;
	CPen penEvent;
	CPen penSelEvent;
	CPen penLineVertical;//竖直线
	CFont   font;   
	CFont   fontValue;  
	CFont   *pOldFont = NULL;; 
	CFont   *pOldFontValue;  
	CBrush *pOldBrush = NULL;
	CPen* pOldPen = NULL;
	CString temp;
	CRect rectTemp;

	//绘事件列表
	if (!m_bShowEvent)
	{
		return;
	}//eif

	fTempY = rect.Height() / (m_fRangeY[1] - m_fRangeY[0]);
	fYConst =  rect.bottom + rect.Height() * m_fRangeY[0]  / (m_fRangeY[1] - m_fRangeY[0]) + m_nOffsetY;

	penEvent.CreatePen(PS_SOLID,1,COLOR_NOSELECT_EVENT);
	penSelEvent.CreatePen(PS_SOLID,1,COLOR_SELECT_EVENT);
		//先计算需要绘制哪些点
	nStartIndex= DistanceToIndex(m_fRangeX[0]);
	nEndIndex = DistanceToIndex(m_fRangeX[1]);

	////zll 2013.5.11 以下是A或B标旁边的字体属性
	pDC->SetTextColor(COLOR_EVENT_RECT);//zll 2013.5.12
	pDC->SetBkMode(TRANSPARENT);

	font.CreateFont(16, 0, 900, 900, FW_NORMAL,//从上往下2700,字太大了，改小点。lzy2014.7.15
		0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH,_T("@Arial"));
	pOldFont = pDC->SelectObject(&font);

	////zll 2013.5.11 以下是新建矩形框中的数值的字体属性 
	fontValue.CreateFont(16, 0, 0, 0, FW_BOLD,//从上往下2700
		0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH,_T("@Arial")); 
	pOldFontValue = pDC->SelectObject(&fontValue);

	penLineVertical.CreatePen(PS_SOLID, 1, COLOR_EVENT_RECT);

	pOldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
	pOldPen = pDC->SelectObject(&penLineVertical);
	rectTemp.SetRectEmpty();//zll 2013.5.24 初始化A或B标旁边的字显示框，否则位置乱掉，release版无法显示A或B标旁边的字
	for (int i=0; i<m_vAllEvents.size(); i++)
	{		
		CRect rectLineBottom;//rectLineBottom:直线下方的矩形wcq2011.11.16
		nPos = m_vAllEvents[i].m_nIndex;

		if ( (nPos >= nStartIndex ) &&(nPos <= nEndIndex))
		{
			nLineBottomX = (m_vAllEvents[i].m_distance - m_fRangeX[0])*rect.Width() / (m_fRangeX[1] - m_fRangeX[0]);
			nLineBottomY = fYConst - m_pData[nPos] * fTempY;   //lzy2014.7.16

			//ghq 2016.4.12 选中事件
		    if(m_iSelEventLine == i)
			    pDC->SelectObject(&penSelEvent);
		    else
		        pDC->SelectObject(&penEvent);

			//画竖直线
			pDC->MoveTo(nLineBottomX, nLineBottomY + LINE_TOP);//使竖直线沿y轴向上移动
			pDC->LineTo(nLineBottomX, nLineBottomY + LINE_BOTTOM);
			//竖直线下面的正方形
			rectLineBottom.SetRect(nLineBottomX - LINE_BOTTOM_OFFSET_X, nLineBottomY + LINE_BOTTOM, 
				nLineBottomX - LINE_BOTTOM_OFFSET_X + LINE_BOTTOM_WIDTH, nLineBottomY + LINE_BOTTOM + LINE_BOTTOM_HEIGHT);
			if (i>8)//两位数
			{
				rectLineBottom.right += 2;
			}
			pDC->Rectangle(&rectLineBottom);

			temp.Format(_T("%d"),i + 1);
			pDC->SelectObject(&fontValue);////zll 2013.5.11 以下是A或B标旁边的字体属性
			rectLineBottom.DeflateRect(2,2);
			pDC->DrawText(temp,&rectLineBottom,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//矩形框中的数值

			/*
			//距离文本
			float fVal=m_vAllEvents[i].m_distance/DIVISOR_TOKM;
			if(_T("m")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为km
			{
				//temp.Format(_T("%.3fkm"), fVal);
			}
			else if(_T("mi")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为miles
			{
				fVal=m_util.KmeterToOther(fVal,UNIT_SHOW_MILE);
				temp.Format(_T("%.3fmiles"), fVal);//zll 2013.5.9
			}
			else if(_T("ft")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为kft
			{
				fVal=m_util.KmeterToOther(fVal,UNIT_SHOW_KFEET);
				temp.Format(_T("%.3fkft"), fVal);//zll 2013.5.9
			}

			CSize sizeText = pDC->GetTextExtent(temp);//得到文本长宽度
			pOldFont = pDC->SelectObject(&font);
			pDC->DrawText(temp, -1, rectTemp, DT_WORDBREAK | DT_CENTER | DT_CALCRECT);
			pDC->TextOut(fX - sizeText.cy / 2, rectLineBottom.bottom + sizeText.cx, temp);
*/
		}//eif
	}//efor

	pDC->SelectObject(&pOldFont);
	pDC->SelectObject(pOldPen);
	font.DeleteObject();
	fontValue.DeleteObject();
	penLineVertical.DeleteObject();
	penEvent.DeleteObject();
	penSelEvent.DeleteObject();
}
#endif

//绘制事件点,双向测试
#if 0
void CGraphElement::DrawEvents2Way(CDC* pDC, CRect rect)
{
	if (0 == m_vAllEvents.size())//无事件列表
	{
		return;
	}//
	int nAxisX;
	int nAxisY;
	CPen penAxisY;
	int left = 0,top = 0,right,bottom;//左上
	right = rect.Width();
	bottom = rect.Height();
		//画事件点
	int pos;//事件点下标，事件点位于第几点
	CPoint pointEventTemp,pointBegin,pointEnd;//直线两端
	CRect rectTemp,rectLineBottom;//
	rectTemp.bottom = bottom - EVENT_POINT_NUM_HEIGHT;//
	rectTemp.top = rectTemp.bottom - 15;//
	CString temp;
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0x30, 0x30, 0x30));//
	CPen penEventNum;
	penEventNum.CreatePen(PS_SOLID,1,RGB(128,64,64));
	CPen penEvent;
	penEvent.CreatePen(PS_SOLID,1,RGB(255,0,0));
	CPen* pOldPen = pDC->SelectObject(&penEvent);
	for (int i=0; i<m_vAllEvents.size(); i++)
	{
		pos = m_vAllEvents[i].m_nIndex;//第几点
		//得到位置
		nAxisX = pos * rect.Width() / m_nPointsCount;
		//nAxisY = rect.bottom - ((m_pData[pos] - m_fRangeY[0]) / (m_fRangeY[1] - m_fRangeY[0]) * rect.Height());//
		
		//计算坐标
		pointBegin.y = rect.top + rect.Height() / 6;
		pointEnd.y = rect.top + rect.Height() * 5 / 6;
		pDC->SelectObject(&penEvent);//wcq2011.12.20
		//直线
		
		pDC->MoveTo(nAxisX,pointBegin.y);
		pDC->LineTo(nAxisX,pointEnd.y);


		//下方矩形
		//rectLineBottom.SetRect(nAxisX - 2,nAxisY + EVENT_POINT_HEIGHT / 2,nAxisX + 2,nAxisY + EVENT_POINT_HEIGHT / 2 + 4);

		//pDC->FillSolidRect(&rectLineBottom,RGB(255,0,0));

		pDC->SelectObject(&penEventNum);//
		//第几个
		rectTemp.left = nAxisX - 6;
		rectTemp.right = rectTemp.left + 13;//被加数表示矩形宽度
		if (i>8)//两位数
		{
			rectTemp.right += 8;
		}
		pDC->Rectangle(rectTemp);//后面矩形框
		rectTemp.left +=1;
		temp.Format(_T("%d"),i + 1);
		pDC->DrawText(temp,&rectTemp,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}//efor
	pDC->SelectObject(pOldPen);
}
#else
void CGraphElement::DrawEvents2Way(CDC* pDC, CRect rect)
{
    INT nStartIndex = 0;
	INT nEndIndex = 0;
	INT nXAxis = 0;
	INT nIndex = 0;
	INT nX = 0;
	INT nY = 0;
	INT nPos = 0;
	FLOAT fRangeXMin = 0.0;
	FLOAT fRangeXMax = 0.0;

	CPen pen;
	CPen* pOldPen = NULL;
	CPen penEvent;
	CPen penSelEvent;
	CPen penLineVertical;//竖直线
	CFont   font;   
	CFont   fontValue;   
	CFont   *pOldFont = NULL;
	CFont   *pOldFontValue = NULL;
	CString temp;
	CRect rectTemp;
	INT iTop = 0;
	INT iBottom = 0;

	//绘事件列表
	if (!m_bShowEvent)
	{
		return;
	}//eif

	//先计算需要绘制哪些点
	if(m_fRangeX[0]<0)
	{
		fRangeXMin = 0;
		fRangeXMax = m_fRangeX[1] - m_fRangeX[0];
	}
	else
	{
		fRangeXMin = m_fRangeX[0];
		fRangeXMax = m_fRangeX[1];
	}

	nStartIndex= DistanceToIndex(fRangeXMin);
	nEndIndex = DistanceToIndex(fRangeXMax);
	////zll 2013.5.11 以下是A或B标旁边的字体属性
	pDC->SetTextColor(COLOR_EVENT_RECT);//zll 2013.5.12
	pDC->SetBkMode(TRANSPARENT);

	font.CreateFont(16, 0, 900, 900, FW_NORMAL,//从上往下2700,字太大了，改小点。lzy2014.7.15
		0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH,_T("@Arial"));
	pOldFont = pDC->SelectObject(&font);

	////zll 2013.5.11 以下是新建矩形框中的数值的字体属性
	fontValue.CreateFont(16, 0, 0, 0, FW_BOLD,//从上往下2700
		0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH,_T("@Arial")); 
	pOldFontValue = pDC->SelectObject(&fontValue);

	penLineVertical.CreatePen(PS_SOLID, 1, COLOR_EVENT_RECT);
	penEvent.CreatePen(PS_SOLID,1,COLOR_NOSELECT_EVENT);
	penSelEvent.CreatePen(PS_SOLID,1,COLOR_SELECT_EVENT);

	CBrush *pOldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
	pOldPen = pDC->SelectObject(&penLineVertical);

	iTop = rect.top + rect.Height() / 6;
	iBottom = rect.top + rect.Height() *5/ 6;
	rectTemp.SetRectEmpty();//zll 2013.5.24 初始化A或B标旁边的字显示框，否则位置乱掉，release版无法显示A或B标旁边的字
	for (int i=0; i<m_vAllEvents.size(); i++)
	{		
		CRect rectLineBottom;//rectLineBottom:直线下方的矩形wcq2011.11.16
		nPos = m_vAllEvents[i].m_nIndex;

		if ( (nPos >= nStartIndex ) &&(nPos <= nEndIndex))
		{
			nX = (m_vAllEvents[i].m_distance - m_fRangeX[0] - m_fOffsetX)*rect.Width() / (m_fRangeX[1] - m_fRangeX[0]);
			//nY = fYConst - m_pData[nPos] * fTempY;   //lzy2014.7.16
			//nY = rect.top + rect.Height() / 6;

			//ghq 2016.4.12 选中事件
		    if(m_iSelEventLine == i)
			    pDC->SelectObject(&penSelEvent);
		    else
		        pDC->SelectObject(&penEvent);

			//画竖直线
			pDC->MoveTo(nX, nY + iTop);//使竖直线沿y轴向上移动
			pDC->LineTo(nX, nY + iBottom);
			//竖直线下面的正方形
			rectLineBottom.SetRect(nX - LINE_BOTTOM_OFFSET_X,nY + iBottom,
				nX - LINE_BOTTOM_OFFSET_X + LINE_BOTTOM_WIDTH, nY + iBottom + LINE_BOTTOM_HEIGHT);

			if (i>8)//两位数
			{
				rectLineBottom.right += 2;
			}
			pDC->Rectangle(&rectLineBottom);

			temp.Format(_T("%d"),i + 1);
			pDC->SelectObject(&fontValue);////zll 2013.5.11 以下是A或B标旁边的字体属性
			rectLineBottom.DeflateRect(2,2);
			pDC->DrawText(temp,&rectLineBottom,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//矩形框中的数值

			/*
			//距离文本
			float fVal=m_vAllEvents[i].m_distance/DIVISOR_TOKM;
			if(_T("m")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为km
			{
				//temp.Format(_T("%.3fkm"), fVal);
			}
			else if(_T("mi")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为miles
			{
				fVal=m_util.KmeterToOther(fVal,UNIT_SHOW_MILE);
				temp.Format(_T("%.3fmiles"), fVal);//zll 2013.5.9
			}
			else if(_T("ft")==g_sorFileArray.waveConfig.DistUnit)//横坐标单位为kft
			{
				fVal=m_util.KmeterToOther(fVal,UNIT_SHOW_KFEET);
				temp.Format(_T("%.3fkft"), fVal);//zll 2013.5.9
			}

			CSize sizeText = pDC->GetTextExtent(temp);//得到文本长宽度
			pOldFont = pDC->SelectObject(&font);
			pDC->DrawText(temp, -1, rectTemp, DT_WORDBREAK | DT_CENTER | DT_CALCRECT);
			pDC->TextOut(fX - sizeText.cy / 2, rectLineBottom.bottom + sizeText.cx, temp);
*/
		}//eif
	}//efor

	pDC->SelectObject(&pOldFont);
	pDC->SelectObject(pOldPen);
	font.DeleteObject();
	fontValue.DeleteObject();
	pen.DeleteObject();
	penLineVertical.DeleteObject();
	penEvent.DeleteObject();
	penSelEvent.DeleteObject();
}
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// COtdrTrace

IMPLEMENT_DYNAMIC(COtdrTrace, CStatic)

COtdrTrace::COtdrTrace()
{
	m_bLeftButtonDown = false;
	m_nCurMode = CURVE_ZOOM_IN;
	m_rtMouseSelect.SetRectEmpty();
	m_rtCurve.SetRectEmpty();
	m_bInitPaint = FALSE;
	m_nCount=0;

	m_bShowLineAB = FALSE;  //初始化不显示AB标
	m_bShowScale = TRUE;   //初始化显示坐标
	m_cursorA.m_strAOrB = _T("A");
	m_cursorB.m_strAOrB = _T("B");
	m_cursorA.m_strDistance = _T("0km");
	m_cursorB.m_strDistance = _T("0km");
	m_cursorA.m_strDB= _T("0dB");
	m_cursorB.m_strDB= _T("0dB");
	m_nCurABSele = OTDRTRACE_AB_SELE_DISABLE;//默认选中AB关闭
	m_nABSele = OTDRTRACE_AB_SELE_A;
	m_nCurveIndex = 0;
	m_fMaxDistance = 0;
	m_bDrawGrid = TRUE;
	m_iSelMode = 0;
	m_cursorEvent.m_fPos = 0;

	m_nSelectCursor = CURSOR_SELECT::CURSOR_A; //默认选中A
	m_nPointA = 0;
	m_nPointB = 0;
	m_ptMouse.SetPoint(0, 0);
		/*
	m_nMaxIndex = 0;
	m_nStartPoint=0;
	m_nMaxPoint= INVALID_MAX_POINT;//原为0,wcq2012.02.20
	*/
	m_wdGridH = COUNT_GRIDLINE_HORIZONE;  //条数可以根据实际数据自动调整，lzy2014.7.16
	m_wdGridV = COUNT_GRIDLINE_VERTICAL;	
	g_nTraceYmax = OTDR_TRACE_Y_MAX;
}

COtdrTrace::~COtdrTrace()
{
	for(int i=0; i< m_vAllData.size(); i++)
	{
		if (m_vAllData[i].m_pData)
		{
//			delete[] m_vAllData[i].m_pData;
//			m_vAllData[i].m_pData = NULL;
		}//eif
	}//efor
}


BEGIN_MESSAGE_MAP(COtdrTrace, CStatic)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

// COtdrTrace 消息处理程序

void COtdrTrace::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CStatic::OnPaint()
	if (!m_bInitPaint)//初始化DC等
	{
		InitPaint(&dc);
	}//eif
	CDC memDC ;
	CBitmap memBitmap ;
	CBitmap* oldBitmap ; // bitmap originally found in CMemDC

	//CalculateRect();
	memDC.CreateCompatibleDC(&dc) ;
	memBitmap.CreateCompatibleBitmap(&dc, m_rtClient.Width(), m_rtClient.Height()) ;
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap) ;

	//解决闪屏的问题 ghq 2016.5.3
	//dc.FillSolidRect(&m_rtClient, RGB(0,0,0));
	if (memDC.GetSafeHdc() != NULL)
	{
		DoDraw(&memDC);
		if(m_dlgParent->m_bCursorAB)
		ShowCatch(&memDC, m_ptMouse);  //显示鼠标靠近AB标线后的图形。lzy2014.9.9
		dc.BitBlt(0, 0, m_rtClient.Width(), m_rtClient.Height(), &memDC, 0, 0, SRCCOPY); //拷贝内存DC到屏幕
	}

	memDC.SelectObject(oldBitmap);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}
void COtdrTrace::InitPaint(CDC* pDC)
{
	//ASSERT(m_dcGrid.GetSafeHdc());
	//ASSERT(m_dcPlot.GetSafeHdc());
	//DC
	if (!m_dcGrid.GetSafeHdc())
	{
		m_dcGrid.CreateCompatibleDC(pDC);
	}//eif
	if (!m_dcCurve.GetSafeHdc())
	{
		m_dcCurve.CreateCompatibleDC(pDC);
	}//eif

	//内存位图
	if (m_bmpCurve.GetSafeHandle())
	{
		m_bmpCurve.DeleteObject();
	}//eif
	if (m_bmpGrid.GetSafeHandle())
	{
		m_bmpGrid.DeleteObject();
	}//eif
	CalculateRect();
	m_bmpCurve.CreateCompatibleBitmap(pDC, m_rtClient.Width(), m_rtClient.Height());//
	m_bmpGrid.CreateCompatibleBitmap(pDC, m_rtClient.Width(), m_rtClient.Height());//
	m_dcGrid.SelectObject(&m_bmpGrid);
	m_dcCurve.SelectObject(&m_bmpCurve);

	m_bInitPaint = TRUE;
}
void COtdrTrace::DoDraw(CDC* pDC)
{
	//CalculateRect();
	//解决闪屏的问题 ghq 2016.5.3
//	pDC->FillSolidRect(&m_rtClient, RGB(0,0,0));

	TransparentBlt(pDC->GetSafeHdc(), m_rtCurve.left, m_rtCurve.top, m_rtCurve.Width(), m_rtCurve.Height(), m_dcCurve.GetSafeHdc(), 0, 0, m_rtCurve.Width(), m_rtCurve.Height(), RGB(0, 0, 0));
	//pDC->BitBlt(m_rtCurve.left, m_rtCurve.top, m_rtCurve.Width(), m_rtCurve.Height(), &m_dcCurve, 0, 0, SRCCOPY); //曲线。拷贝内存DC到屏幕

	//ghq 2016.4.20
	DrawGrid(pDC);
/*
	switch(m_nCurMode)//根据不同模式
	{
	case CURVE_ZOOM_OUT://缩小
	case CURVE_ZOOM_IN://放大
		DrawMouseSeleRect(pDC);
		break;
	}//eswitch
*/
	//if(m_bShowLineAB)
	DrawLineAB(pDC);//

}
//绘制鼠标拖动矩形
void COtdrTrace::DrawMouseSeleRect(CDC* pDc)
{
	CPen zoomRectPen(PS_SOLID, 1, RGB(255,0,0));
	CPen* oldPen = pDc->SelectObject(&zoomRectPen);
	pDc->MoveTo(m_rtMouseSelect.TopLeft());
	pDc->LineTo(m_rtMouseSelect.right,m_rtMouseSelect.top);
	pDc->LineTo(m_rtMouseSelect.BottomRight());
	pDc->LineTo(m_rtMouseSelect.left,m_rtMouseSelect.bottom);
	pDc->LineTo(m_rtMouseSelect.TopLeft());
	pDc->SelectObject(oldPen);
}
//绘制所有曲线
void COtdrTrace::DrawCurves(CDC* pDC)
{
	pDC->FillSolidRect(&m_rtCurve, RGB(0,0,0));
	for(int i = 0; i < m_vAllData.size(); i++)
	{
		m_vAllData[i].DrawCurve(pDC, m_rtCurve,m_nCurveIndex);
	}//efor
}

//绘制双向测试数据
void COtdrTrace::DrawCurve2Way(CDC* pDC)
{
	pDC->FillSolidRect(&m_rtCurve, RGB(0,0,0));
	for(int i = 0; i < m_vAllData.size(); i++)
	{
		m_vAllData[i].DrawCurve2Way(pDC, m_rtCurve,i);
	}
	if(m_vAllData.size()>1)
	{
	    m_vAllData[1].DrawEvents2Way(pDC, m_rtCurve);
	    m_vAllData[1].DrawScale(pDC, m_rtCurve,SHOW_ALL);
	}
}

void COtdrTrace::DrawGrid(CDC *pDC)
{
	FLOAT ndx = m_rtCurve.Width ()/COUNT_GRIDLINE_HORIZONE;
	FLOAT ndy = m_rtCurve.Height ()/ m_wdGridV;
	INT bottom = m_rtCurve.Height();
	INT left = 0;//sizeText.cx;
	INT iPos = 0;//
	INT iYmax = m_fRangeY[1];//y轴最大值，g_nTraceYmax;
	CPen penGridLine;//
	LOGBRUSH logBrush;

	//如果不需要重绘则退出
	if (!m_bDrawGrid)
	{
		return;
	}
	
	m_wdGridV = COUNT_GRIDLINE_VERTICAL;

	//if (m_ShowGrid)//是否显示栅格
	{
		logBrush.lbColor = COLOR_GRID_BK;
		logBrush.lbStyle = PS_SOLID;
		penGridLine.CreatePen(PS_COSMETIC | PS_ALTERNATE, 1, &logBrush, 0, NULL);
		CPen* pOldPen = pDC->SelectObject(&penGridLine); //

		CString strText = _T("00.00");
		CSize sizeText = pDC->GetTextExtent(strText);//得到文本长宽度

		for ( INT i = 1; i<COUNT_GRIDLINE_HORIZONE; i++)//
		{	
			iPos=left + ndx * i;
			//横坐标刻度从上到下-------------------------------------------------------
			pDC->MoveTo (iPos,bottom);
			pDC->LineTo (iPos,0);
		}//efor
		//纵坐标:::	从左到右
		for (INT j=1; j<m_wdGridV; j++)
		{
			iPos = bottom - ndy * j;
			pDC->MoveTo (left , iPos);
			pDC->LineTo (m_rtCurve.right,iPos);
		}//efor		

		penGridLine.DeleteObject();
		pDC->SelectObject(pOldPen);
	}	
}

void COtdrTrace::OnLButtonDown(UINT nFlags, CPoint point)
{
	//// TODO: 在此添加消息处理程序代码和/或调用默认值

	if(m_rtCurve.PtInRect(point))
	{
		m_nCurABSele = GetABSelectState(point);
		if (OTDRTRACE_AB_SELE_NONE != m_nCurABSele)//点击点在A/B标附近，认为后面即将移动A/B标
		{
			m_bLeftButtonDown = FALSE;
			m_nABSele = m_nCurABSele;

				//点击选中事件
			for(int i = 0; i < m_vAllData.size(); i++)
			{
                m_vAllData[i].SetSelectLine(-1);
			}
			RefreshCurve();
			CButton::OnLButtonDown(nFlags, point);
			return;
		}//eif

		//点击选中事件
		for(int i = 0; i < m_vAllData.size(); i++)
		{
			if(m_vAllData[i].SelectLine(point,m_rtCurve,m_iSelMode))
			{
				m_nABSele = OTDRTRACE_AB_SELE_EVENT;
				m_iSelEvent = m_vAllData[i].GetSelectLine();
				break;
			}
		}
		m_bLeftButtonDown = true;
		m_dlgParent->OnDown(this->GetDlgCtrlID());
		CButton::OnLButtonDown(nFlags, point);
	}
	////CRectTracker tracker;
	////double xmin, xmax, ymin, ymax;
	////TRACE(_T("OnLButtonDown..x:%d y:%d\n"),point.x,point.y);
	////if (tracker.TrackRubberBand(this, point) )
	////{
	////	CPoint pt1, pt2;
	////	tracker.m_rect.NormalizeRect ();
	////	pt1= tracker.m_rect.TopLeft ();
	////	pt2= tracker.m_rect.BottomRight ();
	////	TRACE(_T("OnLButtonDown..TrackRubberBand x:%d y:%d x:%d y:%d\n"),pt1.x,pt1.y,pt2.x,pt2.y);

	////	int xmin = 2;
	////	//Invalidate(false);
	////}this->SetFocus();

}

void COtdrTrace::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_bLeftButtonDown)
	{
		m_nCurABSele = OTDRTRACE_AB_SELE_NONE;
		m_dlgParent->OnResetEventList();
		CButton::OnLButtonUp(nFlags, point);
		return;
	}

	if (OTDRTRACE_AB_SELE_NONE != m_nCurABSele)//拖动AB标
	{
		CButton::OnLButtonUp(nFlags, point);
		return;
	}//eif

	m_dlgParent->OnUp(this->GetDlgCtrlID());
	CButton::OnLButtonUp(nFlags, point);
}

void COtdrTrace::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_rtCurve.PtInRect(point))
	{
		m_ptMouse = point;//lzy2014.9.9
		//移动A/B标
		if (OTDRTRACE_AB_SELE_NONE != m_nCurABSele)//
		{
			CursorABOnMove(point);
			this->InvalidateRect(m_rtCurve);
			CButton::OnMouseMove(nFlags, point);
			return;
		}//eif
	}
	this->InvalidateRect(m_rtCurve);
	CButton::OnMouseMove(nFlags, point);
}

void COtdrTrace::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	CalculateRect();
	if(m_dcCurve.GetSafeHdc())
	{
		CDC *pDC = GetDC(); // Get device context
		InitPaint(pDC);
		if(m_iSelMode == SHOW_ALL)
			DrawCurve2Way(&m_dcCurve);
		else
			DrawCurves(&m_dcCurve);
		ReleaseDC(pDC);
	}//eif
}
//计算绘图区域
void COtdrTrace::CalculateRect()
{
	GetClientRect(m_rtClient);
	m_rtCurve.SetRect(m_rtClient.left,m_rtClient.top,m_rtClient.right,m_rtClient.bottom);
	//m_rtCurve = m_rtClient;
}
void COtdrTrace::SetRange(float xmin, float xmax, float ymin, float ymax, BOOL bSetScrollInof) 
{
	if (xmin == xmax || ymin == ymax)
	{
		return;
	}
	else if (xmin > xmax || ymin > ymax)
	{
		return;
	}//eif

	if (m_vAllData.size() > 0)
	{
		if(m_vAllData[0].DistanceToIndex(xmax) - m_vAllData[0].DistanceToIndex(xmin)< 10)//只有一个点
		{
			return;
		}//eif

		m_vAllData[0].m_fRangeX[0] = xmin;
		m_vAllData[0].m_fRangeX[1] = xmax;
		m_vAllData[0].m_fRangeY[0] = ymin;
		m_vAllData[0].m_fRangeY[1] = ymax;
	}
	else
	{
		return;
	}

	m_fRangeX[0] = xmin;
	m_fRangeX[1] = xmax;
	m_fRangeY[0] = ymin;
	m_fRangeY[1] = ymax;

	m_fResolutionX = (m_fRangeX[1] - m_fRangeX[0]) / m_rtCurve.Width();
	m_fResolutionY = (m_fRangeY[1] - m_fRangeY[0]) / m_rtCurve.Height();
	if(m_iSelMode == SHOW_ALL)
		DrawCurve2Way(&m_dcCurve);
	else
	    DrawCurves(&m_dcCurve);

	//ghq 2016.4.14
	//if (bSetScrollInof)
	//{
	//	SetMyScrollInfo(xmin, xmax,ymin,ymax);
	//}

	Invalidate(FALSE);
}
//清空所有数据2013.08.14
void COtdrTrace::ReleaseAllData()
{
	for(int i = 0; i < m_vAllData.size(); i++)
	{
		if(NULL != m_vAllData[i].m_pData)
		{
			delete[] m_vAllData[i].m_pData;
			m_vAllData[i].m_pData = NULL;
		}
		if(NULL != m_vAllData[i].m_pPointsToDraw)
		{
			delete[] m_vAllData[i].m_pPointsToDraw;
			m_vAllData[i].m_pPointsToDraw = NULL;
		}
		m_vAllData[i].m_vAllEvents.clear();
	}//efor
	m_vAllData.clear();
}

//ghq 2016.4.12 选中事件
BOOL CGraphElement::SelectLine( CPoint  ptClick ,CRect rtCurve, INT iSelMode)
{
	INT pos;//事件点下标，事件点位于第几点
	INT nLineBeginX = 0;
	INT	nLineBeginY = 0,nLineEndY = 0;	
	FLOAT fTempY = rtCurve.Height() / (m_fRangeY[1] - m_fRangeY[0]);
	FLOAT fYConst =  rtCurve.bottom + rtCurve.Height() * m_fRangeY[0]  / (m_fRangeY[1] - m_fRangeY[0]) + m_nOffsetY;
	BOOL bResult = FALSE;
	CRect rtLineBottom,rtLine;//两种矩形
	FLOAT fRangeX = 0;

	if(iSelMode == SHOW_ALL)
	    fRangeX = m_fRangeX[0]+m_fOffsetX;
	else
		fRangeX = m_fRangeX[0];

	for (int i=0; i<m_vAllEvents.size(); i++)
	{		
		pos = m_vAllEvents[i].m_nIndex;//第几点

		//下方矩形
		nLineBeginX = (m_vAllEvents[i].m_distance - fRangeX)*rtCurve.Width() / (m_fRangeX[1] - m_fRangeX[0]);
		if(iSelMode == SHOW_ALL)
		{
		    nLineBeginY = rtCurve.top + rtCurve.Height() / 6;  
			nLineEndY = rtCurve.top + rtCurve.Height() *5/ 6;  
		}
		else
		{
			nLineBeginY = fYConst - m_pData[pos] * fTempY;
			nLineEndY = nLineBeginY + LINE_BOTTOM;
		}

		rtLine.SetRect(nLineBeginX - OTDR_TRACE_SELNEAR, nLineBeginY + LINE_TOP,
			nLineBeginX + OTDR_TRACE_SELNEAR, nLineEndY);
		rtLineBottom.SetRect(nLineBeginX - LINE_BOTTOM_OFFSET_X,nLineEndY,
			nLineBeginX - LINE_BOTTOM_OFFSET_X + LINE_BOTTOM_WIDTH, nLineEndY + LINE_BOTTOM_HEIGHT);
		if (i>8)//两位数
		{
			rtLineBottom.right += 2;
		}

		//选中任意一种矩形，视为选中事件
		if(rtLineBottom.PtInRect(ptClick)
			||rtLine.PtInRect(ptClick))
		{
			m_iSelEventLine = i;
			bResult = TRUE;
			break;
		}
	}//efor

	return bResult;
}

INT CGraphElement::GetSelectLine( )
{
	return m_iSelEventLine;
}

void CGraphElement::SetSelectLine(INT iSelEventLine)
{
	m_iSelEventLine = iSelEventLine;
}

INT CGraphElement::GetSelectEventPos( )
{
    INT iPos = RETURN_ERROR;

	if(m_vAllEvents.size()!=0&&m_iSelEventLine >= 0&&m_iSelEventLine < m_vAllEvents.size())
	{
	    for(INT i=0;i<m_vAllEvents.size();i++)
	    {
		    iPos = m_vAllEvents[m_iSelEventLine].m_distance;
	    }
	}
	return iPos;
}

//当前是否同时选中AB标，同时移动AB标
BOOL COtdrTrace::IsCurrentTypeAB()
{
	return OTDRTRACE_AB_SELE_BOTH == m_nABType;
}

void COtdrTrace::SetCurrentTypeAB(INT nABType)
{
	m_nABType = nABType;
	Invalidate(FALSE);
}

//根据当前选择的A/B标或者同时选中，计算缩放时的中心偏移
FLOAT COtdrTrace::GetCenterOffset()
{
	if (IsCurrentTypeAB())//当前选中AB标
	{
		return (m_cursorA.m_fPos + m_cursorB.m_fPos)/2;
	}//eif
	if (OTDRTRACE_AB_SELE_A == m_nABSele)//A
	{
		//return (m_cursorA.m_fPos - m_fRangeX[OTDR_TRACE_MIN])/(m_fRangeX[OTDR_TRACE_MAX] + m_fRangeX[OTDR_TRACE_MIN]);
		return m_cursorA.m_fPos;
	}
	else if (OTDRTRACE_AB_SELE_B == m_nABSele)//B
	{
		//return (m_cursorB.m_fPos - m_fRangeX[OTDR_TRACE_MIN])/(m_fRangeX[OTDR_TRACE_MAX] + m_fRangeX[OTDR_TRACE_MIN]);
		return m_cursorB.m_fPos;// - m_fRangeX[OTDR_TRACE_MIN])/(m_fRangeX[OTDR_TRACE_MAX] + m_fRangeX[OTDR_TRACE_MIN]);
	}//eif
	else if (OTDRTRACE_AB_SELE_EVENT == m_nABSele)//选中事件
	{
		INT iEventPos = 0;
		if(m_iSelMode == SHOW_ALL)
            iEventPos = m_vAllData[1].GetSelectEventPos()-m_vAllData[1].m_fOffsetX;
		else
			iEventPos = m_vAllData[0].GetSelectEventPos();

		return iEventPos;
	}

	return 0.0;//(m_cursorA.m_fPos + m_cursorB.m_fPos)/2;//什么也没选的话目前是00
}
//得到某点对应的db值。fPos:某处的位置
//如果比当前最大点数大，如多曲线情况，则以最后一点db值作为返回值
FLOAT COtdrTrace::GetYDB(FLOAT fPos)
{
	if (m_vAllData.size()<m_nCurveIndex + 1)
	{
		return 0;
	}//
	//INT nIndex = FLOAT2POINTX(y);
	INT nIndex = m_vAllData[m_nCurveIndex].DistanceToIndex(fPos);
	if (nIndex >= m_vAllData[m_nCurveIndex].m_nPointsCount)
	{
		nIndex = m_vAllData[m_nCurveIndex].m_nPointsCount - 1;
	}//
	if(nIndex < 0 || nIndex > m_vAllData[m_nCurveIndex].m_nPointsCount) //解决AB标同时移动时超范围的问题。lzy2014.9.10
		return 0;

	return m_vAllData[m_nCurveIndex].m_pData[nIndex];
}
//水平放大 fPercent放大比例，默认2%
void COtdrTrace::ZoomInHorizonForBig(FLOAT fPercent)
{
	FLOAT fCenter = GetCenterOffset();
	FLOAT m_fRangeXMIN = fCenter - (fCenter - m_fRangeX[0])*(1 - fPercent);
	FLOAT m_fRangeXMAX = fCenter + (m_fRangeX[1] - fCenter)*(1 - fPercent);
	SetRange(m_fRangeXMIN, m_fRangeXMAX, m_fRangeY[0], m_fRangeY[1]);
}
//水平缩小 fPercent放大比例，默认2%
void COtdrTrace::ZoomOutHorizonForSmall(FLOAT fPercent)
{
	FLOAT fCenter = GetCenterOffset();
	FLOAT m_fRangeXMIN = fCenter - (fCenter - m_fRangeX[0])*(1/(1 - fPercent));
	FLOAT m_fRangeXMAX = fCenter + (m_fRangeX[1] - fCenter)*(1/(1 - fPercent));
	SetRange(m_fRangeXMIN, m_fRangeXMAX, m_fRangeY[0], m_fRangeY[1]);
}
//竖直放大 fPercent放大比例，默认2%
void COtdrTrace::ZoomInVertialForBig(FLOAT fPercent)
{
	FLOAT fCenter = GetCenterOffset();//AB标距离
	FLOAT fDb = GetYDB(fCenter);//得到光标处DB值
	FLOAT m_fRangeYMIN = fDb - (fDb - m_fRangeY[0])*(1 - fPercent);
	FLOAT m_fRangeYMAX = fDb + (m_fRangeY[1] - fDb)*(1 - fPercent);
	SetRange(m_fRangeX[0], m_fRangeX[1], m_fRangeYMIN, m_fRangeYMAX);
}
//竖直缩小fPercent放大比例，默认2%
void COtdrTrace::ZoomOutVertialForSmall(FLOAT fPercent)
{
	FLOAT fCenter = GetCenterOffset();//AB标距离
	FLOAT fDb = GetYDB(fCenter);//得到光标处DB值
	FLOAT m_fRangeYMIN = fDb - (fDb - m_fRangeY[0])*(1/(1 - fPercent));
	FLOAT m_fRangeYMAX = fDb + (m_fRangeY[1] - fDb)*(1/(1 - fPercent));
	SetRange(m_fRangeX[0], m_fRangeX[1], m_fRangeYMIN, m_fRangeYMAX);
}
//水平竖直放大 fPercent放大比例，默认2%
void COtdrTrace::ZoomInBothForBig(FLOAT fPercent)
{
	FLOAT fCenter = GetCenterOffset();//AB标距离
	FLOAT fDb = GetYDB(fCenter);//得到光标处DB值
	FLOAT m_fRangeYMIN = fDb - (fDb - m_fRangeY[0])*(1 - fPercent);
	FLOAT m_fRangeYMAX = fDb + (m_fRangeY[1] - fDb)*(1 - fPercent);
	FLOAT m_fRangeXMIN = fCenter - (fCenter - m_fRangeX[0])*(1 - fPercent);
	FLOAT m_fRangeXMAX = fCenter + (m_fRangeX[1] - fCenter)*(1 - fPercent);
	SetRange(m_fRangeXMIN, m_fRangeXMAX, m_fRangeYMIN, m_fRangeYMAX);
}
//水平竖直缩小 fPercent缩小比例，默认2%
void COtdrTrace::ZoomOutBothForSmall(FLOAT fPercent)
{
	FLOAT fCenter = GetCenterOffset();//AB标距离
	FLOAT fDb = GetYDB(fCenter);//得到光标处DB值
	FLOAT m_fRangeYMIN = fDb - (fDb - m_fRangeY[0])*(1/(1 - fPercent));
	FLOAT m_fRangeYMAX = fDb + (m_fRangeY[1] - fDb)*(1/(1 - fPercent));
	FLOAT m_fRangeXMIN = fCenter - (fCenter - m_fRangeX[0])*(1/(1 - fPercent));
	FLOAT m_fRangeXMAX = fCenter + (m_fRangeX[1] - fCenter)*(1/(1 - fPercent));
	SetRange(m_fRangeXMIN, m_fRangeXMAX, m_fRangeYMIN, m_fRangeYMAX);
}
//还原，水平竖直缩放为0
void COtdrTrace::ZoomReset()
{
	//竖直偏移量为0
	for(int i=0; i<m_vAllData.size(); i++)
	{
		m_vAllData[i].m_nOffsetY = 0;
	}

	SetRange(0, GetMaxDistance(), OTDR_TRACE_Y_MIN, OTDR_TRACE_Y_MAX);//g_nTraceYmax);
//	SetScrollRangeX();//重设水平方向范围
}

void COtdrTrace::MoveToEvent()
{
	FLOAT fCenter = GetCenterOffset();//AB标距离
	FLOAT fDb = GetYDB(fCenter);//得到光标处DB值

	//FLOAT m_fRangeYMIN = fDb - (m_fRangeY[1] - m_fRangeY[0])/2;
	//FLOAT m_fRangeYMAX = fDb + (m_fRangeY[1] - m_fRangeY[0])/2;
	FLOAT m_fRangeXMIN = fCenter - (m_fRangeX[1] - m_fRangeX[0])/2;
	FLOAT m_fRangeXMAX = fCenter + (m_fRangeX[1] - m_fRangeX[0])/2;

	SetRange(m_fRangeXMIN, m_fRangeXMAX, m_fRangeY[0], m_fRangeY[1]);
}

////单击放大，默认放大10%  nPointX：点击的水平x坐标  nPointY：点击的Y坐标 
void COtdrTrace::ZoomInClickForBig(INT nPointX, INT nPointY, FLOAT fPercent)
{
	m_rtMouseSelect.InflateRect(150,150);
	SetRange(POINT2FLOATX(m_rtMouseSelect.left), POINT2FLOATX(m_rtMouseSelect.right), POINT2FLOATY(m_rtMouseSelect.bottom), POINT2FLOATY(m_rtMouseSelect.top));
	
}
//得到最长距离
FLOAT COtdrTrace::GetMaxDistance()
{
	if (m_vAllData.size() > 0)
	{
		return m_vAllData[0].m_fMaxDistance;
	}//eif
	
	return 0;
}

//重设水平方向范围//zll2013.4.23
void COtdrTrace::SetRadioType(INT iSelMode)
{
	m_iSelMode = iSelMode;
}

//绘制刻度lzy2014.7.17
void CGraphElement::DrawScale(CDC *pDC,CRect rtCurve,INT nType)
{
	//如果不需要重绘则退出
	INT iYmax = m_fRangeY[1];
	INT iXmax = m_fRangeX[1];
	INT bottom = rtCurve.Height();
	INT left = 0;
	INT itemp;
	FLOAT fTemp = 0.0;
	FLOAT ndx = rtCurve.Width () / COUNT_GRIDLINE_HORIZONE;
	FLOAT ndy = rtCurve.Height () / COUNT_GRIDLINE_VERTICAL;
	CRect rectText;
	CString strText;
	CSize sizeText;
	CFont   font;   
	CFont   *pOldFont;  

	pDC->SetBkMode(OPAQUE);
	pDC->SetTextColor(COLOR_SCALE_TEXT);
	pDC->SetBkColor(COLOR_SCALE_BK);

	font.CreateFont(15, 0, 0, 0, FW_BOLD,	0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH,_T("Arial")); 
	pOldFont = pDC->SelectObject(&font);

	//画Y轴刻度
	if(iYmax >= 0 && iYmax <= 100)
	{
		rectText.left = rtCurve.left + 5;		
		
		for(int i = 1; i < COUNT_GRIDLINE_VERTICAL; i++)
		{
			itemp = bottom - ndy * i;
			strText.Format(_T("%.1f"), m_fRangeY[0] + (m_fRangeY[1] - m_fRangeY[0]) * i / COUNT_GRIDLINE_VERTICAL);
			sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
			rectText.top = itemp - sizeText.cy / 2;
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx;
			pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
		}

		strText = _T("dB");
		sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
		rectText.top = rtCurve.top + 5;
		rectText.bottom = rectText.top + sizeText.cy;
		rectText.right = rectText.left + sizeText.cx;
		pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
	}

	//画X轴刻度
	if(iXmax > 0)
	{
		rectText.bottom = rtCurve.bottom - 3;
		if(nType != SHOW_ALL)
		{
			for(int i = 1; i < COUNT_GRIDLINE_HORIZONE; i++)
			{
				itemp = left + ndx * i;
				fTemp = 0.001 * m_fRangeX[0] + 0.001 * (m_fRangeX[1] - m_fRangeX[0]) * i / COUNT_GRIDLINE_HORIZONE;
				strText.Format(_T("%.1f"), fTemp);
				sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
				rectText.left = itemp - sizeText.cx / 2;
				rectText.right = rectText.left + sizeText.cx;
				rectText.top = rectText.bottom - sizeText.cy;			
				pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
			}
		}
		else
		{
			ndx = rtCurve.Width () / (COUNT_GRIDLINE_HORIZONE);
			for(int i = 1; i < COUNT_GRIDLINE_HORIZONE; i++)
			{
				itemp = left + ndx * i;
				fTemp = 0.001 * (m_fRangeX[0]+m_fOffsetX) + 0.001 * (m_fRangeX[1] - m_fRangeX[0]) * i / COUNT_GRIDLINE_HORIZONE;
				strText.Format(_T("%.1f"), fTemp);
				sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
				rectText.left = itemp - sizeText.cx / 2;
				rectText.right = rectText.left + sizeText.cx;
				rectText.top = rectText.bottom - sizeText.cy;
				pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
			}
		}
		strText = _T("km");
		sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
		rectText.right = rtCurve.right - 10;
		rectText.left = rectText.right - sizeText.cx;		
		pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);
	}

    pDC->SelectObject(pOldFont);
	font.DeleteObject();
}

void COtdrTrace::RefreshCurve()
{
	if(m_iSelMode == SHOW_ALL)
		DrawCurve2Way(&m_dcCurve);
	else
		DrawCurves(&m_dcCurve);
	InvalidateRect(&m_rtCurve);
}

CEventList::CEventList(void)
{}

CEventList::~CEventList(void)
{}

BEGIN_MESSAGE_MAP(CEventList, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, &CEventList::OnNMClick)
END_MESSAGE_MAP()

void CEventList::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_dlgParent->OnDown(this->GetDlgCtrlID());
	*pResult = 0;
}

void COtdrTrace::SetHorScroll() //设置水平滚动条
{
	//水平滚动条
	SCROLLINFO siHor;
	siHor.cbSize = sizeof(SCROLLINFO);
	siHor.fMask = SIF_ALL; 
	siHor.nMin = 0;
	if (0 != m_fMaxDistance)
	{
		siHor.nMax = m_fMaxDistance;//这么做后windows会把最大的滚动条位置限制为si.nmax-si.npage+1,而不是si.nmax
	}
	else
	{
		siHor.nMax = m_rtCurve.Width();
	}
	//siHor.nMax = 66;
	//siHor.nPage = 22;//m_rectCurve.Width();
	siHor.nMax = m_rtCurve.Width();//zll 2013.4.23
	siHor.nPage = m_rtCurve.Width();//zll 2013.4.23
	siHor.nPos = 0;
	SetScrollInfo(SB_HORZ, &siHor, TRUE);
}

void COtdrTrace::SetVerScoll() //设置竖直滚动条
{
	SCROLLINFO siVer;
	siVer.cbSize = sizeof(SCROLLINFO);
	siVer.fMask = SIF_ALL; 
	siVer.nMin = OTDR_TRACE_Y_MIN;
	siVer.nMax = g_nTraceYmax;
	siVer.nPage = g_nTraceYmax;
	siVer.nPos = 0;
	//this->SetScrollRange( SB_VERT,0,10000);
	SetScrollInfo(SB_VERT, &siVer, TRUE);
}

//滚动条
//初始化滚动条
void COtdrTrace::InitScrollInfo()
{
	m_nHorzMaxSize = m_rtCurve.Width();
	m_nVertMaxSize = g_nTraceYmax * OTDR_TRACE_Y_FACTOR;

	m_nHorzPageSize = m_rtCurve.Width();
	if (0==m_rtCurve.Height())
	{
		return;
	}
	m_nVertPageSize = (g_nTraceYmax - OTDR_TRACE_Y_MIN) * OTDR_TRACE_Y_FACTOR/m_rtCurve.Height();

	m_nHorzOneStep = 0;
	m_nVertOneStep = (g_nTraceYmax - OTDR_TRACE_Y_MIN) * OTDR_TRACE_Y_FACTOR * OTDR_TRACE_DEFAULT_ZOOM_PERCENT;
	m_nHorzPos = 0;
	m_nVertPos = 0;
	//水平
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; 
	si.nMin = 0;
	si.nMax = m_rtCurve.Width();
	si.nPage = m_rtCurve.Width();//
	si.nPos = 0;
	SetScrollInfo(SB_HORZ, &si, TRUE);
	//竖直
	si.fMask = SIF_ALL; 
	si.nMin = OTDR_TRACE_Y_MIN * OTDR_TRACE_Y_FACTOR;
	si.nMax = g_nTraceYmax * OTDR_TRACE_Y_FACTOR;
	si.nPage = (g_nTraceYmax - OTDR_TRACE_Y_MIN) * OTDR_TRACE_Y_FACTOR;//
	si.nPos = 0;
	SetScrollInfo(SB_VERT, &si, TRUE);
	//SetScrollRange(OTDR_TRACE_Y_MIN, g_nTraceYmax);
}

//设置滚动条范围
void COtdrTrace::SetMyScrollInfo(float xmin, float xmax, float ymin, float ymax)
{
	SCROLLINFO si;
	//水平
	GetScrollInfo(SB_HORZ, &si);
	si.nPage = xmax - xmin;
	SetScrollInfo(SB_HORZ, &si, TRUE);

	m_nHorzPos = xmin;
	m_nHorzOneStep = si.nPage * OTDR_TRACE_DEFAULT_ZOOM_PERCENT;
	m_nHorzPageSize = si.nPage;
	SetScrollRange(SB_HORZ, 0, m_fMaxDistance);
	SetScrollPos(SB_HORZ, m_nHorzPos);

	//竖直
	ymin *= OTDR_TRACE_Y_FACTOR;
	ymax *= OTDR_TRACE_Y_FACTOR;
	GetScrollInfo(SB_VERT, &si);
	si.nPage = ymax - ymin;
	//si.nMax = ymax;
	SetScrollInfo(SB_VERT, &si, TRUE);

	m_nVertPos = ymax;
	m_nVertOneStep = (ymax - ymin) * OTDR_TRACE_DEFAULT_ZOOM_PERCENT;
	m_nVertPageSize = ymax - ymin;
	//m_nVertMaxSize = 
	SetScrollPos(SB_VERT, m_nVertPos);
	//
}

void COtdrTrace::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch(nSBCode)
	{
		//////////////case SB_LEFT://按Home触发
		//////////////	m_nHorzPos = 0;
		//////////////	this->SetScrollPos(SB_HORZ,m_nHorzPos);
		//////////////	SetRange(0, m_nHorzPageSize, m_fRangeY[OTDR_TRACE_MIN], m_fRangeY[OTDR_TRACE_MAX], FALSE);
		//////////////	break;
	case SB_LINELEFT:
		if(0 == m_nHorzPos)
		{
			break;
		}
		m_nHorzPos -= m_nHorzOneStep;
		if (m_nHorzPos < 0)
		{
			m_nHorzPos = 0;
		}//

		this->SetScrollPos(SB_HORZ, m_nHorzPos);
		//TRACE(_T("SB_LINELEFT: %d\n"),m_nHorzPos);
		if (0 == m_nHorzPos)
		{
			SetRange(0, m_nHorzPageSize, m_fRangeY[0], m_fRangeY[1], FALSE);
		}
		else
		{
			SetRange(m_fRangeX[0] - m_nHorzOneStep, m_fRangeX[1]  - m_nHorzOneStep, m_fRangeY[0], m_fRangeY[1], FALSE);
		}//eif
		break;
	case SB_PAGELEFT:
		if (m_nHorzPos - m_nHorzPageSize < 0)
		{
			m_nHorzPos = 0;
			this->SetScrollPos(SB_HORZ, 0);
			SetRange(0, m_nHorzPageSize, m_fRangeY[0], m_fRangeY[1], FALSE);
			break;
		}//eif
		m_nHorzPos -= m_nHorzPageSize;
		if(m_nHorzPos<0)
		{
			m_nHorzPos = 0;
		}
		this->SetScrollPos(SB_HORZ,m_nHorzPos);
		SetRange(m_fRangeX[0] - m_nHorzPageSize, m_fRangeX[1]  - m_nHorzPageSize, m_fRangeY[0], m_fRangeY[1], FALSE);
		break;
	case SB_LINERIGHT:
		if (m_nHorzPos + m_nHorzPageSize > m_nHorzMaxSize)
		{
			break;
		}//eif
		m_nHorzPos += m_nHorzOneStep;
		if(m_nHorzPos>m_nHorzMaxSize)
		{
			m_nHorzPos = m_nHorzMaxSize;
		}
		this->SetScrollPos(SB_HORZ,m_nHorzPos);
		//TRACE(_T("SB_LINERIGHT: %d  page: %d max: %d\n"),m_nHorzPos,m_nHorzPageSize,m_nHorzMaxSize);
		SetRange(m_fRangeX[0] + m_nHorzOneStep, m_fRangeX[1] + m_nHorzOneStep, m_fRangeY[0], m_fRangeY[1], FALSE);
		break;
	case SB_PAGERIGHT:
		if (m_nHorzPos + m_nHorzPageSize > m_nHorzMaxSize)
		{
			m_nHorzPos = m_nHorzMaxSize;
			SetRange(m_fMaxDistance - m_nHorzPageSize, m_fMaxDistance, m_fRangeY[0], m_fRangeY[1], FALSE);
			break;
		}//eif
		m_nHorzPos += m_nHorzPageSize;
		this->SetScrollPos(SB_HORZ,m_nHorzPos);
		if (m_nHorzPos + m_nHorzPageSize > m_fMaxDistance)
		{
			SetRange(m_fMaxDistance - m_nHorzPageSize, m_fMaxDistance, m_fRangeY[0], m_fRangeY[1], FALSE);
			break;
		}
		SetRange(m_nHorzPos, m_nHorzPos + m_nHorzPageSize, m_fRangeY[0], m_fRangeY[1], FALSE);
		break;
		//////////////case SB_RIGHT://按End触发
		//////////////	m_nHorzPos = m_nHorzMaxSize;
		//////////////	this->SetScrollPos(SB_HORZ,m_nHorzPos);
		//////////////	SetRange(m_fMaxDistance - m_nHorzPageSize, m_fMaxDistance, m_fRangeY[OTDR_TRACE_MIN], m_fRangeY[OTDR_TRACE_MAX], FALSE);
		//////////////	break;
	case SB_THUMBTRACK://拖动过程中
		SCROLLINFO si;
		GetScrollInfo(SB_HORZ, &si);
		m_nHorzPos = si.nTrackPos;
		this->SetScrollPos(SB_HORZ,m_nHorzPos);
		//TRACE(_T("SB_THUMBTRACK0: %d\n"),m_nHorzPos);
		SetRange(m_nHorzPos, m_nHorzPos + m_nHorzPageSize, m_fRangeY[0], m_fRangeY[1], FALSE);
		break;
	case SB_THUMBPOSITION://拖动结束
		//SCROLLINFO si;
		GetScrollInfo(SB_HORZ, &si);
		m_nHorzPos = si.nPos;
		this->SetScrollPos(SB_HORZ,m_nHorzPos);
		//TRACE(_T("SB_THUMBPOSITION1: %d\n"),m_nHorzPos);
		SetRange(m_nHorzPos, m_nHorzPos + m_nHorzPageSize, m_fRangeY[0], m_fRangeY[1], FALSE);
		break;
	default:
		break;
	}//eofswitch
	CButton::OnHScroll(nSBCode, nPos, pScrollBar);
}
void COtdrTrace::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nSBCode)
	{
		//////////////case SB_LEFT://按Home触发
		//////////////	m_nHorzPos = 0;
		//////////////	this->SetScrollPos(SB_HORZ,m_nHorzPos);
		//////////////	SetRange(0, m_nHorzPageSize, m_fRangeY[OTDR_TRACE_MIN], m_fRangeY[OTDR_TRACE_MAX], FALSE);
		//////////////	break;
		//////////////case SB_RIGHT://按End触发
		//////////////	m_nHorzPos = m_nHorzMaxSize;
		//////////////	this->SetScrollPos(SB_HORZ,m_nHorzPos);
		//////////////	SetRange(m_fMaxDistance - m_nHorzPageSize, m_fMaxDistance, m_fRangeY[OTDR_TRACE_MIN], m_fRangeY[OTDR_TRACE_MAX], FALSE);
		//////////////	break;
	case SB_LINELEFT://向上
		if(0 == m_fRangeY[0])
		{
			break;
		}
		m_nVertPos -= m_nVertOneStep;
		if (m_nVertPos < 0)
		{
			m_nVertPos = 0;
		}//

		this->SetScrollPos(SB_VERT, m_nVertPos);
		//TRACE(_T("SB_LINELEFT: %d\n"),m_nVertPos);
		if (0 == m_nVertPos)
		{
			SetRange(m_fRangeX[0], m_fRangeX[1], OTDR_TRACE_Y_MIN, OTDR_TRACE_Y_MIN + m_nVertPageSize / OTDR_TRACE_Y_FACTOR, FALSE);
		}
		else
		{
			SetRange(m_fRangeX[0], m_fRangeX[1], m_fRangeY[0] + m_nVertOneStep / OTDR_TRACE_Y_FACTOR, m_fRangeY[1] + m_nVertOneStep / OTDR_TRACE_Y_FACTOR, FALSE);
		}//eif
		break;
	case SB_PAGELEFT:
		m_nVertPos -= m_nVertPageSize;
		if(m_nVertPos<0)
		{
			m_nVertPos = 0;
		}
		this->SetScrollPos(SB_VERT,m_nVertPos);
		SetRange(m_fRangeX[0], m_fRangeX[1], m_nVertPos / OTDR_TRACE_Y_FACTOR, (m_nVertPos + m_nVertPageSize) / OTDR_TRACE_Y_FACTOR, FALSE);
		break;
	case SB_LINERIGHT://向下
		if (m_nVertPos == m_nVertMaxSize)
		{
			break;
		}//eif
		m_nVertPos += m_nVertOneStep;
		if(m_nVertPos > m_nVertMaxSize)
		{
			m_nVertPos = m_nVertMaxSize;
		}
		this->SetScrollPos(SB_VERT,m_nVertPos);
		//TRACE(_T("SB_LINERIGHT: %d  page: %d max: %d\n"),m_nVertPos,m_nVertPageSize,m_nVertMaxSize);
		if ((m_nVertMaxSize-m_nVertPageSize) <= m_nVertPos)
			SetRange(m_fRangeX[0], m_fRangeX[1], OTDR_TRACE_Y_MIN, OTDR_TRACE_Y_MIN + (g_nTraceYmax-m_nVertPageSize) / OTDR_TRACE_Y_FACTOR, FALSE);
		else
			SetRange(m_fRangeX[0], m_fRangeX[1], m_fRangeY[0] - m_nVertOneStep / OTDR_TRACE_Y_FACTOR, m_fRangeY[1] - m_nVertOneStep / OTDR_TRACE_Y_FACTOR, FALSE);
		break;
	case SB_PAGERIGHT:
		m_nVertPos += m_nVertPageSize;
		if(m_nVertPos>m_nVertMaxSize)
		{
			m_nVertPos = m_nVertMaxSize;
			this->SetScrollPos(SB_VERT,m_nVertPos);
			//TRACE(_T("SB_LINERIGHT: %d  page: %.2f max: %.2f\n"),m_nVertPos,m_fRangeY[OTDR_TRACE_MIN] - m_nVertPageSize,m_fRangeY[OTDR_TRACE_MAX] - m_nVertPageSize);
			SetRange(m_fRangeX[0], m_fRangeX[1], g_nTraceYmax - m_nVertPageSize / OTDR_TRACE_Y_FACTOR, g_nTraceYmax, FALSE);
			break;
		}//eif
		this->SetScrollPos(SB_VERT,m_nVertPos);
		//TRACE(_T("SB_LINERIGHT: %d  page: %.2f max: %.2f\n"),m_nVertPos,m_fRangeY[OTDR_TRACE_MIN] - m_nVertPageSize,m_fRangeY[OTDR_TRACE_MAX] - m_nVertPageSize);
		SetRange(m_fRangeX[0], m_fRangeX[1], m_nVertPos / OTDR_TRACE_Y_FACTOR, (m_nVertPos + m_nVertPageSize) / OTDR_TRACE_Y_FACTOR, FALSE);
		break;
	case SB_THUMBTRACK://拖动过程中
		SCROLLINFO si;
		GetScrollInfo(SB_VERT, &si);
		m_nVertPos = si.nTrackPos;
		this->SetScrollPos(SB_VERT,m_nVertPos);
		//TRACE(_T("SB_THUMBTRACK0: %d\n"),m_nVertPos);
		SetRange(m_fRangeX[0], m_fRangeX[1], g_nTraceYmax-m_nVertPos / OTDR_TRACE_Y_FACTOR, g_nTraceYmax-(m_nVertPageSize+m_nVertPos) / OTDR_TRACE_Y_FACTOR, FALSE);
		break;
	case SB_THUMBPOSITION://拖动结束
		//SCROLLINFO si;
		GetScrollInfo(SB_VERT, &si);
		m_nVertPos = si.nPos;
		this->SetScrollPos(SB_VERT,m_nVertPos);
		//TRACE(_T("SB_THUMBPOSITION1: %d\n"),m_nVertPos);
		SetRange(m_fRangeX[0], m_fRangeX[1], (  m_nVertPos) / OTDR_TRACE_Y_FACTOR, ( m_nVertPos + m_nVertPageSize) / OTDR_TRACE_Y_FACTOR, FALSE);
		break;
	default:
		break;
	}//eofswitch

	CButton::OnVScroll(nSBCode, nPos, pScrollBar);
}

//重设水平方向范围//zll2013.4.23
void COtdrTrace::SetScrollRangeX()
{
	SetScrollRange(SB_HORZ, 0, m_fMaxDistance);
    //SetScrollRange(SB_VERT, 0, OTDR_TRACE_Y_MAX);
	m_nHorzMaxSize = m_fMaxDistance;
	//m_nVertMaxSize = OTDR_TRACE_Y_MAX*OTDR_TRACE_Y_FACTOR;
}

//设置最长距离
void COtdrTrace::SetMaxDistance(FLOAT fMaxDistance)
{
	m_fMaxDistance=fMaxDistance;
}

void COtdrTrace::SetMaxTraceY(INT nShowType)
{
	if(nShowType != SHOW_ALL)
	    g_nTraceYmax = OTDR_TRACE_Y_MAX;
	else
		g_nTraceYmax = OTDR_TRACE_Y_MAX_2WAY;
}

void COtdrTrace::OnInitAB(void)
{		
//	if(!g_sorFileArray.totalCurveNumber)
//		return;

	m_cursorA.m_nPos = m_rtCurve.left + m_rtCurve.Width() / 2 ;
	m_cursorA.m_fPos = POINT2FLOATX(m_cursorA.m_nPos);

	m_cursorB.m_nPos = m_rtCurve.left + m_rtCurve.Width() / 3;
	m_cursorB.m_fPos = POINT2FLOATX(m_cursorB.m_nPos);
}
//判断是否在当前显示范围内,针对水平方向
BOOL COtdrTrace::IsInCurRangeX(FLOAT fDistance)
{
	return ((fDistance >= m_fRangeX[0]) && (fDistance <= m_fRangeX[1]));
}
//判断是否在当前显示范围内,针对竖直方向
BOOL COtdrTrace::IsInCurRangeY(FLOAT fDistance)
{
	return ((fDistance >= m_fRangeY[0]) && (fDistance <= m_fRangeY[1]));
}
void COtdrTrace:: SetCurveCursorData()
{
	g_cursorInfo.fRatioA = FLOAT2RATIO(m_cursorA.m_fPos);
	g_cursorInfo.strDistanceA = m_cursorA.m_strDistance;
	g_cursorInfo.strDBA = m_cursorA.m_strDB;
	g_cursorInfo.strA = _T("A");

	g_cursorInfo.fRatioB = FLOAT2RATIO(m_cursorB.m_fPos);
	g_cursorInfo.strDistanceB = m_cursorB.m_strDistance;
	g_cursorInfo.strDBB = m_cursorB.m_strDB;
	g_cursorInfo.strB = _T("B");
}
//绘制AB光标
void COtdrTrace::DrawLineAB(CDC* pDC)
{
	g_cursorInfo.bDrawAB = m_bShowLineAB;

	if (!m_bShowLineAB)
	{
		return;
	}//eif
	if (0 == m_vAllData.size())//wcq2016.05.05
	{
		return;
	}
	//A
	if (IsInCurRangeX(m_cursorA.m_fPos))//只有在当前范围内时，才绘制
	{
		DrawLineABDetail(pDC, &m_cursorA);
	}//eif
	if (IsInCurRangeX(m_cursorB.m_fPos))////只有在当前范围内时，才绘制
	{
		DrawLineABDetail(pDC, &m_cursorB);
	}//eif
	//设置全局变量g_CursorInfo;
	SetCurveCursorData();
}
//创建字体
void COtdrTrace::CreateMyFont(CDC* pDC, CFont& font, CString fontName)
{
	CString strFontName = fontName;
	LOGFONT lf;
	SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
	if ("" == strFontName)
	{
		strFontName = lf.lfFaceName;
	}//eif
	font.CreateFont(-::MulDiv(lf.lfWidth, pDC->GetDeviceCaps(LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, 0, 0, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_MODERN, strFontName);//
}
//具体绘制函数.
void COtdrTrace::DrawLineABDetail(CDC* pDC, CursorAB* pCursorAB)
{
	//得到X轴坐标
	CRect rect;
	GetClientRect(&rect);
	INT nX = FLOAT2POINTX(pCursorAB->m_fPos);////根据距离得到X坐标
	CPen pen;//(m_LineColor, PS_SOLID, 1);
	if ((("A" == pCursorAB->m_strAOrB) && (OTDRTRACE_AB_SELE_A == m_nABSele)) || (("B" == pCursorAB->m_strAOrB) && (OTDRTRACE_AB_SELE_B == m_nABSele)))
	{//当前选中
		pen.CreatePen(PS_SOLID, 1, OTDR_TRACE_CURSOR_AB_COLOR_SELE);
	}
	else
	{
		pen.CreatePen(PS_SOLID, 1, pCursorAB->m_colorLine);
	}//eif
	CPen* pOldPen = pDC->SelectObject(&pen);
	INT nMarginTop = OTDR_TRACE_CURSOR_AB_MARGIN_TOP;
	if ("B" == pCursorAB->m_strAOrB)
	{
		//nMarginTop += 20;
		nMarginTop += 0;    //两条线还是等长好，lzy2014.7.14
	}//eif
	//直线
	pDC->MoveTo(nX, rect.top + nMarginTop);
	pDC->LineTo(nX, rect.bottom);

	//文本
	CFont   font;   
	CFont   *pOldFont;   
	/*font.CreateFont(16, 0, 0, 0, FW_NORMAL,	0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,DEFAULT_PITCH,_T("Arial")); */
	CreateMyFont(pDC, font, _T("Arial"));
	pOldFont = pDC->SelectObject(&font);
	pDC->SetTextColor(pCursorAB->m_colorLabel);

	FLOAT fUserOffset = m_vAllData[0].m_fOffsetX;////wcq2015.12.23
	//zll 2013.5.11
	//float fVal=m_cursorA.m_fPos/Divisor_toKm;//cwcq2015.12.25
	float fVal=(m_cursorA.m_fPos + fUserOffset)/DIVISOR_TOKM;//wcq2015.12.25

	m_cursorA.m_strDistance.Format(_T("%.3fkm"), fVal);//zll 2013.5.9


	float fDB=GetYDB(m_cursorA.m_fPos);
	//fDB=(65536-g_sorFileArray.sorFileList[m_nMaxIndex].pTestResultInfo->pDataPoints[m_cursorA.m_nPos])/1000.0;
	m_cursorA.m_strDB.Format(_T("%.2fdB"), fDB);

	//fVal=m_cursorB.m_fPos/Divisor_toKm;//cwcq2015.12.25
	fVal=(m_cursorB.m_fPos + fUserOffset)/DIVISOR_TOKM;//wcq2015.12.25

	m_cursorB.m_strDistance.Format(_T("%.3fkm"), fVal);//zll 2013.5.9


	fDB=GetYDB(m_cursorB.m_fPos);
	//fDB=(65536-g_sorFileArray.sorFileList[m_nMaxIndex].pTestResultInfo->pDataPoints[m_cursorB.m_nPos])/1000.0;
	m_cursorB.m_strDB.Format(_T("%.2fdB"), fDB);

	///*********************zll 2013.6.25  增加A B 光标之间的距离
	fVal=(m_cursorA.m_fPos-m_cursorB.m_fPos)/DIVISOR_TOKM;

	m_cursorAminusB.m_strDistance.Format(_T("%.3fkm"), fVal);//zll 2013.5.9


	/*fDB=abs((65536-g_sorFileArray.sorFileList[m_nMaxIndex].pTestResultInfo->pDataPoints[m_cursorA.m_nPos])/1000.0
		    -(65536-g_sorFileArray.sorFileList[m_nMaxIndex].pTestResultInfo->pDataPoints[m_cursorB.m_nPos])/1000.0);*/
	fDB=abs(GetYDB(m_cursorA.m_fPos)-GetYDB(m_cursorB.m_fPos));
	//fDB = GetYDB(m_cursorA.m_fPos)-GetYDB(m_cursorB.m_fPos);//lzy2014.7.18
	m_cursorAminusB.m_strDB.Format(_T("%.2fdB"), fDB);
	///*********************zll 2013.6.25

	CString strText,strTextAminusB;
	strText.Format(_T("%s %s %s"), pCursorAB->m_strAOrB, pCursorAB->m_strDistance,pCursorAB->m_strDB);
	//strText.Format(_T("%s"), pCursorAB->m_strAOrB);  //在AB线旁边只显示A或B,其他的位置信息显示在右上方lzy2014.7.15
	CSize sizeText = pDC->GetTextExtent(strText);//得到文本长宽度
	CRect rectText;

	if ("B" == pCursorAB->m_strAOrB) //为了使B线显示的坐标向下移动一些距离以免两线重合。lzy2014.7.14
	{
		rectText.top = nMarginTop + 20; 
	}//eif
	else
		rectText.top = nMarginTop;

	//rectText.top = nMarginTop;
	rectText.bottom = rectText.top + sizeText.cy;
	if (nX + sizeText.cx + 1 <= rect.right)//文本正常显示，在右边,1：文本和竖线间隔
	{
		rectText.left = nX + 1;
		rectText.right = nX + 1 + sizeText.cx;
	}
	else//移动到右边，若文本显示不下，则把文本显示到左边
	{
		rectText.left = nX - (sizeText.cx + 1);
		rectText.right = nX - 1;
	}//eif
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(strText, &rectText, DT_LEFT | DT_SINGLELINE);


	///*********************zll 2013.6.25  增加A B 光标之间的距离
	strTextAminusB.Format(_T("%s %s %s"), _T("A-B:"), m_cursorAminusB.m_strDistance,m_cursorAminusB.m_strDB);
	CSize sizeTextAminusB = pDC->GetTextExtent(strTextAminusB);//得到文本长宽度
	CRect rectTextAminusB;
	rectTextAminusB.top = 0;
	rectTextAminusB.bottom = rectTextAminusB.top + sizeTextAminusB.cy;
	//rectTextAminusB.left = 1;
	//rectTextAminusB.right = 1 + sizeTextAminusB.cx;
	//将此条显示移动到右边空档，使其不至于影响到左边可能的其他显示项。lzy2014.7.15
	rectTextAminusB.left = rect.right - 1 - sizeTextAminusB.cx ;
	rectTextAminusB.right = rect.right - 1;

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(strTextAminusB, &rectTextAminusB, DT_LEFT | DT_SINGLELINE);
	///*********************zll 2013.6.25

	////将AB线的坐标显示在右上方lzy2014.7.15
	//CString strTextAB;
	//strTextAB.Format(_T("%s: %s %s"), pCursorAB->m_strAOrB, pCursorAB->m_strDistance,pCursorAB->m_strDB);
	//CSize sizeTextAB = pDC->GetTextExtent(strTextAB);
	//CRect rcTextAB = rectTextAminusB;
	//if(_T("A") == pCursorAB->m_strAOrB)  //A线的话
	//{
	//	rcTextAB.top = rectTextAminusB.bottom + 2;
	//	rcTextAB.bottom = rcTextAB.top + sizeTextAB.cy;
	//}
	//else
	//{
	//	rcTextAB.top = (rectTextAminusB.bottom + 2) * 2;
	//	rcTextAB.bottom = rcTextAB.top + sizeTextAB.cy;
	//}
	//pDC->SetBkMode(TRANSPARENT);
	//pDC->DrawText(strTextAB, &rcTextAB, DT_LEFT | DT_SINGLELINE);


	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
	pOldFont = pDC->SelectObject(pOldFont);
	font.DeleteObject();
}

void COtdrTrace::CursorABOnMove(CPoint pointMoving)
{
	if (m_vAllData.size() <= m_nCurveIndex)//下标越界
	{
		return;
	}//
	if (OTDRTRACE_AB_SELE_A == m_nCurABSele)//拖动A
	{
		FLOAT fPos = m_cursorA.m_fPos;//方便记录拖动长度,方便选择AB标时相应移动B标
		m_cursorA.m_nPos = pointMoving.x;
		m_cursorA.m_fPos = POINT2FLOATX(pointMoving.x);
		int nIndex = m_vAllData[m_nCurveIndex].DistanceToIndex(m_cursorA.m_fPos);
		m_cursorA.m_fPos = nIndex * m_vAllData[m_nCurveIndex].m_fDataSpacing;
		m_cursorA.m_strDistance.Format(_T("%.3fm"), m_cursorA.m_fPos);
		//float fDB=GetYDB(m_cursorA.m_fPos);
		/*float fTemp=(299792458/1000000.0)/(2*25 * 1.46000) * pointMoving.x;
		fTemp = pointMoving.x*(m_fRangeX[OTDR_TRACE_MAX] - m_fRangeX[OTDR_TRACE_MIN])/818 + m_fRangeX[OTDR_TRACE_MIN];
		TRACE(_T("m_cursorA.m_nPos:%d %.3fm\n"),m_cursorA.m_nPos, fTemp);*/
		if (IsCurrentTypeAB())//当前选中AB标
		{
			fPos = m_cursorA.m_fPos - fPos;			
			m_cursorB.m_fPos = m_cursorB.m_fPos + fPos;
			m_cursorB.m_nPos = FLOAT2POINTX(m_cursorB.m_fPos);  //lzy2014.9.10
			m_cursorB.m_strDistance.Format(_T("%.3fm"), m_cursorB.m_fPos);
			
		}//eif

		//TRACE(_T("CursorABOnMove() posA=%d, posB=%d\n"), m_cursorA.m_nPos, m_cursorB.m_nPos);////////////////////////////
		
	}//eif
	if (OTDRTRACE_AB_SELE_B == m_nCurABSele)//拖动B
	{
		FLOAT fPos = m_cursorB.m_fPos;//方便记录拖动长度,方便选择AB标时相应移动A标
		m_cursorB.m_nPos = pointMoving.x;
		m_cursorB.m_fPos = POINT2FLOATX(pointMoving.x);
		int nIndex = m_vAllData[m_nCurveIndex].DistanceToIndex(m_cursorB.m_fPos);
		m_cursorB.m_fPos = nIndex * m_vAllData[m_nCurveIndex].m_fDataSpacing;
		m_cursorB.m_strDistance.Format(_T("%.3fm"), m_cursorB.m_fPos);
		//float fDB=GetYDB(m_cursorB.m_fPos);
		if (IsCurrentTypeAB())//当前选中AB标
		{
			fPos = m_cursorB.m_fPos - fPos;
			m_cursorA.m_fPos = m_cursorA.m_fPos + fPos;
			m_cursorA.m_nPos = FLOAT2POINTX(m_cursorA.m_fPos);  //lzy2014.9.10
			m_cursorA.m_strDistance.Format(_T("%.3fm"), m_cursorA.m_fPos);
		}//eif
	}//eif
}

void COtdrTrace::OnShowAB()
{	
	if(!m_bShowLineAB)
	{
		m_bShowLineAB = TRUE;
		OnInitAB();
	//	m_dlgParent->ChangeABStatus(TRUE);//
	}
	else
	{
		m_bShowLineAB = FALSE;
	//	m_dlgParent->ChangeABStatus(FALSE);//
	}
}
//按下鼠标时，判断是否选中AB标,未选中返回OTDRTRACE_AB_SELE_NONE，选中A返回OTDRTRACE_AB_SELE_A，选中B返回OTDRTRACE_AB_SELE_B
INT COtdrTrace::GetABSelectState(CPoint pointClick)
{
	INT nReturn = OTDRTRACE_AB_SELE_NONE;
	INT nMarginA = -1;
	INT nMarginB = -1;
	INT nTemp;
	if (IsInCurRangeX(m_cursorA.m_fPos))//A在屏幕显示范围内
	{
		nTemp = FLOAT2POINTX(m_cursorA.m_fPos);//光标屏幕位置，象素
		if (abs(nTemp - pointClick.x) < OTDR_TRACE_CURSOR_AB_VALID_SELE)
		{
			nMarginA = abs(nTemp - pointClick.x);
		}//eif
	}//eif

	if (IsInCurRangeX(m_cursorB.m_fPos))//B在屏幕显示范围内
	{
		nTemp = FLOAT2POINTX(m_cursorB.m_fPos);//光标屏幕位置，象素
		if (abs(nTemp - pointClick.x) < OTDR_TRACE_CURSOR_AB_VALID_SELE)
		{
			nMarginB = abs(nTemp - pointClick.x);
		}//eif
	}//eif

	if ((-1 != nMarginA) && (-1 != nMarginB))//AB都在移动有效区域，选最近的
	{
		if (nMarginA <= nMarginB)
		{
			nReturn = OTDRTRACE_AB_SELE_A;
		}
		else{
			nReturn = OTDRTRACE_AB_SELE_B;
		}//eif
	}
	else if (-1 != nMarginA)//只有A在有效区域
	{
		nReturn = OTDRTRACE_AB_SELE_A;
	}
	else if (-1 != nMarginB)//只有B在有效区域
	{
		nReturn = OTDRTRACE_AB_SELE_B;
	}//eif
	return nReturn;
}

//显示抓取AB标的小图形
void COtdrTrace::ShowCatch(CDC *pDC, CPoint pt)
{
	int n1 = 3;  
	int n2 = 3;
	int n3 = OTDR_TRACE_CURSOR_AB_VALID_SELE;  //范围
	int nOffset = 3;
	int nA = abs(m_cursorA.m_nPos - pt.x);
	int nB = abs(m_cursorB.m_nPos - pt.x);
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	CPen *oldPen = pDC->SelectObject(&pen);

	switch(m_nCurABSele)
	{
	case OTDRTRACE_AB_SELE_A:
		pDC->MoveTo(m_cursorA.m_nPos - n1 - nOffset, pt.y - n2);
		pDC->LineTo(m_cursorA.m_nPos - n1 - n2 - nOffset, pt.y);
		pDC->LineTo(m_cursorA.m_nPos - n1 - nOffset, pt.y + n2);
		pDC->MoveTo(m_cursorA.m_nPos + n1, pt.y - n2);
		pDC->LineTo(m_cursorA.m_nPos + n1 + n2, pt.y);
		pDC->LineTo(m_cursorA.m_nPos + n1, pt.y + n2);
		break;

	case OTDRTRACE_AB_SELE_B:
		pDC->MoveTo(m_cursorB.m_nPos - n1 - nOffset, pt.y - n2);
		pDC->LineTo(m_cursorB.m_nPos - n1 - n2 - nOffset, pt.y);
		pDC->LineTo(m_cursorB.m_nPos - n1 - nOffset, pt.y + n2);
		pDC->MoveTo(m_cursorB.m_nPos + n1, pt.y - n2);
		pDC->LineTo(m_cursorB.m_nPos + n1 + n2, pt.y);
		pDC->LineTo(m_cursorB.m_nPos + n1, pt.y + n2);
		break;

	case OTDRTRACE_AB_SELE_AB:
	case OTDRTRACE_AB_SELE_NONE:		
		if(nA <= n3 || nB <= n3)
		{
			if(nA <= nB)
			{
				pDC->MoveTo(m_cursorA.m_nPos - n1 - nOffset, pt.y - n2);
				pDC->LineTo(m_cursorA.m_nPos - n1 - n2 - nOffset, pt.y);
				pDC->LineTo(m_cursorA.m_nPos - n1 - nOffset, pt.y + n2);
				pDC->MoveTo(m_cursorA.m_nPos + n1, pt.y - n2);
				pDC->LineTo(m_cursorA.m_nPos + n1 + n2, pt.y);
				pDC->LineTo(m_cursorA.m_nPos + n1, pt.y + n2);
			}
			else
			{
				pDC->MoveTo(m_cursorB.m_nPos - n1 - nOffset, pt.y - n2);
				pDC->LineTo(m_cursorB.m_nPos - n1 - n2 - nOffset, pt.y);
				pDC->LineTo(m_cursorB.m_nPos - n1 - nOffset, pt.y + n2);
				pDC->MoveTo(m_cursorB.m_nPos + n1, pt.y - n2);
				pDC->LineTo(m_cursorB.m_nPos + n1 + n2, pt.y);
				pDC->LineTo(m_cursorB.m_nPos + n1, pt.y + n2);
			}
		}
		break;
	}
	
	pen.DeleteObject();
	pDC->SelectObject(oldPen);
}

void COtdrTrace::SetABSelStatus(INT nType)
{
	m_nCurABSele = nType;
}

//得到当前光标对应当前曲线下标
INT COtdrTrace::GetCurrentCursorIndex()
{
	if (0  == m_nABSele)
	{
		return 0;
	}//
	int nIndex = 0;
	float fTemp = 0;
	if (OTDRTRACE_AB_SELE_A == m_nABSele)//拖动A
	{
		fTemp = m_cursorA.m_fPos + m_vAllData[m_nCurveIndex].m_fOffsetX;
		nIndex = m_vAllData[m_nCurveIndex].DistanceToIndex(fTemp);
	}//eif
	else if(OTDRTRACE_AB_SELE_B == m_nABSele)//拖动B
	{
		fTemp = m_cursorB.m_fPos + m_vAllData[m_nCurveIndex].m_fOffsetX;
		nIndex = m_vAllData[m_nCurveIndex].DistanceToIndex(fTemp);
	}//eif
	return nIndex;
}
