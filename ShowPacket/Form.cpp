
#include "stdafx.h"
#include "Form.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <vector>
#include <string>
#include <MyPython.h>


#pragma comment(lib,"user32.lib")
#pragma comment(lib,"Gdi32.lib")


using std::vector;

HWND FormHandle = 0;
HWND FormHandle2 = 0;
HWND FormHandle3 = 0;
bool IsDialogVisible = true;
bool IsLoggingPackets = false;

extern bool IsBotting;
extern DWORD Target;
extern DWORD HuntingRadius;
extern bool LoggedIn;
extern bool PickItems;
extern bool DoSellItems;
extern bool DoRepairEquipment;
extern bool UseOnlySkills;
extern bool BotReady;
extern DWORD Killcount;
extern DWORD AttackDistance;


extern CRITICAL_SECTION g_cs;

char Account[32] = "";


MyPython* g_objPy = new MyPython("MyPacket");

void SendPacket(char *buf, int len);
void RecvPacket(char *buf, DWORD len);

void TargetNextMonster();
void AttackTarget();
void SetHuntingArea(DWORD Radius);
void IsUsingSkill(BYTE Use, bool IsUsing);
void StartBot();
void StopBot();


vector<std::string> g_vecExcutedList;

BOOL CALLBACK Dlgproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{


				case IDC_BUTTON5:
					ShowWindow(hWnd, SW_HIDE);
					IsDialogVisible = false;
					break;

				case IDC_BUTTON8:
					ShowWindow(FormHandle2, SW_SHOW);
					ShowWindow(FormHandle3, SW_HIDE);
					break;
			}
	}
	return FALSE;
}

BOOL CALLBACK Dlgproc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BUTTON1:
					SendPacketInEdit(GetDlgItem(hWnd, IDC_EDIT1));
					break;
				case IDC_BUTTON2:
					ClearListBox(GetDlgItem(hWnd, IDC_LIST1));
					break;
				case IDC_BUTTON3:
					SelectedPacketsToClipboard(GetDlgItem(hWnd, IDC_LIST1));
					break;
				case IDC_BUTTON4:
					AllPacketsToClipboard(GetDlgItem(hWnd, IDC_LIST1));
					break;
				case IDC_BUTTON5:
					SendRecvInEdit(GetDlgItem(hWnd, IDC_EDIT5));
					break;
				case IDC_BUTTON6://激活显示
					IsLoggingPackets = TRUE;
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON6), false);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON7), true);
					break;
				case IDC_BUTTON7://取消显示
					IsLoggingPackets = FALSE;
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON6), true);
					EnableWindow(GetDlgItem(hWnd, IDC_BUTTON7), false);
					break;

				case IDC_Exclude:
					AddExIncludePacket(GetDlgItem(hWnd, IDC_LIST1));
					break;



			}
	}
	return FALSE;
}



