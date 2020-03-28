
// RewardEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RewardEdit.h"
#include "RewardEditDlg.h"
#include "afxdialogex.h"

#include <vector>
using std::vector;

//标题
std::wstring g_wsTitle[] = { L"Index",
	                         L"Upper",
	                         L"Pants",
							 L"Hand",
	                         L"Shoots",
	                         L"Face",
	                         L"Helmet" };
//数据的结构体
typedef struct _DualLayer
{
	WORD  wIndex;    //偏号
	WORD  wUpper;    //上衣
	WORD  wPants;    //护腿
	WORD  wHand;     //护手
	WORD  wShoots;   //鞋子
	WORD  wUnknow;   //未知
	WORD  wHelmet;   //头盔
}DualLayer, *PDualLayer;

/*
#ifdef _DEBUG
WCHAR szPath[] = L"E:\\Shaiya Develop\\1解密加密\\解密\\DualLayerClothes.sdata";
#else
WCHAR szPath[] = L"DualLayerClothes.sdata";
#endif*/




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRewardEditDlg 对话框



CRewardEditDlg::CRewardEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRewardEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRewardEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
}

BEGIN_MESSAGE_MAP(CRewardEditDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &CRewardEditDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &CRewardEditDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CRewardEditDlg 消息处理程序

BOOL CRewardEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	/*初始化列表框*/
	for (DWORD i = 0; i < _countof(g_wsTitle); i++)
	{
		m_ListCtrl.InsertColumn(i, g_wsTitle[i].c_str(), 0, 60);
	}
	m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);//设置属性

	//读取文件
	OnBnClickedButton2();

	return TRUE;  
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRewardEditDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRewardEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//************************************
// 函数作用: 读取修改的内容
// 参数说明: 
//************************************

void CRewardEditDlg::OnBnClickedButton2()
{
    //0.获取文件路径
	CString szPath;
	CFileDialog objDialog(TRUE, 0, L"DualLayerClothes.sdata", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"sdata|DualLayerClothes.sdata| All Files(*.*)| *.*");
	if (objDialog.DoModal() == IDOK)//是否打开成功
	{
		szPath = objDialog.GetPathName();//取得文件路径及文件名
	}
	else
	{
		return;
	}


    m_ListCtrl.DeleteAllItems();
	CFileReadWrite obj;
	if (!obj.Open(szPath.GetBuffer()))
	{
		szPath.ReleaseBuffer();
		ExitProcess(0);
	}
	szPath.ReleaseBuffer();

	//1.读战时装个数
	DWORD dwCount;
	obj.ReadData(&dwCount, sizeof(DWORD));

	
	//2.读取内容进结构体
	PDualLayer pDualLayer = new DualLayer[dwCount];
	obj.ReadData(pDualLayer, sizeof(DualLayer)*dwCount);

	for (DWORD i = 0; i < 3; i++)
	{
		m_ListCtrl.InsertItem(0, L"");
	}

	//3.把结构体里的内容插入
	for (int i = dwCount - 1; i >= 0; i--)
	{
		DualLayer stcDualLayer = pDualLayer[i];
		WCHAR szTemp[MAX_PATH] = { 0 };
		//序号
		swprintf_s(szTemp, L"%d", stcDualLayer.wIndex);
		m_ListCtrl.InsertItem(0, szTemp);
		//上衣
		swprintf_s(szTemp, L"%d", stcDualLayer.wUpper);
		m_ListCtrl.SetItemText(0, 1, szTemp);
		//护腿
		swprintf_s(szTemp, L"%d", stcDualLayer.wPants);
		m_ListCtrl.SetItemText(0, 2, szTemp);
		//护手
		swprintf_s(szTemp, L"%d", stcDualLayer.wHand);
		m_ListCtrl.SetItemText(0, 3, szTemp);
		//鞋子
		swprintf_s(szTemp, L"%d", stcDualLayer.wShoots);
		m_ListCtrl.SetItemText(0, 4, szTemp);
		//未知
		swprintf_s(szTemp, L"%d", stcDualLayer.wUnknow);
		m_ListCtrl.SetItemText(0, 5, szTemp);
		//头盔
		swprintf_s(szTemp, L"%d", stcDualLayer.wHelmet);
		m_ListCtrl.SetItemText(0, 6, szTemp);
	}

	delete[] pDualLayer;
 	return;
}



//************************************
// 函数作用: 写入修改的内容
// 参数说明: 
//************************************

void CRewardEditDlg::OnBnClickedButton1()
{
	//0.选择路径
	CString szPath;
	CFileDialog fileDlg(0,  // TRUE打开Open，FALSE保存Save As文件对话框
		L".sdata",            // 默认的打开文件的类型
		L"DualLayerClothes",  // 默认打开的文件名 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // 单选打开
		L"sdata|DualLayerClothes.sdata| All Files(*.*)| *.*||");  // 打开的文件类型
		

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//初始化路径。
	if (fileDlg.DoModal() == IDOK)
	{
		szPath = fileDlg.GetPathName();//返回选择或输入的文件名称，
	}
	else
	{
		return;
	}


	//1.读取数据
	int i = 0;  //行
	vector<DualLayer> vecDualLayer;
	while (TRUE)
	{
		DualLayer stcDualLayer;
		PWORD pDual = (PWORD)&stcDualLayer;
		for (int j = 0; j < sizeof(DualLayer) / sizeof(WORD); j++)//列
		{
			WCHAR szTemp[MAX_PATH] = { 0 };
			m_ListCtrl.GetItemText(i, j, szTemp, MAX_PATH);
			if (NULL==szTemp[0]) goto __exit;                      //空行退出
			pDual[j] = _tcstoul(szTemp, 0, 10);
		}
		i++;
		vecDualLayer.push_back(stcDualLayer);
	}
__exit:
	if (!vecDualLayer.size()) return;

	//2.备份文件
	WCHAR szBackupFile[MAX_PATH] = { 0 };
	swprintf_s(szBackupFile, MAX_PATH, L"%s.bak", szPath.GetBuffer());
	szPath.ReleaseBuffer();
	CopyFile(szPath, szBackupFile, 0);
	DeleteFile(szPath);
	//3.写入文件
	HANDLE hFile= CreateFile(szPath, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

	 //3.1 写入时装个数
	DWORD dwWritenSize = 0;
	WriteFile(hFile, &i, sizeof(i), &dwWritenSize, 0);

	 //3.2 写入时装的结构体
	for (DWORD j = 0; j < vecDualLayer.size(); j++)
	{
		WriteFile(hFile, &vecDualLayer[j], sizeof(DualLayer), &dwWritenSize, 0);
	}

	CloseHandle(hFile);

	::MessageBox(0,L"success",L"by艹如",MB_OK);
}


void CRewardEditDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}


void CRewardEditDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnClose();
}
