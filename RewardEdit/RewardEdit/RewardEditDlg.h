
// RewardEditDlg.h : 头文件
//

#pragma once
#include "editlistctrl.h"
#include <fstream>
#include "FileReadWrite.h"
using std::fstream;
using std::ios_base;


// CRewardEditDlg 对话框
class CRewardEditDlg : public CDialogEx
{
// 构造
public:
	CRewardEditDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_REWARDEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