DWORD ShowDialog(HMODULE hInstance)
{
	MSG msg;
	HWND hWnd;
	HWND hWnd2;

	hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)Dlgproc);
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
	ShowWindow(hWnd, SW_SHOW);
	FormHandle = hWnd;
	

	hWnd2 = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, (DLGPROC)Dlgproc2);
	ShowWindow(hWnd2, SW_SHOW);
	FormHandle2 = hWnd2;

	IsDialogVisible = TRUE;


	HFONT hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE,FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH|FF_SWISS, "Lucida Console");
	SendMessage(GetDlgItem(hWnd2, IDC_LIST1), WM_SETFONT, (WPARAM)hFont, 0xFFFFFFFF);

	while(GetMessage(&msg, hWnd, 0, 0)>0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

//显示封包在窗体上
void DisplayPacket(char *direction, char *buf, int len)
{
	//是否要显示
	if (!IsLoggingPackets)
		return;


	char *Packet = new char[10000];
	strcpy_s(Packet, 10000, direction);                                                 //头部插入send or recv
	CreatePacketString((BYTE *)buf, len, Packet);                                       //然后组合成字符串

	bool isIgnore = false;
	try
	{
		g_objPy->lock();
		isIgnore = boost::python::call_method<bool>(g_objPy->m_module->get(), "packet_arrived", (const char*)Packet);
		g_objPy->Unlock();
	}
	catch (...)
	{
		g_objPy->Unlock();
	}


	if (!isIgnore)
		SendMessage(GetDlgItem(FormHandle2, IDC_LIST1), LB_ADDSTRING, NULL, (LPARAM)Packet);//把字符串发送到窗体上
	delete[] Packet;
}

//组合封包字符串
void CreatePacketString(BYTE *BufferPointer, DWORD BufferLength, char *PacketString)
{
	char PacketByte[4];
	for (DWORD i = 0; i < BufferLength; i++)
	{
		sprintf_s(PacketByte, 4, "%02X ", BufferPointer[i]);
		strcat_s(PacketString, 10000, PacketByte);
	}
}

void FormatMyString(char *src, char *dst)
{
	DWORD i = 0;
	while (src[i])
	{
		dst[i] = toupper(src[i]);
		i++;
	}
	i = 0;
	char temp[1000] = "";
	while (dst[i])
	{
		if ((dst[i] != '0') && (dst[i] != '1') && (dst[i] != '2') && (dst[i] != '3') &&
			(dst[i] != '4') && (dst[i] != '5') && (dst[i] != '6') && (dst[i] != '7') &&
			(dst[i] != '8') && (dst[i] != '9') && (dst[i] != 'A') && (dst[i] != 'B') &&
			(dst[i] != 'C') && (dst[i] != 'D') && (dst[i] != 'E') && (dst[i] != 'F'))
		{
			strcpy_s(temp, 1000, &dst[i+1]);
			strcpy_s(&dst[i], 1000, temp);
		}
		else
			i++;
	}

	i = 2;
	while (dst[i])
	{
		strcpy_s(temp, 1000, &dst[i]);
		strcpy_s(&dst[i+1], 1000, temp);
		dst[i] = ' ';
		i += 3;
	}
}

DWORD StringToByteArray(char *str, char *pbyte)
{
	DWORD ArraySize = ((strlen(str) + 1) / 3);
	char *pEnd = str;
	for (DWORD i = 0; i < ArraySize; i++)
	{
		pbyte[i] = (BYTE)strtol(pEnd, &pEnd, 16);
	}
	return ArraySize;
}

void SendPacketInEdit(HWND Edit)
{
	char *Packet = new char[1000];
	memset(Packet, 0, 1000);
	char PacketString[1000];
	char FormatedPacketString[1000] = "";
	GetWindowTextA(Edit, PacketString, 1000);
	FormatMyString(PacketString, FormatedPacketString);
	SetWindowTextA(Edit, FormatedPacketString);
	DWORD PacketLength = StringToByteArray(FormatedPacketString, Packet);
	SendPacket(Packet, PacketLength);
}

void SendRecvInEdit(HWND Edit)
{
	char *Packet = new char[1000];
	memset(Packet, 0, 1000);
	char PacketString[1000];
	char FormatedPacketString[1000] = "";
	GetWindowTextA(Edit, PacketString, 1000);
	FormatMyString(PacketString, FormatedPacketString);
	SetWindowTextA(Edit, FormatedPacketString);
	DWORD PacketLength = StringToByteArray(FormatedPacketString, Packet);
	RecvPacket(Packet, PacketLength);
}



void ClearListBox(HWND ListBox)
{
	DWORD ListCount = SendMessage(ListBox, LB_GETCOUNT, NULL, NULL);
	for (DWORD i = 0; i < ListCount; i++)
		PostMessage(ListBox, LB_DELETESTRING, 0, NULL);
}

void SelectedPacketsToClipboard(HWND ListBox)
{
	char *Packet = new char[1000000];
	Packet[0] = '\0';
	DWORD Count = SendMessage(ListBox, LB_GETCOUNT, 0, 0);
	if ((Count != 0) && (Count != LB_ERR))
	{		
		for (DWORD i = 0; i < Count; i++)
		{
			if (SendMessage(ListBox, LB_GETSEL, i, 0) > 0)
			{
				DWORD Size = strlen(Packet);
				if (Size > 0)
				{
					Packet[Size] = '\r';
					Packet[Size+1] = '\n';
					SendMessage(ListBox, LB_GETTEXT, i, (LPARAM)&Packet[Size+2]);
				}
				else
					SendMessage(ListBox, LB_GETTEXT, i, (LPARAM)Packet);
			}
		}
		if (strlen(Packet) > 0)
			CopyToClipboard(Packet);
	}
	delete Packet;
}

void AddExIncludePacket(HWND ListBox)
{
	char *Packet = new char[1000000];
	Packet[0] = '\0';
	DWORD Count = SendMessage(ListBox, LB_GETCOUNT, 0, 0);
	if ((Count != 0) && (Count != LB_ERR))
	{
		for (DWORD i = 0; i < Count; i++)
		{
			if (SendMessage(ListBox, LB_GETSEL, i, 0) > 0)
			{
				ZeroMemory(Packet, 1000000);
				SendMessage(ListBox, LB_GETTEXT, i, (LPARAM)Packet);

				//交给python进行逻辑处理
				try
				{
					g_objPy->lock();
					 boost::python::call_method<void>(g_objPy->m_module->get(), "AddExInclude", (const char*)Packet);
					g_objPy->Unlock();
				}
				catch (...)
				{
					g_objPy->Unlock();
				}
			}
		}
	}
	delete[] Packet;
}

void AllPacketsToClipboard(HWND ListBox)
{
	char *Packet = new char[100000];
	Packet[0] = '\0';
	DWORD Count = SendMessage(ListBox, LB_GETCOUNT, 0, 0);
	if ((Count != 0) && (Count != LB_ERR))
	{						
		for (DWORD i = 0; i < Count; i++)
		{
			DWORD Size = strlen(Packet);
			if (Size > 0)
			{
				Packet[Size] = '\r';
				Packet[Size+1] = '\n';
				SendMessage(ListBox, LB_GETTEXT, i, (LPARAM)&Packet[Size+2]);
			}
			else
				SendMessage(ListBox, LB_GETTEXT, i, (LPARAM)Packet);
		}	
		CopyToClipboard(Packet);
	}
	delete Packet;
}

void CopyToClipboard(char *source)
{
	OpenClipboard(NULL);
	EmptyClipboard();
	HANDLE hMem;
	char *lpMem;
	DWORD szSize = strlen(source);
	hMem = GlobalAlloc(GMEM_MOVEABLE, szSize + 1);
	lpMem = (char *)GlobalLock(hMem);
	memcpy(lpMem, source, strlen(source) + 1);
	GlobalUnlock(hMem);
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}

extern DWORD Target;


//获取键盘消息F5显示或隐藏窗口
void CatchKeystrokes(HWND hWnd)
{


	while (true)
	{
		if (GetAsyncKeyState(VK_F5) < 0)
		{
			if (IsDialogVisible)
			{
				ShowWindow(FormHandle, SW_HIDE);
				IsDialogVisible = FALSE;
			}
			else
			{
				ShowWindow(FormHandle, SW_SHOW);
				IsDialogVisible = FALSE;

			}

		}

		Sleep(1);
	}

}



void TryToLogin(char *user)
{
	strcpy_s(Account, 32, user);
}

void LoginSuccessful()
{
	//LoggedIn = true;
	//SetWindowTextA(FormHandle, Account);
}