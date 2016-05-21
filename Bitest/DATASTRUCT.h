#ifndef _DATASTRUCT_H_
#define _DATASTRUCT_H_


#include <vector>
//#include "EndGabor.h"
#include "GaborResult.h"
#include "global.h"

struct EventInfo //事件点信息结构体
{
	unsigned int index;//位于数据数组的第几个点
	unsigned int nEventType;//事件类型，0反射事件，1非反射事件
	float fLocation;//事件位置,(C/(2nf))*index,根据第几点计算得到距离//2011.11.24
	float fEventloss;//损耗//2011.11.24
	float fEventHigh;////事件点反射dB//2011.11.24
	float fLineK;////区段斜率//2011.11.24
	float fTotalLoss;////总损耗、累计损耗//2011.11.24
	float fReflect;//反射值
	float fTrailEnd;////尾端位置
	char  cEventType;// event code 第二个参数  S:开始事件 E:光纤末端即结束事件；A：用户自加； F:软件自行查找；O:超出测量范围的光纤末端 D: wcq2012.06.11
};

struct EventInfoList//事件点信息列表
{
	~EventInfoList()
	{
		vEventInfo.clear();
	}
	std::vector<EventInfo> vEventInfo; //从网络接收的事件数组
};
//struct DataScales//wcq20111101
//{
//	short scaleFactor;//编码2字节
//	std::vector<unsigned short> vData;//
//};
//std::vector<DataScales> vAllData;//所有数据
struct TestResultInfo 
{
	TestResultInfo()//wcq 2011.11.1
	{
		IsMaxLength = FALSE;
		IsLowerStandard = FALSE;
		IsLoadFromFile = TRUE;
		DataType = 8888; //数据类型， 有数据8888，无数据4444
		TestMode = 2222; //测试模式，平均测试2222，
		DataSerialNum = 0; //数据包序号
		pDataPoints = NULL; //采样点值 扩大了1000倍的值
		pDataCurve = NULL; //保存取点后计算过的数据
		ScaleFactor = 1;//缩放系数，每个数据点都乘该系数
		YOffset = 0;//竖直方向偏移，用于平移
		StartPoint = 0;  //曲线横坐标的起点
		MaxPoint = 0;    //曲线横坐标点数的最大值
		FullPointCount = 0;//曲线缩放时为多少个点时曲线占满整个绘图区域,用以确定曲线宽度wcq2011.09.16
		FullPointCountMin = 0;//曲线未缩放,为多少个点时曲线占满整个绘图区域,用以确定曲线宽度,FullPointCount为该数的倍数。wcq2011.09.21
		LastZoomPointCount = 0;//曲线缩放时最后一次缩放共多少点，和前面的缩放的点的个数不一定一样。wcq2011.09.21
		DataSpacing = 0;//for each pulse width used:one-way time(units of 100 psec) for 10000 data points//wcq2011.10.19理解为两点间距离
		Refraction = 0.0f; //折射率
		ChainLoss = 0.0f; //链损耗，扩大了1000倍
		ChainAttenu = 0.0f; //链衰减，扩大了1000倍
		MaxTestDistance = 0.0f; //测量曲线最大距离
		DataPointsNum = 0; //采样点个数
		Frequency = 0; //频率
		Range = 0; //测量范围
		PulseWidth = 0; //脉冲宽度
		WaveLength = 0; //中心波长
		MeasureTiem = 0; //测量时间
		ChainLength = 0; //链长
		ptDrawData = NULL; //画图坐标点,每个类对应一个绘图数据指针,原来名称为COTDRPage类中m_ptRecord wcq2011.09.21

		//
		Atten = 0.0f;//衰减
		Average = 0.0f;//平均
		SampleCount = 0;//采样数
		FusionLoss = 0.0f;//熔接损耗
		ReturnLoss = 0.0f;//回波损耗

		//附加属性
		Relay = 0;//继电器//wcq2011.12.06
		Joint1 = 0;//拼接点1//wcq2011.12.05
		Joint2 = 0;//拼接点2//wcq2011.12.05
		Length = 0; //曲线长度(光纤长度)
		ThumbDrawXCount = 0;//缩略图绘制时x坐标值最大值，最长曲线为缩略图区域宽度
		dataCurveCount = 0;//pDataCurve点个数,不一定为COUNT_POINT_MAX_MIN，wcq2011.11.23
		curveColor = RGB(0,0,255); //曲线颜色
		pThumbData = NULL;//缩略图数据
//显示时用平滑过的数据，保存时用实际数据 wcq2013.04.26
		pDataPointsSmooth = NULL;

		BackscatterCoefficient = 0;//散射系数 wcq2012.10.30
	    LossThreshold = 0;//衰减门限 wcq2012.10.30
	    ReflectanceThreshold = 0;//反射门限 wcq2012.10.30
	    EndOfFiberThreshold = 0;//光纤末端门限 wcq2012.10.30
	}
	~TestResultInfo()//wcq 2011.11.1
	{
		if (NULL != pDataPoints)
		{
			delete[] pDataPoints;
			pDataPoints = NULL;
		}
		if (NULL != pDataCurve)
		{
			delete[] pDataCurve;
			pDataCurve = NULL;
		}
		if (NULL != ptDrawData)
		{
			//delete[] ptDrawData;
			ptDrawData = NULL;
		}
		if (NULL != pThumbData)
		{
			delete[] pThumbData;
			pThumbData = NULL;
		}
		

	//显示时用平滑过的数据，保存时用实际数据 wcq2013.04.26
		//m_TestResult.pDataPointsSmooth=NULL;
		if (NULL != pDataPointsSmooth)
		{
			delete[] pDataPointsSmooth;
			pDataPointsSmooth=NULL;
		}

		EventList.vEventInfo.clear();
	}
	BOOL IsMaxLength; //是否是最长曲线
	BOOL IsLowerStandard;//wcq2011.09.26根据与ZOOM_VERTICAL_STANDARD比较而为true或false.用于竖直方向缩放时确定最大量程
	BOOL IsLoadFromFile;//是否从文件加载上来的曲线//wcq2011.10.19
	WORD DataType; //数据类型， 有数据8888，无数据4444
	WORD TestMode; //测试模式，平均测试2222，
	WORD DataSerialNum; //数据包序号
	WORD* pDataPoints; //采样点值 扩大了1000倍的值
	CPoint* pDataCurve; //保存取点后计算过的数据
	short ScaleFactor;//缩放系数，每个数据点都乘该系数
	int YOffset;//竖直方向偏移，用于平移
	int StartPoint;  //曲线横坐标的起点
	int MaxPoint;    //曲线横坐标点数的最大值
	int FullPointCount;//曲线缩放时为多少个点时曲线占满整个绘图区域,用以确定曲线宽度wcq2011.09.16
	int FullPointCountMin;//曲线未缩放,为多少个点时曲线占满整个绘图区域,用以确定曲线宽度,FullPointCount为该数的倍数。wcq2011.09.21
	int LastZoomPointCount;//曲线缩放时最后一次缩放共多少点，和前面的缩放的点的个数不一定一样。wcq2011.09.21
	float DataSpacing;//for each pulse width used:one-way time(units of 100 psec) for 10000 data points//wcq2011.10.19理解为两点间距离
	float Refraction; //折射率1310nm
	float Refraction1550; //折射率1550nm//zll 2012.9.18
	float ChainLoss; //链损耗，扩大了1000倍
	float ChainAttenu; //链衰减，扩大了1000倍
	float MaxTestDistance; //测量曲线最大距离
	unsigned long int DataPointsNum; //采样点个数
	float Frequency; //频率
	long int Range; //测量范围
	long int PulseWidth; //脉冲宽度
	long int WaveLength; //中心波长
	long int MeasureTiem; //测量时间
	long int ChainLength; //链长
	CPoint* ptDrawData; //画图坐标点,每个类对应一个绘图数据指针,原来名称为COTDRPage类中m_ptRecord wcq2011.09.21
	EventInfoList EventList;//事件列表
//显示时用平滑过的数据，保存时用实际数据
	WORD* pDataPointsSmooth; //采样点值 扩大了1000倍的值,原始数据经过平滑处理后的数据


