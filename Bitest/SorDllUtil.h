#pragma once
#include "SorFile.h"
#include "Util.h"

typedef CSorFile* (* FN_GETSORFILE)();//
typedef void (* FN_DELETESORFILE)(CSorFile* pSorFile);//

class CSorDllUtil
{
public:
	CSorDllUtil(void);
	~CSorDllUtil(void);
public:
	BOOL Init();
	void UnInit();
public:
	virtual INT readSorFile(CString filePathAndName,SORDATA& sorData);//wcq2012.07.23
	virtual INT readSorFile(BYTE *pData, int nLength, SORDATA& sorData); //lzy2014.11.11直接读内存sor文件
	virtual INT saveSorFile(CString filePathAndName,SORDATA& sorData);//wcq2012.07.23
	virtual void Release();
	void DeleteSorFile();
public:
private:
	HMODULE m_hModule;
	CSorFile* m_pSorFile;
	FN_DELETESORFILE m_pFnDeleteSorfile;

	CUtil m_util;
};

