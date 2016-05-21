// PageOpen.cpp : 实现文件
//

#include "stdafx.h"
#include "Bitest.h"
#include "PageOpen.h"
#include "BitestDlg.h"

// CPageOpen 对话框

extern CSorDllUtil g_sorUtil;

IMPLEMENT_DYNAMIC(CPageOpen, CDialog)

CPageOpen::CPageOpen(CWnd* pParent /*=NULL*/)
	: CDialog(CPageOpen::IDD, pParent)
{
	//初始化数据长度为零,为以后作是否有数据的判断
	for(int i = 0; i < 3; i++)
		m_sorDATA[i].Length = 0;
}

CPageOpen::~CPageOpen()
{
}

void CPageOpen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageOpen, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_OPEN_A, &CPageOpen::OnOpenA)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_OPEN_B, &CPageOpen::OnOpenB)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_OPEN_OK, &CPageOpen::OnOpenOk)
	ON_BN_CLICKED(IDC_BUTTON_PAGE_OPEN_CANCEL, &CPageOpen::OnOpenCancel)
END_MESSAGE_MAP()


// CPageOpen 消息处理程序

//打开A到B的测试文件
void CPageOpen::OnOpenA()
{
	CFileDialog dlg(TRUE,_T("*.SOR"),NULL,OFN_HIDEREADONLY,_T("SOR (*.SOR)|*.SOR||"));

	CString path;
	//path = m_reg.readKey(REPORT_RPT_PATH_REG);
	path = m_util.ReadConfigInfo(_T("FileOpen"), _T("Path1"));
	if(""==path)//首次打开曲线，则路径指向软件安装所在的路径 zll 2012.10.30
	{
		path= m_util.getExePath()+REPORT_RPT_DEFAULT_PATH;
	}
	dlg.m_ofn.lpstrInitialDir=path;
	if(dlg.DoModal() != IDOK)
		return;

	if (path!=dlg.m_ofn.lpstrFile)
//		m_reg.writeKey(REPORT_RPT_PATH_REG,dlg.m_ofn.lpstrFile);//软件安装路径与当前打开曲线的路径不同，则保存当前路径
	    m_util.WriteConfigInfo(_T("FileOpen"), _T("Path1"), dlg.m_ofn.lpstrFile);
	else
//		m_reg.writeKey(REPORT_RPT_PATH_REG,path);////软件安装路径与当前打开曲线的路径相同，则保存软件安装路径
        m_util.WriteConfigInfo(_T("FileOpen"), _T("Path1"), path);

	CString strFilePath=dlg.GetPathName();//路径加文件名
	CString strFileName=dlg.GetFileName();//得到文件名
	m_util.WriteConfigInfo(_T("FileOpen"), _T("Name1"), strFileName);
	GetDlgItem(IDC_EDIT_PAGE_OPEN_A)->SetWindowText(strFilePath);
	CBitestDlg *pDlg = (CBitestDlg*)(AfxGetApp()->GetMainWnd());
	pDlg->m_testResult[0].strFileName = strFileName;
	//读取文件 ghq 2016.5.4 更新SOR库文件
	//CSorFile sorFile;
	INT nRes = 0;
	//TestResultInfo* pTestResultInfo;

	//ghq 2016.5.6 设置文件路径名称
	pDlg->SetFileText(strFilePath,0);

	try
	{
		nRes = g_sorUtil.readSorFile(strFilePath, m_sorDATA[0]);
		Sleep(100);
		//m_sorAssist.convert(m_sorDATA[0], m_testResInfo[0]);

		//int x = 0;///////////////////////////////////////////////////////////////////////////////
		//m_sorDrawStatic.EventToGaborResult(g_sorFileArray.sorFileList[index].pTestResultInfo);

		//pTestResultInfo=g_sorFileArray.sorFileList[index].pTestResultInfo;
		//zll,2012.9.13 增加DataSpacing的计算

		//g_sorFileArray.sorFileList[index].pTestResultInfo->DataSpacing = m_sorDrawStatic.Get_XCoord2nf(1,g_sorFileArray.sorFileList[index].pTestResultInfo);
	}
	catch (CException* e)
	{
		nRes = 0;
	}
}

