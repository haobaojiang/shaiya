
// RewardEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RewardEdit.h"
#include "RewardEditDlg.h"
#include "afxdialogex.h"

//多少个阶段

/*客户要求的设置基址*/
//LONG  nKillBase=0x2fe4d0;       //第一阶段的战功要求
//LONG  nUmBase=0x2fe450;         //死亡属性点基址

/*9.0的设置基址*/
//int  nKillBase=0x308298;          //第一阶段的战功要求基址
//int  nUmBase=0x308218;            //死亡属性点基址
//int  nVipColorBase=0x4b609;       //VIP角色名颜色基址

/*3.0的设置基址*/
//int  nKillBase=0x235b28;          //第一阶段的战功要求基址
//int  nUmBase=0x235b00;            //死亡属性点基址
//int  nVipColorBase=0x4b609;       //VIP角色名颜色基址


/*威奇的基址*/

int  nKillBase = 0x002fe4d0;          //第一阶段的战功要求基址
int  nUmBase = nKillBase-0x80;            //死亡属性点基址
//int  nVipColorBase = 0x4b609;       //VIP角色名颜色基址


#define nArrayCount 31


LONG nHardBase=nUmBase-0x80;      //困难属性点基址
LONG nNormalBase=nHardBase-0x80;  //简单属性点基址
UINT nReqKill[nArrayCount];       //每阶段战功要求
UINT nNormPoint 	 [nArrayCount];//普通属性点奖励
UINT nHardPoint 	 [nArrayCount];//困难属性点奖励
UINT nUmPoint   	 [nArrayCount];//死亡属性点奖励
BYTE byColor         [3] ;//颜色
WCHAR szPath[]=L"game.exe";

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
	m_ListCtrl.InsertColumn(0,L"阶段",0,100);
	m_ListCtrl.InsertColumn(1,L"战功要求",0,100);
	m_ListCtrl.InsertColumn(2,L"普通",0,100);
	m_ListCtrl.InsertColumn(3,L"困难",0,100);
	m_ListCtrl.InsertColumn(4,L"死亡",0,100);
	m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);//设置属性

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



void CRewardEditDlg::OnBnClickedButton2()
{
    m_ListCtrl.DeleteAllItems();
	////************************循环从文件中读取数据************************************/
	CFileReadWrite obj;
	obj.Open(szPath);
	//读战功阶段
	obj.SetOffset(nKillBase);
    obj.ReadData(nReqKill,sizeof(int)*_countof(nReqKill));
	//读死亡属性点
	obj.SetOffset(nUmBase);
	obj.ReadData(nUmPoint,sizeof(int)*_countof(nUmPoint));
	//读困难属性点
	obj.SetOffset(nHardBase);
	obj.ReadData(nHardPoint,sizeof(int)*_countof(nHardPoint));
	//读简单模式属性点
	obj.SetOffset(nNormalBase);
	obj.ReadData(nNormPoint,sizeof(int)*_countof(nNormPoint));
	//读GM角色名颜色
// 	obj.SetOffset(nVipColorBase);
// 	obj.ReadData(byColor,3);
	///*循环放入ListCtrl*/
	WCHAR szTemp[MAX_PATH]={0};
	for(int i=nArrayCount-1;i>=0;i--)
	{
		//阶段
		swprintf_s(szTemp,L"阶段%d",i+1);
		m_ListCtrl.InsertItem(0,szTemp);
		//战功要求
		ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
		swprintf_s(szTemp,L"%d",nReqKill[i]);
		m_ListCtrl.SetItemText(0,1,szTemp);
		//普通属性点
		ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
		swprintf_s(szTemp,L"%d",nNormPoint[i]);
		m_ListCtrl.SetItemText(0, 2,szTemp);
		//困难属性点
		ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
		swprintf_s(szTemp,L"%d",nHardPoint[i]);
		m_ListCtrl.SetItemText(0, 3,szTemp);
		//死亡属性点
		ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
		swprintf_s(szTemp,L"%d",nUmPoint[i]);
		m_ListCtrl.SetItemText(0, 4,szTemp);
	}
// 	//VIP角色名字颜色
// 	ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
// 	swprintf_s(szTemp,L"%02x%02x%02x",byColor[2],byColor[1],byColor[0]);
// 	SetDlgItemText(IDC_EDIT_VipColor,szTemp);
	return;
}


void CRewardEditDlg::OnBnClickedButton1()
{
	CFileReadWrite obj;
	obj.Open(szPath);
	////************************写入战功要求******************************************/
	for(int i=0;i<nArrayCount;i++)
	{
		WCHAR szTemp[MAX_PATH]={0};
		m_ListCtrl.GetItemText(i,1,szTemp,MAX_PATH);
		nReqKill[i]=_wtoi(szTemp);
	}
	obj.SetOffset(nKillBase);
	obj.WriteData(nReqKill,sizeof(int)*_countof(nReqKill));
	////************************写入普通属性点要求******************************************/
	for(int i=0;i<nArrayCount;i++)
	{
		WCHAR szTemp[MAX_PATH]={0};
		m_ListCtrl.GetItemText(i,2,szTemp,MAX_PATH);       
		nNormPoint[i]=_wtoi(szTemp);
	}
	obj.SetOffset(nNormalBase);
	obj.WriteData(nNormPoint,sizeof(int)*_countof(nNormPoint));
	////************************写入困难属性点要求******************************************/
	for(int i=0;i<nArrayCount;i++)
	{
		WCHAR szTemp[MAX_PATH]={0};
		m_ListCtrl.GetItemText(i,3,szTemp,MAX_PATH);       
		nHardPoint[i]=_wtoi(szTemp);
	}

	obj.SetOffset(nHardBase);
	obj.WriteData(nHardPoint,sizeof(int)*_countof(nHardPoint));
	////************************写入死亡属性点要求******************************************/
	for(int i=0;i<nArrayCount;i++)
	{
		WCHAR szTemp[MAX_PATH]={0};
		m_ListCtrl.GetItemText(i,4,szTemp,MAX_PATH);       
		nUmPoint[i]=_wtoi(szTemp);
	}
	obj.SetOffset(nUmBase);
	obj.WriteData(nUmPoint,sizeof(int)*_countof(nUmPoint));
	////************************写入VIP角色名颜色******************************************/
// 	obj.SetOffset(nVipColorBase);
// 	WCHAR szTemp[MAX_PATH]={0};
// 	GetDlgItemText(IDC_EDIT_VipColor,szTemp,MAX_PATH);
// 	swscanf_s(szTemp,L"%x",&byColor);
// 	obj.WriteData(&byColor,sizeof(BYTE)*_countof(byColor));

	::MessageBox(0,L"修改成功",L"by艹如",MB_OK);
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
