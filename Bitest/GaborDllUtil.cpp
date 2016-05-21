#include "StdAfx.h"
#include "GaborDllUtil.h"

CGaborDllUtil::CGaborDllUtil(void)
{
	m_hModule = NULL;
	m_pFnDeleteEvent = NULL;
	m_pFnInsertEvent = NULL;
	m_pFnGaborArithMetic = NULL;
	m_pFnVersionNumber = NULL;
	m_pFnEventArrayEnd = NULL;
	m_pFnDeadZoom = NULL;
	m_pFnTheMonitoringMode = NULL;
	m_pFnAcountTotalLoss = NULL;
	m_gaborResult.Valid_Num = 0;
}

CGaborDllUtil::~CGaborDllUtil(void)
{
	if (NULL != m_hModule)
	{
		UnInit();
	}//eif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access:    public 
// Returns:   void
// Comment(2015-11-12):
////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CGaborDllUtil::Init()
{
	if (NULL != m_hModule)
	{
		return TRUE;
	}//eif
	CString strTemp = m_util.getExePath() + _T("\\GABOR.dll");
	m_hModule = ::LoadLibrary(strTemp);
	if (!m_hModule)
	{
		return FALSE;
	}//eif
	m_pFnDeleteEvent = (FN_DELETE_EVENT)GetProcAddress(m_hModule, "deleteEvent");
	m_pFnInsertEvent = (FN_INSERT_EVENT)GetProcAddress(m_hModule, "insertEvent");
	m_pFnGaborArithMetic = (FN_GABOR_ARITHMETIC)GetProcAddress(m_hModule, "gabor_arithmetic");
	m_pFnVersionNumber = (FN_VERSION_NUMBER)GetProcAddress(m_hModule, "Version_number");
	m_pFnTheMonitoringMode = (FN_THE_MONITORING_MODE)GetProcAddress(m_hModule, "The_monitoring_mode");
	m_pFnEventArrayEnd = (FN_EVENT_ARRAY_END_FUNCTION)GetProcAddress(m_hModule, "Event_Array_End_function");
	m_pFnDeadZoom = (FN_DEADZOOM_FUNCTION)GetProcAddress(m_hModule, "Deadzoon_function");
	m_pFnAcountTotalLoss = (FN_ACOUNT_TOTAL_LOSS)GetProcAddress(m_hModule, "Acount_Total_Loss");

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access:    public 
// Returns:   void
// Comment(2015-11-12):
////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGaborDllUtil::UnInit()
{
	if (NULL != m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_pFnDeleteEvent = NULL;
		m_pFnInsertEvent = NULL;
		m_pFnGaborArithMetic = NULL;
		m_pFnVersionNumber = NULL;
		m_hModule = NULL;
	}//eif

}
#ifdef COMPILE_ALGORITHM_OLDER_DELETE_ENENT_PARAMETER//调用20130624前算法处理得到事件列表，0624版算法主要是删除事件作了修改，函数个数多加了一个 wcq2013.06.26
Gabor_Result CGaborDllUtil::deleteEvent(int deleteNum,int wavelength,int pulsewidthbox,WORD input_data[],Gabor_Result sGabor_Result,float Index_N,float Sample_freaq)
{

}
#else
Gabor_Result CGaborDllUtil::deleteEvent(int deleteNum,int wavelength,int pulsewidth,WORD input_data[],Gabor_Result sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe)
{
	if (m_pFnDeleteEvent)
	{
		return m_pFnDeleteEvent( deleteNum, wavelength, pulsewidth, input_data, sGabor_Result, Index_N, Sample_freaq, Return_loss_Tybe);
	}//eif

	return m_gaborResult;
}
#endif

Gabor_Result CGaborDllUtil::insertEvent(int insertNum,int wavelength,int pulsewidthbox,WORD input_data[],int paramLen,Gabor_Result sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe)
{
	if (m_pFnInsertEvent)
	{
		return m_pFnInsertEvent( insertNum, wavelength, pulsewidthbox, input_data, paramLen, sGabor_Result, Index_N, Sample_freaq, Return_loss_Tybe);
	}//eif

	return m_gaborResult;
}
Gabor_Result CGaborDllUtil::gabor_arithmetic(int wavelength,int pulseWidth,WORD input_data[],INT paramLen,double thresholdUpSet,double thresholdDownSet,float dB_Loss,float dB_reflex,float dB_End,float Return_loss_Tybe,float Sample_freaq,float Index_N,int link_point_number)
{
	if (m_pFnGaborArithMetic)
	{
		return m_pFnGaborArithMetic( wavelength, pulseWidth, input_data, paramLen, thresholdUpSet, thresholdDownSet, dB_Loss, dB_reflex, dB_End, Return_loss_Tybe, Sample_freaq, Index_N, link_point_number);
	}//eif

	return m_gaborResult;
}
LPCTSTR CGaborDllUtil::Version_number()
{
	if (m_pFnVersionNumber)
	{
		return m_pFnVersionNumber( );
	}//eif

	return _T("");
}

int CGaborDllUtil::Event_Array_End_function( int number )
{
	if (m_pFnEventArrayEnd)
	{
		return m_pFnEventArrayEnd(number);
	}//eif

	return 0;
}

void CGaborDllUtil::Deadzoon_function( int wavelength,int pulsewidth,WORD input_data[],int paramLen,Gabor_Result& sGabor_Result,float Index_N,float Sample_freaq,float Return_loss_Tybe )
{
	if (m_pFnDeadZoom)
	{
		m_pFnDeadZoom(  wavelength, pulsewidth, input_data, paramLen, sGabor_Result, Index_N, Sample_freaq, Return_loss_Tybe);
	}//eif
}

Gabor_Result CGaborDllUtil::The_monitoring_mode( int wavelength,int pulseWidth,WORD input_data[],INT paramLen,float dB_Loss,float dB_reflex,float Sample_freaq,float Index_N,int start_point,int end_point )
{
	if (m_pFnTheMonitoringMode)
	{
		return m_pFnTheMonitoringMode( wavelength, pulseWidth, input_data, paramLen, dB_Loss, dB_reflex, Sample_freaq, Index_N, start_point, end_point);
	}//eif
	return m_gaborResult;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access:    public 
// Returns:   void
// Parameter: int wavelength
// Parameter: int pulsewidth
// Parameter: WORD input_data[]
// Parameter: Gabor_Result * sGabor_Result
// Parameter: float Index_N
// Parameter: float Sample_freaq
// Comment(wcq2016-4-25):计算总损耗
////////////////////////////////////////////////////////////////////////////////////////////////////////
void CGaborDllUtil::Acount_Total_Loss( int wavelength,int pulsewidth,WORD input_data[],Gabor_Result* sGabor_Result,float Index_N,float Sample_freaq )
{
	if (m_pFnAcountTotalLoss)
	{
		m_pFnAcountTotalLoss(  wavelength, pulsewidth, input_data, sGabor_Result, Index_N, Sample_freaq);
	}//eif
}