	//
	float Atten;//衰减
	float Average;//平均
	int   SampleCount;//采样数
	float FusionLoss;//熔接损耗
	float ReturnLoss;//回波损耗

	//附加属性
	WORD Relay;//继电器//wcq2011.12.06
	WORD Joint1;//拼接点1//wcq2011.12.05
	WORD Joint2;//拼接点2//wcq2011.12.05
	float Length; //曲线长度(光纤长度)
	unsigned int ThumbDrawXCount;//缩略图绘制时x坐标值最大值，最长曲线为缩略图区域宽度
	int dataCurveCount;//pDataCurve点个数,不一定为COUNT_POINT_MAX_MIN，wcq2011.11.23
	COLORREF curveColor; //曲线颜色
	CPoint*  pThumbData;//缩略图数据

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//后面添加 wcq2012.10.30
	float BackscatterCoefficient;//散射系数 wcq2012.10.30
	float LossThreshold;//衰减门限 wcq2012.10.30
	float ReflectanceThreshold;//反射门限 wcq2012.10.30
	float EndOfFiberThreshold;//光纤末端门限 wcq2012.10.30
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	Gabor_Result m_GaborResult;//事件列表
	CString strTestTime;//ghq 2016.5.3 测试时间
	CString strFileName;//ghq 2016.5.3 文件名
};

