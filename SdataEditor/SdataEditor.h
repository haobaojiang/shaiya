
// SdataEditor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSdataEditorApp: 
// �йش����ʵ�֣������ SdataEditor.cpp
//

class CSdataEditorApp : public CWinApp
{
public:
	CSdataEditorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSdataEditorApp theApp;