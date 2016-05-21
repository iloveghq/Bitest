#pragma once

#include "Util.h"
#include "SorAssist.h"
#include "afxwin.h"

// CPageSetup 对话框

class CPageSetup : public CDialog
{
	DECLARE_DYNAMIC(CPageSetup)

public:
	CPageSetup(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPageSetup();

// 对话框数据
	enum { IDD = IDD_PAGE_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	CUtil m_util;
	PrintQuest m_printQuest;
public:
	CBitestDlg *m_dlgParent;
private:
	void OnWriteQuestConfigFile();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSetupClear();
public:
	void SetLocaleString();
	void OnReadQuestConfigFile();
	void OnGetQuestInfo();

	virtual BOOL OnInitDialog();
};
