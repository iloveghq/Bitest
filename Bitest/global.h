#pragma once

#define VERSION_SOFTWARE _T("2.00")//软件版本

#define CONFIG_FILE       _T("\\config\\config.ini")
#define CHS_FILE         _T("\\config\\Multi_ch.ini")
#define ENG_FILE         _T("\\config\\Multi_eng.ini")

#define SET_LOSS_MAX 60    //y轴显示最大的DB
#define SCALE_BIG   8    //坐标轴刻度长线段的长度
#define SCALE_SMALL  4   //坐标轴刻度短线段的长度
#define SCALE_MAX   6    //缩放系数范围
#define SCALE_MIN   1   //缩放系数范围
#define SCALE_STEP   5   //缩放系数范围
#define  OTDR_EVENT_NUM_MAX 100//事件最大数

const unsigned C = 299792458 ; //真空中的光速m
const unsigned C_zll = 299792458 ; //真空中的光速m

#define  EVENT_POINT_HEIGHT 30//在曲线绘制事件点时会绘一条竖直线，该值为曲线高度
#define  EVENT_POINT_NUM_HEIGHT 20//在曲线绘制事件点时会绘一条竖直线，该值为竖直线下方的1,2..竖直方向位置
#define POINT2FLOATX(x) m_fRangeX[0] + (FLOAT)( (x - m_rtCurve.left)   * m_fResolutionX)
#define POINT2FLOATY(y) m_fRangeY[0] + (FLOAT)( (m_rtCurve.bottom - y) * m_fResolutionY)
#define FLOAT2POINTX(x) m_rtCurve.left + (x - m_fRangeX[0]) * m_rtCurve.Width() / (m_fRangeX[1] - m_fRangeX[0])//根据距离得到X坐标
#define FLOAT2RATIO(x) (x-m_fRangeX[0]) / (m_fRangeX[1] - m_fRangeX[0])//xjf 2014.03.03  获取比例系数

#define COLOR_BK RGB(0, 0, 0)  //曲线绘制区域背景颜色
#define COLOR_A2B RGB(255, 255, 0)  //A2B线的颜色
#define COLOR_B2A RGB(0, 255, 0)  //B2A线的颜色
#define COLOR_NOSELECT_EVENT RGB(0,0,255)  //未选中事件的颜色
#define COLOR_SELECT_EVENT RGB(255,0,0)  //选中事件的颜色
#define COLOR_EVENT_RECT RGB(128,64,64)  //事件数字边框的颜色
#define COLOR_EVENT_NUM RGB(48,48,48)  //事件数字文本的颜色
#define COLOR_SCALE_TEXT RGB(180,180,180)  //刻度文字的颜色
#define COLOR_SCALE_BK RGB(0,0,0)  //刻度背景的颜色
#define COLOR_GRID_BK RGB(180,180,180)   //栅格的颜色
#define OTDR_TRACE_CURSOR_AB_MARGIN_TOP 30//光标AB竖直方向离顶部多远
#define OTDR_TRACE_CURSOR_AB_VALID_SELE 6//多少象素之内认为选中了A/B标
#define OTDR_TRACE_CURSOR_AB_COLOR_SELE RGB(255,0,0)//A/B标选中颜色

#define DIVISOR_TOKM  1000;//

#define OTDR_TRACE_SELNEAR  3
//ghq 2016.4.15
#define OTDR_TRACE_DEFAULT_ZOOM_PERCENT 0.02//2%默认缩放比例 
#define OTDR_TRACE_Y_MIN 0
#define COUNT_GRIDLINE_HORIZONE 10//绘图用。水平方向几列//wcq2011.12.19
#define COUNT_GRIDLINE_VERTICAL 8//绘图用。竖直方向几列//wcq2011.12.19

#define LINE_BOTTOM_OFFSET_X   8   //事件数字方块偏移
#define LINE_BOTTOM_WIDTH      16   //事件数字方块宽
#define LINE_BOTTOM_HEIGHT     18   //事件数字方块高
#define LINE_TOP               -10  //事件线条上下偏移
#define LINE_BOTTOM            20

