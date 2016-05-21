#include "StdAfx.h"
#include "SorDllUtil.h"


CSorDllUtil::CSorDllUtil(void)
{
	m_pSorFile = NULL;
	m_pFnDeleteSorfile = NULL;
}


CSorDllUtil::~CSorDllUtil(void)
{
	if (NULL != m_hModule)
	{
		UnInit();
	}//eif
}

BOOL CSorDllUtil::Init()
{

	if (NULL != m_hModule)
	{
		return TRUE;
	}//eif

	m_hModule = ::LoadLibrary(m_util.getExePath() + _T("\\SOR.dll"));
	if (!m_hModule)
	{
		return FALSE;
	}//eif
	FN_GETSORFILE pFnGetSorfile = (FN_GETSORFILE)GetProcAddress(m_hModule, "GetSorFile");
	if (NULL != pFnGetSorfile)
	{
		m_pSorFile = pFnGetSorfile();
	}//eif
	FN_DELETESORFILE pFnDeleteSorfile = (FN_DELETESORFILE)GetProcAddress(m_hModule, "DeleteSorFile");
	if (NULL != pFnDeleteSorfile)
	{
		m_pFnDeleteSorfile = pFnDeleteSorfile;
	}//eif

	return TRUE;
}

void CSorDllUtil::UnInit()
{
	if (NULL != m_pSorFile)
	{
		m_pSorFile->Release();
		DeleteSorFile( );//É¾³ým_pSorFileÀàÖ¸Õë
	}//eif

	if (NULL != m_hModule)
	{
		::FreeLibrary(m_hModule);
	}//eif
}

INT CSorDllUtil::readSorFile( CString filePathAndName,SORDATA& sorData )
{
	if (NULL != m_pSorFile)
	{
		return m_pSorFile->readSorFile(filePathAndName, sorData);
	}//eif
	return 0;
}

INT CSorDllUtil::readSorFile( BYTE *pData, int nLength, SORDATA &sorData )
{
	if (NULL != m_pSorFile)
	{
		return m_pSorFile->readSorFile(pData, nLength, sorData);
	}//eif
	return 0;
}

INT CSorDllUtil::saveSorFile( CString filePathAndName,SORDATA& sorData )
{
	if (NULL != m_pSorFile)
	{
		return m_pSorFile->saveSorFile(filePathAndName, sorData);
	}//eif
	return 0;
}

void CSorDllUtil::Release()
{
	if (NULL != m_pSorFile)
	{
		m_pSorFile->Release();
		m_pSorFile = NULL;
	}//eif
}

void CSorDllUtil::DeleteSorFile( )
{
	if (NULL != m_pFnDeleteSorfile)
	{
		m_pFnDeleteSorfile(m_pSorFile);
	}//eif
}