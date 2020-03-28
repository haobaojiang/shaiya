
// RewardEditDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RewardEdit.h"
#include "RewardEditDlg.h"
#include "afxdialogex.h"

#include <vector>
using std::vector;

//����
std::wstring g_wsTitle[] = { L"Index",
	                         L"Upper",
	                         L"Pants",
							 L"Hand",
	                         L"Shoots",
	                         L"Face",
	                         L"Helmet" };
//���ݵĽṹ��
typedef struct _DualLayer
{
	WORD  wIndex;    //ƫ��
	WORD  wUpper;    //����
	WORD  wPants;    //����
	WORD  wHand;     //����
	WORD  wShoots;   //Ь��
	WORD  wUnknow;   //δ֪
	WORD  wHelmet;   //ͷ��
}DualLayer, *PDualLayer;

/*
#ifdef _DEBUG
WCHAR szPath[] = L"E:\\Shaiya Develop\\1���ܼ���\\����\\DualLayerClothes.sdata";
#else
WCHAR szPath[] = L"DualLayerClothes.sdata";
#endif*/




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
	for (DWORD i = 0; i < _countof(g_wsTitle); i++)
	{
		m_ListCtrl.InsertColumn(i, g_wsTitle[i].c_str(), 0, 60);
	}
	m_ListCtrl.SetExtendedStyle(m_ListCtrl.GetExtendedStyle()|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);//��������

	//��ȡ�ļ�
	OnBnClickedButton2();

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



//************************************
// ��������: ��ȡ�޸ĵ�����
// ����˵��: 
//************************************

void CRewardEditDlg::OnBnClickedButton2()
{
    //0.��ȡ�ļ�·��
	CString szPath;
	CFileDialog objDialog(TRUE, 0, L"DualLayerClothes.sdata", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"sdata|DualLayerClothes.sdata| All Files(*.*)| *.*");
	if (objDialog.DoModal() == IDOK)//�Ƿ�򿪳ɹ�
	{
		szPath = objDialog.GetPathName();//ȡ���ļ�·�����ļ���
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

	//1.��սʱװ����
	DWORD dwCount;
	obj.ReadData(&dwCount, sizeof(DWORD));

	
	//2.��ȡ���ݽ��ṹ��
	PDualLayer pDualLayer = new DualLayer[dwCount];
	obj.ReadData(pDualLayer, sizeof(DualLayer)*dwCount);

	for (DWORD i = 0; i < 3; i++)
	{
		m_ListCtrl.InsertItem(0, L"");
	}

	//3.�ѽṹ��������ݲ���
	for (int i = dwCount - 1; i >= 0; i--)
	{
		DualLayer stcDualLayer = pDualLayer[i];
		WCHAR szTemp[MAX_PATH] = { 0 };
		//���
		swprintf_s(szTemp, L"%d", stcDualLayer.wIndex);
		m_ListCtrl.InsertItem(0, szTemp);
		//����
		swprintf_s(szTemp, L"%d", stcDualLayer.wUpper);
		m_ListCtrl.SetItemText(0, 1, szTemp);
		//����
		swprintf_s(szTemp, L"%d", stcDualLayer.wPants);
		m_ListCtrl.SetItemText(0, 2, szTemp);
		//����
		swprintf_s(szTemp, L"%d", stcDualLayer.wHand);
		m_ListCtrl.SetItemText(0, 3, szTemp);
		//Ь��
		swprintf_s(szTemp, L"%d", stcDualLayer.wShoots);
		m_ListCtrl.SetItemText(0, 4, szTemp);
		//δ֪
		swprintf_s(szTemp, L"%d", stcDualLayer.wUnknow);
		m_ListCtrl.SetItemText(0, 5, szTemp);
		//ͷ��
		swprintf_s(szTemp, L"%d", stcDualLayer.wHelmet);
		m_ListCtrl.SetItemText(0, 6, szTemp);
	}

	delete[] pDualLayer;
 	return;
}



//************************************
// ��������: д���޸ĵ�����
// ����˵��: 
//************************************

void CRewardEditDlg::OnBnClickedButton1()
{
	//0.ѡ��·��
	CString szPath;
	CFileDialog fileDlg(0,  // TRUE��Open��FALSE����Save As�ļ��Ի���
		L".sdata",            // Ĭ�ϵĴ��ļ�������
		L"DualLayerClothes",  // Ĭ�ϴ򿪵��ļ��� 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR,  // ��ѡ��
		L"sdata|DualLayerClothes.sdata| All Files(*.*)| *.*||");  // �򿪵��ļ�����
		

	//fileDlg.m_ofn.lpstrInitialDir = strPath;//��ʼ��·����
	if (fileDlg.DoModal() == IDOK)
	{
		szPath = fileDlg.GetPathName();//����ѡ���������ļ����ƣ�
	}
	else
	{
		return;
	}


	//1.��ȡ����
	int i = 0;  //��
	vector<DualLayer> vecDualLayer;
	while (TRUE)
	{
		DualLayer stcDualLayer;
		PWORD pDual = (PWORD)&stcDualLayer;
		for (int j = 0; j < sizeof(DualLayer) / sizeof(WORD); j++)//��
		{
			WCHAR szTemp[MAX_PATH] = { 0 };
			m_ListCtrl.GetItemText(i, j, szTemp, MAX_PATH);
			if (NULL==szTemp[0]) goto __exit;                      //�����˳�
			pDual[j] = _tcstoul(szTemp, 0, 10);
		}
		i++;
		vecDualLayer.push_back(stcDualLayer);
	}
__exit:
	if (!vecDualLayer.size()) return;

	//2.�����ļ�
	WCHAR szBackupFile[MAX_PATH] = { 0 };
	swprintf_s(szBackupFile, MAX_PATH, L"%s.bak", szPath.GetBuffer());
	szPath.ReleaseBuffer();
	CopyFile(szPath, szBackupFile, 0);
	DeleteFile(szPath);
	//3.д���ļ�
	HANDLE hFile= CreateFile(szPath, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

	 //3.1 д��ʱװ����
	DWORD dwWritenSize = 0;
	WriteFile(hFile, &i, sizeof(i), &dwWritenSize, 0);

	 //3.2 д��ʱװ�Ľṹ��
	for (DWORD j = 0; j < vecDualLayer.size(); j++)
	{
		WriteFile(hFile, &vecDualLayer[j], sizeof(DualLayer), &dwWritenSize, 0);
	}

	CloseHandle(hFile);

	::MessageBox(0,L"success",L"byܳ��",MB_OK);
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