//打开B到A的测试文件
void CPageOpen::OnOpenB()
{
	CFileDialog dlg(TRUE,_T("*.SOR"),NULL,OFN_HIDEREADONLY,_T("SOR (*.SOR)|*.SOR||"));

	CString path;
	path = m_util.ReadConfigInfo(_T("FileOpen"), _T("Path2"));
	if(""==path)//首次打开曲线，则路径指向软件安装所在的路径 zll 2012.10.30
	{
		path = m_util.getExePath()+REPORT_RPT_DEFAULT_PATH;
	}
	dlg.m_ofn.lpstrInitialDir=path;
	if(dlg.DoModal() != IDOK)
		return;

	if (path!=dlg.m_ofn.lpstrFile)
        m_util.WriteConfigInfo(_T("FileOpen"), _T("Path2"), dlg.m_ofn.lpstrFile);
	else
        m_util.WriteConfigInfo(_T("FileOpen"), _T("Path2"), path);

	CString strFilePath=dlg.GetPathName();//得到文件名
	CString strFileName=dlg.GetFileName();//路径加文件名
	m_util.WriteConfigInfo(_T("FileOpen"), _T("Name2"), strFileName);
	GetDlgItem(IDC_EDIT_PAGE_OPEN_B)->SetWindowText(dlg.GetPathName());
	CBitestDlg *pDlg = (CBitestDlg*)(AfxGetApp()->GetMainWnd());
	pDlg->m_testResult[1].strFileName = strFileName;
	//读取文件 ghq 2016.5.4 更新SOR库文件
	//CSorFile sorFile;
	//CSorDllUtil sorUtil;
	INT nRes = 0;
	//TestResultInfo* pTestResultInfo;

	//ghq 2016.5.6 设置文件路径名称
	pDlg->SetFileText(strFilePath,1);

	try
	{
		nRes = g_sorUtil.readSorFile(strFilePath, m_sorDATA[1]);
		//m_sorAssist.convert(m_sorDATA[1], m_testResInfo[1]);

		//m_sorAssist.convert(m_sorDATA,*g_sorFileArray.sorFileList[index].pTestResultInfo);
		//m_sorDrawStatic.EventToGaborResult(g_sorFileArray.sorFileList[index].pTestResultInfo);

		//pTestResultInfo=g_sorFileArray.sorFileList[index].pTestResultInfo;
		//zll,2012.9.13 增加DataSpacing的计算

		//g_sorFileArray.sorFileList[index].pTestResultInfo->DataSpacing = m_sorDrawStatic.Get_XCoord2nf(1,g_sorFileArray.sorFileList[index].pTestResultInfo);
	}
	catch (CException* e)
	{
		nRes = 0;
	}
}

//数据文件打开确认
void CPageOpen::OnOpenOk()
{
	CBitestDlg *pDlg = (CBitestDlg*)(AfxGetApp()->GetMainWnd());
	m_sorAssist.convert(m_sorDATA[0], pDlg->m_testResult[0]);
	m_sorAssist.convert(m_sorDATA[1], pDlg->m_testResult[1]);
	//m_sorAssist.convert(m_sorDATA[0], pDlg->m_testResult[2]);   //将混合双向的数据初始化为a->b的数据
	m_dlgParent->OnDown(IDD_PAGE_OPEN_FILE);
	OnOK();
}

//取消
void CPageOpen::OnOpenCancel()
{
	OnCancel();
}

void CPageOpen::SetLocaleString()
{
	CString strTitle = m_util.ReadLangString(_T("MainDlg"),_T("IDS_OPEN_FILE_TITLE"));
	SetWindowText(strTitle);
	strTitle = m_util.ReadLangString(_T("MainDlg"),_T("IDS_OPEN_FILE1_LABEL"));
	GetDlgItem(IDC_STATIC_AB)->SetWindowText(strTitle);
	strTitle = m_util.ReadLangString(_T("MainDlg"),_T("IDS_OPEN_FILE2_LABEL"));
	GetDlgItem(IDC_STATIC_BA)->SetWindowText(strTitle);
	strTitle = m_util.ReadLangString(_T("MainDlg"),_T("IDS_OPEN_FILE1_LABEL"));
	GetDlgItem(IDC_BUTTON_PAGE_OPEN_A)->SetWindowText(strTitle);
	strTitle = m_util.ReadLangString(_T("MainDlg"),_T("IDS_OPEN_FILE2_LABEL"));
	GetDlgItem(IDC_BUTTON_PAGE_OPEN_B)->SetWindowText(strTitle);
	strTitle = m_util.ReadLangString(_T("MainDlg"),_T("IDS_BUTTON_OK"));
	GetDlgItem(IDC_BUTTON_PAGE_OPEN_OK)->SetWindowText(strTitle);
	strTitle = m_util.ReadLangString(_T("MainDlg"),_T("IDS_BUTTON_CANCEL"));
	GetDlgItem(IDC_BUTTON_PAGE_OPEN_CANCEL)->SetWindowText(strTitle);
}

