#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//======================gabor_dll.dll调用封装=====================================
//wcq2015.11.12
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "GaborResult.h"
#include "Util.h"
//#include "EndGabor.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef COMPILE_ALGORITHM_OLDER_DELETE_ENENT_PARAMETER//调用20130624前算法处理得到事件列表，0624版算法主要是删除事件作了修改，函数个数多加了一个 wcq2013.06.26
	typedef Gabor_Result (* FN_DELETE_EVENT)(int deleteNum,int wavelength,int pulsewidthbox,WORD input_data[],Gabor_Result sGabor_Result,float Index_N,float Sample_freaq);
#else
	typedef Gabor_Result (* FN_DELETE_EVENT)(int deleteNum,int wavelength,int pulsewidth,WORD input_data[],Gabor_Result sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe);
#endif
typedef Gabor_Result (* FN_INSERT_EVENT)(int insertNum,int wavelength,int pulsewidthbox,WORD input_data[],int paramLen,Gabor_Result sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe);
typedef Gabor_Result (* FN_GABOR_ARITHMETIC)(int wavelength,int pulseWidth,WORD input_data[],INT paramLen,double thresholdUpSet,double thresholdDownSet,float dB_Loss,float dB_reflex,float dB_End,float Return_loss_Tybe,float Sample_freaq,float Index_N,int link_point_number);
typedef LPCTSTR (* FN_VERSION_NUMBER)();//
typedef Gabor_Result (* FN_THE_MONITORING_MODE)(int wavelength,int pulseWidth,WORD input_data[],INT paramLen,float dB_Loss,float dB_reflex,float Sample_freaq,float Index_N,int start_point,int end_point);
typedef int (* FN_EVENT_ARRAY_END_FUNCTION)(int number);//Event_Array_End_function
typedef void (* FN_DEADZOOM_FUNCTION)(int wavelength,int pulsewidth,WORD input_data[],int paramLen,Gabor_Result& sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe);//
typedef void (* FN_ACOUNT_TOTAL_LOSS)(int wavelength,int pulsewidth,WORD input_data[],Gabor_Result* sGabor_Result,float Index_N,float Sample_freaq);//2016.04.25
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class CGaborDllUtil
{
public:
	CGaborDllUtil(void);
	~CGaborDllUtil(void);
public:
	BOOL Init();
	void UnInit();

public:
#ifdef COMPILE_ALGORITHM_OLDER_DELETE_ENENT_PARAMETER//调用20130624前算法处理得到事件列表，0624版算法主要是删除事件作了修改，函数个数多加了一个 wcq2013.06.26
	Gabor_Result deleteEvent(int deleteNum,int wavelength,int pulsewidthbox,WORD input_data[],Gabor_Result sGabor_Result,float Index_N,float Sample_freaq);
#else
	Gabor_Result deleteEvent(int deleteNum,int wavelength,int pulsewidth,WORD input_data[],Gabor_Result sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe);
#endif
	Gabor_Result insertEvent(int insertNum,int wavelength,int pulsewidthbox,WORD input_data[],int paramLen,Gabor_Result sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe);
	Gabor_Result gabor_arithmetic(int wavelength,int pulseWidth,WORD input_data[],INT paramLen,double thresholdUpSet,double thresholdDownSet,float dB_Loss,float dB_reflex,float dB_End,float Return_loss_Tybe,float Sample_freaq,float Index_N,int link_point_number);
	LPCTSTR Version_number();//
	Gabor_Result The_monitoring_mode(int wavelength,int pulseWidth,WORD input_data[],INT paramLen,float dB_Loss,float dB_reflex,float Sample_freaq,float Index_N,int start_point,int end_point);
	int Event_Array_End_function(int number);
	void Deadzoon_function(int wavelength,int pulsewidth,WORD input_data[],int paramLen,Gabor_Result& sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe);
	void Acount_Total_Loss(int wavelength,int pulsewidth,WORD input_data[],Gabor_Result* sGabor_Result,float Index_N,float Sample_freaq);//2016.04.25
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
private:
	HMODULE m_hModule;
	FN_DELETE_EVENT m_pFnDeleteEvent;
	FN_INSERT_EVENT m_pFnInsertEvent;
	FN_GABOR_ARITHMETIC m_pFnGaborArithMetic;
	FN_VERSION_NUMBER m_pFnVersionNumber;
	FN_THE_MONITORING_MODE m_pFnTheMonitoringMode;
	FN_EVENT_ARRAY_END_FUNCTION m_pFnEventArrayEnd;
	FN_DEADZOOM_FUNCTION m_pFnDeadZoom;
	FN_ACOUNT_TOTAL_LOSS m_pFnAcountTotalLoss;

	CUtil m_util;
	Gabor_Result m_gaborResult;
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
};