//数据上传数据部分
typedef struct _OTDRAllUploadData
{
	_OTDRAllUploadData()
	{
		DataPointsNum = 0;
		PData = NULL;
	}
	~_OTDRAllUploadData()
	{
		if(PData)
		{
			delete []PData;
		    PData = NULL;
		}
	}
	INT GetLength()
	{
		return 16 + DataPointsNum*2+1 + VEventInfo.size()*6*4+1;//16:测试条件部份;DataPointsNum+1:测试数据;VEventInfo.size()*6+1:事件列表
	}//eif
	//测试条件
	UINT SampleFrequence;//采样频率
	UINT TestRange;//测量范围
	UINT PulseWidth;//脉宽
	UINT WaveLength;//工作光波长
	UINT TestTime;//测量时间
	FLOAT Refract;//折射率
	FLOAT LinkLength;//链长
	FLOAT LinkLoss;//链损耗
	FLOAT LinkAttenuation;//链衰减
	FLOAT Loss;//衰减
	FLOAT FiberEnd;//末端门限
	UINT TestMode;//OTDR测量模式 OtdrMode
	UINT AutoTest;//测试方式 0:自动,1:手动
	FLOAT Reflect;//反射门限
	FLOAT Slope;//斜门率限
	FLOAT ScatterCoefficient;//散射系数门限
	//16
	//ghq 2016.4.14
	FLOAT DataSpacing;//理解为两点间距离,曲线两点间距离
	FLOAT Length;//长度

	//测试结果数据
	UINT DataPointsNum;//数据点个数
	WORD* PData;//数据

	//事件点信息
	UINT EventNum;//事件点个数
	struct ENVENT_INFO
	{
		UINT Index;//事件点的位置在所上传数据里的序号
		UINT EventType;//事件类型,0/1/2/3 对应 起始/反射/非反射/结束
		FLOAT Reflect;//反射
		FLOAT Loss;//插入损耗
		FLOAT Slope;//平均衰减系数
		FLOAT TotalLoss;//事件点距被测链路起始点的链路总损耗
	};
	std::vector<ENVENT_INFO> VEventInfo;
}OTDR_ALL_UPLOAD_DATA,*POTDR_ALL_UPLOAD_DATA;

//定义结构体保存绘制坐标需要的数据
typedef struct CoordinateCoef
{
	CoordinateCoef()
	{
		iHorizontal = 6;
		iVertical = 10;
		fXmin = 0;
		fXmax = 80;
		fYmin = 0;
		fYmax = OTDR_TRACE_Y_MAX;
	}
	
	CRect rcWnd;  //绘制窗口的尺寸
	UINT iHorizontal;  //水平线的条数
	UINT iVertical;    //垂直线的条数
	float fXmin;       //X轴最小值
	float fXmax;       //X轴最大值
	float fYmin;       //Y轴最小值
	float fYmax;       //Y轴最大值
};

typedef struct PrintQuest 
{	
	CString strTitle;//任务标题
	CString strID;//任务编号
	CString strContractor; //承包商
	CString strCustomer;  //用户
	CString strSorFileName; //文件名
	CString strTestDate;   //测试日期
	CString strPlayer;   //操作员
	CString strComment;

	//value
	CString QuestID;    //任务编号
	CString Contractor; //承包商
	CString Customer;  //用户
	CString SorFileName; //文件名
	CString TestDate;   //测试日期
	CString Player;   //操作员
	CString Comment;
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
	PrintConfig stPrintConfig; //配置信息
	PrintTrace stPrintTrace;
	WaveConfig stWaveConfig;
	PrintEvent stPrintEvent;
	TestResultInfo stTestResultInfo[3];
	INT curSelection;//文件列表当前选中哪个
	INT totalCurveNumber;
	CString WaveList;
};

//增加A\B曲线绘制数据
struct CursorInfo
{
	BOOL bDrawAB; //是否绘制
	FLOAT fRatioA; //A光标在坐标中的比例系数	
	CString strDistanceA;
	CString strDBA;
	CString strA;
	

	FLOAT fRatioB;//B光标在坐标中的比例系数
	CString strDistanceB;
	CString strDBB;
	CString strB;
};

#endif   //_DATASTRUCT_H_
