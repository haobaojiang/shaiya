
// RewardEditDlg.h : ͷ�ļ�
//

#pragma once
#include "editlistctrl.h"
#include <fstream>
#include "FileReadWrite.h"
using std::fstream;
using std::ios_base;


// CRewardEditDlg �Ի���
class CRewardEditDlg : public CDialogEx
{
// ����
public:
	CRewardEditDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_REWARDEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEditListCtrl m_ListCtrl;
	afx_msg void OnBnClickedButton2();
private:

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
};
