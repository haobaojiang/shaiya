
// RewardEdit.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRewardEditApp:
// �йش����ʵ�֣������ RewardEdit.cpp
//

class CRewardEditApp : public CWinApp
{
public:
	CRewardEditApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRewardEditApp theApp;