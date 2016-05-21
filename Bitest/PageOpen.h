#pragma once
#include "GrandwayReg.h"
#include "VARIABLE.h"
#include "Util.h"
#include "SORDATA.h"
#include "DATASTRUCT.h"
#include "SorAssist.h"
#include "SorDllUtil.h"

// CPageOpen 对话框

class CPageOpen : public CDialog
{
	DECLARE_DYNAMIC(CPageOpen)

public:
	CPageOpen(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageOpen();

// 对话框数据
	enum { IDD = IDD_PAGE_OPEN_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenA();
	afx_msg void OnOpenB();

public:
	CUtil m_util;
    SORDATA m_sorDATA[3];   //从sor文件中读取的数据
	//TestResultInfo m_testResInfo[3];   //保存处理后的数据

	afx_msg void OnOpenOk();
	afx_msg void OnOpenCancel();

public:
	CSorAssist m_sorAssist;

	//ghq 2016.4.20
    CBitestDlg *m_dlgParent;
public:
	void SetLocaleString();

};
