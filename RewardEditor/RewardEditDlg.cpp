
// RewardEditDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RewardEdit.h"
#include "RewardEditDlg.h"
#include "afxdialogex.h"

//���ٸ��׶�

/*�ͻ�Ҫ������û�ַ*/
//LONG  nKillBase=0x2fe4d0;       //��һ�׶ε�ս��Ҫ��
//LONG  nUmBase=0x2fe450;         //�������Ե��ַ

/*9.0�����û�ַ*/
//int  nKillBase=0x308298;          //��һ�׶ε�ս��Ҫ���ַ
//int  nUmBase=0x308218;            //�������Ե��ַ
//int  nVipColorBase=0x4b609;       //VIP��ɫ����ɫ��ַ

/*3.0�����û�ַ*/
//int  nKillBase=0x235b28;          //��һ�׶ε�ս��Ҫ���ַ
//int  nUmBase=0x235b00;            //�������Ե��ַ
//int  nVipColorBase=0x4b609;       //VIP��ɫ����ɫ��ַ


/*����Ļ�ַ*/

int  nKillBase = 0x002fe4d0;          //��һ�׶ε�ս��Ҫ���ַ
int  nUmBase = nKillBase-0x80;            //�������Ե��ַ
//int  nVipColorBase = 0x4b609;       //VIP��ɫ����ɫ��ַ


#define nArrayCount 31


LONG nHardBase=nUmBase-0x80;      //�������Ե��ַ
LONG nNormalBase=nHardBase-0x80;  //�����Ե��ַ
UINT nReqKill[nArrayCount];       //ÿ�׶�ս��Ҫ��
UINT nNormPoint 	 [nArrayCount];//��ͨ���Ե㽱��
UINT nHardPoint 	 [nArrayCount];//�������Ե㽱��
UINT nUmPoint   	 [nArrayCount];//�������Ե㽱��
BYTE byColor         [3] ;//��ɫ
WCHAR szPath[]=L"game.exe";

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRewardEditDlg �Ի���



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


// CRewardEditDlg ��Ϣ�������

BOOL CRewardEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	/*��ʼ���б��*/
	m_ListCtrl.InsertColumn(0,L"�׶�",0,100);
	m_ListCtrl.InsertColumn(1,L"ս��Ҫ��",0,100);
	m_ListCtrl.InsertColumn(2,L"��ͨ",0,100);
	m_ListCtrl.InsertColumn(3,L"����",0,100);
	m_ListCtrl.InsertColumn(4,L"����",0,100);
	m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);//��������

	return TRUE;  
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRewardEditDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRewardEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRewardEditDlg::OnBnClickedButton2()
{
    m_ListCtrl.DeleteAllItems();
	////************************ѭ�����ļ��ж�ȡ����************************************/
	CFileReadWrite obj;
	obj.Open(szPath);
	//��ս���׶�
	obj.SetOffset(nKillBase);
    obj.ReadData(nReqKill,sizeof(int)*_countof(nReqKill));
	//���������Ե�
	obj.SetOffset(nUmBase);
	obj.ReadData(nUmPoint,sizeof(int)*_countof(nUmPoint));
	//���������Ե�
	obj.SetOffset(nHardBase);
	obj.ReadData(nHardPoint,sizeof(int)*_countof(nHardPoint));
	//����ģʽ���Ե�
	obj.SetOffset(nNormalBase);
	obj.ReadData(nNormPoint,sizeof(int)*_countof(nNormPoint));
	//��GM��ɫ����ɫ
// 	obj.SetOffset(nVipColorBase);
// 	obj.ReadData(byColor,3);
	///*ѭ������ListCtrl*/
	WCHAR szTemp[MAX_PATH]={0};
	for(int i=nArrayCount-1;i>=0;i--)
	{
		//�׶�
		swprintf_s(szTemp,L"�׶�%d",i+1);
		m_ListCtrl.InsertItem(0,szTemp);
		//ս��Ҫ��
		ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
		swprintf_s(szTemp,L"%d",nReqKill[i]);
		m_ListCtrl.SetItemText(0,1,szTemp);
		//��ͨ���Ե�
		ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
		swprintf_s(szTemp,L"%d",nNormPoint[i]);
		m_ListCtrl.SetItemText(0, 2,szTemp);
		//�������Ե�
		ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
		swprintf_s(szTemp,L"%d",nHardPoint[i]);
		m_ListCtrl.SetItemText(0, 3,szTemp);
		//�������Ե�
		ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
		swprintf_s(szTemp,L"%d",nUmPoint[i]);
		m_ListCtrl.SetItemText(0, 4,szTemp);
	}
// 	//VIP��ɫ������ɫ
// 	ZeroMemory(szTemp,sizeof(WCHAR)*MAX_PATH);
// 	swprintf_s(szTemp,L"%02x%02x%02x",byColor[2],byColor[1],byColor[0]);
// 	SetDlgItemText(IDC_EDIT_VipColor,szTemp);
	return;
}


void CRewardEditDlg::OnBnClickedButton1()
{
	CFileReadWrite obj;
	obj.Open(szPath);
	////************************д��ս��Ҫ��******************************************/
	for(int i=0;i<nArrayCount;i++)
	{
		WCHAR szTemp[MAX_PATH]={0};
		m_ListCtrl.GetItemText(i,1,szTemp,MAX_PATH);
		nReqKill[i]=_wtoi(szTemp);
	}
	obj.SetOffset(nKillBase);
	obj.WriteData(nReqKill,sizeof(int)*_countof(nReqKill));
	////************************д����ͨ���Ե�Ҫ��******************************************/
	for(int i=0;i<nArrayCount;i++)
	{
		WCHAR szTemp[MAX_PATH]={0};
		m_ListCtrl.GetItemText(i,2,szTemp,MAX_PATH);       
		nNormPoint[i]=_wtoi(szTemp);
	}
	obj.SetOffset(nNormalBase);
	obj.WriteData(nNormPoint,sizeof(int)*_countof(nNormPoint));
	////************************д���������Ե�Ҫ��******************************************/
	for(int i=0;i<nArrayCount;i++)
	{
		WCHAR szTemp[MAX_PATH]={0};
		m_ListCtrl.GetItemText(i,3,szTemp,MAX_PATH);       
		nHardPoint[i]=_wtoi(szTemp);
	}

	obj.SetOffset(nHardBase);
	obj.WriteData(nHardPoint,sizeof(int)*_countof(nHardPoint));
	////************************д���������Ե�Ҫ��******************************************/
	for(int i=0;i<nArrayCount;i++)
	{
		WCHAR szTemp[MAX_PATH]={0};
		m_ListCtrl.GetItemText(i,4,szTemp,MAX_PATH);       
		nUmPoint[i]=_wtoi(szTemp);
	}
	obj.SetOffset(nUmBase);
	obj.WriteData(nUmPoint,sizeof(int)*_countof(nUmPoint));
	////************************д��VIP��ɫ����ɫ******************************************/
// 	obj.SetOffset(nVipColorBase);
// 	WCHAR szTemp[MAX_PATH]={0};
// 	GetDlgItemText(IDC_EDIT_VipColor,szTemp,MAX_PATH);
// 	swscanf_s(szTemp,L"%x",&byColor);
// 	obj.WriteData(&byColor,sizeof(BYTE)*_countof(byColor));

	::MessageBox(0,L"�޸ĳɹ�",L"byܳ��",MB_OK);
}


void CRewardEditDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}


void CRewardEditDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnClose();
}