#define RETURN_OK               0
#define RETURN_ERROR            -1


#define RECT_CURVE_LEFT        110   //曲线视图区域坐标31
#define RECT_CURVE_TOP         68   //
#define RECT_CURVE_WIDTH       769  //848
#define RECT_CURVE_HEIGHT      245  //200

#define RECT_EVENT_LEFT        110   //事件视图区域坐标
#define RECT_EVENT_TOP         335   //
#define RECT_EVENT_WIDTH       769  //
#define RECT_EVENT_HEIGHT      200

#define BUTTON_FILE_LEFT        30   //打开文件按钮坐标
#define BUTTON_PRINT_LEFT       110   //
#define BUTTON_LANG_LEFT        190   //
#define BUTTON_SETUP_LEFT       270   //
#define BUTTON_ABOUT_LEFT       350   //
#define BUTTON_EXIT_LEFT        430   //

#define TEXT_FILE1_LEFT         530   //
#define TEXT_FILE1_TOP          20   //
#define TEXT_FILE2_LEFT         530   //
#define TEXT_FILE2_TOP          40   //

#define BUTTON_ZOOMIN_V_LEFT        30   //
#define BUTTON_ZOOMIN_V_TOP        68   //
#define BUTTON_ZOOMOUT_V_LEFT        30   //
#define BUTTON_ZOOMOUT_V_TOP        108   //
#define BUTTON_ZOOMIN_BOTH_LEFT        70   //
#define BUTTON_ZOOMIN_BOTH_TOP        68   //
#define BUTTON_ZOOMOUT_BOTH_LEFT        70   //
#define BUTTON_ZOOMOUT_BOTH_TOP        108   //
#define BUTTON_ZOOMIN_H_LEFT          30   //
#define BUTTON_ZOOMIN_H_TOP          148   //
#define BUTTON_ZOOMOUT_H_LEFT        70   //
#define BUTTON_ZOOMOUT_H_TOP        148   //

#define BUTTON_RESET_LEFT         30   //还原按钮坐标
#define BUTTON_RESET_TOP          188   //还原按钮坐标

#define BUTTON_ADDEVETNT_LEFT         30   //添加事件按钮坐标
#define BUTTON_ADDEVETNT_TOP          228   //
#define BUTTON_DELEVETNT_LEFT         70   //删除事件按钮坐标
#define BUTTON_DELEVETNT_TOP          228   //

#define CHECK_CURSORAB_LEFT        30   //
#define CHECK_CURSORAB_TOP        273   //
#define RADIO_AANDB_LEFT              30   //
#define RADIO_AANDB_TOP              (CHECK_CURSORAB_TOP+25)   //
#define RADIO_AORB_LEFT            70   //
#define RADIO_AORB_TOP               (CHECK_CURSORAB_TOP+25)   //

#define RADIO_AB_LEFT        30   //
#define RADIO_AB_TOP         340   //
#define RADIO_BA_LEFT        30   //
#define RADIO_BA_TOP         370   //
#define RADIO_BOTH_LEFT      30   //
#define RADIO_BOTH_TOP       400   //

#define TOP_BUTTON_TOP          15  //
#define TOP_BUTTON_WIDTH        75  //
#define TOP_BUTTON_HEIGHT       40
#define LOWER_BUTTON_WIDTH      36  //
#define LOWER_BUTTON_HEIGHT     36
#define RADIO_WIDTH             40  //
#define RADIO_HEIGHT            15
#define TEXT_WIDTH              500  //
#define TEXT_HEIGHT             20

