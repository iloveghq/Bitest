#include "StdAfx.h"
#define _CLASSINDLL//
#include "SorFile.h"

////CSorFile::CSorFile(void)
////{
////	//m_lpVoid = new CSorFileBase();
////}
////
////CSorFile::~CSorFile(void)
////{
////	//if (NULL != m_lpVoid)
////	//{
////	//	delete (CSorFileBase*)m_lpVoid;//wcq2016.03.15
////	//	//delete[] m_lpVoid;//”–ƒ⁄¥Ê–π¬© cwcq2016.03.15
////	//}
////}
////INT CSorFile::readSorFile(CString filePathAndName,TestResultInfo& testResultInfo)//cwcq2012.07.23
////{
////	return ( (CSorFileBase*)m_lpVoid )->readSorFile(filePathAndName,testResultInfo);
////}
////INT CSorFile::saveSorFile(CString filePathAndName,TestResultInfo testResultInfo)//cwcq2012.07.23
////{
////	return ( (CSorFileBase*)m_lpVoid )->saveSorFile(filePathAndName,testResultInfo);
////}

INT CSorFile::readSorFile(CString filePathAndName,SORDATA& sorData)//wcq2012.07.23
{
	return 1;
}
INT CSorFile::saveSorFile(CString filePathAndName,SORDATA& sorData)//wcq2012.07.23
{
	return 1;
}
INT CSorFile::readSorFile(BYTE* pData, int nLength, SORDATA& sorData)
{
	return 1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// Access:    public 
// Returns:   void
// Comment(wcq2016-4-26): Õ∑≈
////////////////////////////////////////////////////////////////////////////////////////////////////////
void CSorFile::Release()
{

}