#define  M_FSTARTDB0 -65.63//-65.53
#define  M_FMAXDB0   -5.63//-9.23
#define  M_FSTARTDB1 -58.3//当所曲线最小值都 > ZOOM_VERTICAL_STANDARD 30*1000时使用
#define  M_FMAXDB1   -8.3//由-1改为-8.3
#define  CLEAR_DATAPOINTS_NUM 1//重新测试后清空数据时设相应波长的pDataPointsNum值。设为0容易导致除0,表示清空.wcq2011.12.23

#define  EVENT_LIST_COL   8
#define  EVENT_LIST_PRINT_MAX   18

#define  MY_PRINT_TEST   1
#define  OTDR_TRACE_Y_MAX    60
#define  OTDR_TRACE_Y_MAX_2WAY    60
#define  OTDR_TRACE_Y_FACTOR 1000//y轴滚动范围为整数型，当放大到一定倍数后，nPage为0，所以需要放大一定倍数，这里设为1000//zll 2013.4.24

#define  MAX_FILE_PATH       350   //
#define  MARGIN_LEFT_COUNT 18  //左右留空的宽度占总宽度的几分之几
#define  PRINTMARGIN 2
#define  EVENT_2WAY_OFFSET   20

#define	WM_MY_PRINT			    (WM_USER+1003)
#define	WM_BEGIN_PRINTING		(WM_USER+1004)
#define	WM_END_PRINTING			(WM_USER+1005)

const float FONTSIZE_RATIO[5] = {1.0, 0.8, 0.6, 0.45, 0.4};  //报表输出时用的字体的缩放比例，[0]表示单报表，[1]表示2图/页，[2]表示4图/页，[3]表示6图/页，[4]表示8图每页


//定义数据显示类型的枚举
typedef enum ShowType
{
	SHOW_A2B = 0,
	SHOW_B2A = 1,
	SHOW_ALL = 2
};

typedef enum ButtonType
{
	ZOOM_RESET = -1,
	ZOOM_IN_BOTH = 0,
	ZOOM_OUT_BOTH = 1,
	ZOOM_IN_H = 2,
	ZOOM_OUT_H = 3,
	ZOOM_IN_V = 4,
	ZOOM_OUT_V = 5,
};


enum OTDR_TRACE_AB_SELECT//光标AB选中状态
{
	OTDRTRACE_AB_SELE_DISABLE=-1,
	OTDRTRACE_AB_SELE_NONE=0,
	OTDRTRACE_AB_SELE_A,
	OTDRTRACE_AB_SELE_B,
	OTDRTRACE_AB_SELE_AB,
	OTDRTRACE_AB_SELE_EVENT,
};
enum OTDR_TRACE_AB_TYPE//光标AB模式  
{
	OTDRTRACE_AB_TYPE_SINGLE = 0,//选一个(A/B)
	OTDRTRACE_AB_SELE_BOTH//AB模式
};

enum OTDR_EVENT_TYPE//事件类型 
{
	OTDR_EVENT_TYPE_NOREFLECT = 0,//选一个(A/B)
	OTDR_EVENT_TYPE_REFLECT//AB模式
};

//图形绘制区域
enum CURSOR_SELECT : int
{
	CURSOR_NONE = 0,
	CURSOR_A = 1,		//选中A
	CURSOR_B = 2,		//选中B
	CURSOR_AB = 4,		//选中AB
	CURSOR_EVENT = 8	//选中事件
};

enum CURVE_CURRECT_MODE
{
	CURVE_NONE=0,
	CURVE_GET_DATA,
	CURVE_SELECT,
	CURVE_ZOOM_OUT ,
	CURVE_ZOOM_OUT_V,
	CURVE_ZOOM_OUT_H,
	CURVE_ZOOM_IN,
	CURVE_ZOOM_IN_V,
	CURVE_ZOOM_IN_H,
	CURVE_MOVE,
	CURVE_MOVE_UP,
	CURVE_MOVE_DOWN,
	CURVE_MOVE_LEFT,
	CURVE_MOVE_RIGHT,
	CURVE_RESET
};

//support language
enum LANG : int
{
	CHS = 0,   //Chinese
	ENG = 1,   //English
};



