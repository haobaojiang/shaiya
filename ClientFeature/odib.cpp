
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"
#include "odib.h"
#include <vector>
#include <process.h>

#pragma comment(lib,"User32.lib")


namespace SHAIYAOWNERDRAW
{
	using std::vector;

	HWND g_hParentWnd = NULL;
	HWND g_hwnd = NULL;
	HINSTANCE hInst;
	ATOM				MyRegisterClass(HINSTANCE hInstance);
	BOOL				InitInstance(HINSTANCE, int);
	LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
	typedef void (WINAPI *PSWITCHTOTHISWINDOW) (HWND, BOOL);
	PSWITCHTOTHISWINDOW SwitchToThisWindow = NULL;

	vector<OWNDRAWBUTTON> g_OwnDrawButton;
	fun_IconCallBack g_pCallBack = NULL;

	////////////////////////这是一些子功能
	void SetCallBack(fun_IconCallBack pfun)
	{
		g_pCallBack = pfun;
	}

	void ShowWindow()
	{
		if (g_hwnd) {
			ShowWindow(g_hwnd, SW_SHOW);
			UpdateWindow(g_hwnd);

		}

	}

	void HideWindow()
	{
		if (g_hwnd) {
			ShowWindow(g_hwnd, SW_HIDE);
			UpdateWindow(g_hwnd);

		}
	}



	void AddEmoji(char* strFileName, DWORD resId) {
		OWNDRAWBUTTON stcButton;
		ZeroMemory(&stcButton, sizeof(stcButton));
		strcpy(stcButton.strFileName, strFileName);
		stcButton.resId = resId;
		g_OwnDrawButton.push_back(stcButton);
	}
   ///////////////////////////////////////////////////////////////

	
	//外面给的子入口
	void WindowThread(void* p)
	{
		MSG msg;
		HACCEL hAccelTable;
		WCHAR s[BUFFSIZE];
		HINSTANCE hInstance = (HINSTANCE)p;
		MyRegisterClass(hInstance);

		if (!InitInstance(hInstance, SW_HIDE)) {
		}

		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(ID_ACCEL));
		if (NULL == hAccelTable) {
			swprintf(s, L"! Accelerators failed to load (err %ld) (WinMain)", GetLastError());
			OutputDebugString(s);
		}

		//尝试找一下这个API
		HMODULE hUser32 = GetModuleHandle(L"user32");
		SwitchToThisWindow = (PSWITCHTOTHISWINDOW)GetProcAddress(hUser32, "SwitchToThisWindow");
	



		while (GetMessage(&msg, NULL, 0, 0)) {
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}	
	}


	int InitWindow(HINSTANCE hInstance,HWND hParent)
	{
		g_hParentWnd = hParent;
		_beginthread(WindowThread, 0, hInstance);
		return 0;
	}




	ATOM MyRegisterClass(HINSTANCE hInstance) {
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = (WNDPROC)WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = 0;
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = sAppName;
		wcex.hIconSm = NULL;

		return RegisterClassEx(&wcex);
	}


	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
		hInst = hInstance;

		g_hwnd = CreateWindow(
			sAppName,
			sAppName,
			WS_OVERLAPPED,
			300, 300, LINE_COUNT*ICON_WIDTH+5, 
			450,   //height
			0     //desktop
			/*g_hParentWnd*/,
			NULL,
			hInstance,
			NULL);
		if (!g_hwnd) {
			OutputDebugString(L"! Main window creation failed (InitInstance)");
			return FALSE;
		}

		ShowWindow(g_hwnd, nCmdShow);
		UpdateWindow(g_hwnd);

		return TRUE;
	}

	void InitOwnerDrawButtonWindow(HWND hParent, HINSTANCE hInstance)
	{
		for (DWORD i = 0; i < g_OwnDrawButton.size(); i++)
		{
			DWORD resId = g_OwnDrawButton[i].resId;
			g_OwnDrawButton[i].hButton = CreateWindow(L"button", NULL,
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				0, 0, 0, 0,
				hParent,
				(HMENU)resId,
				hInstance,
				NULL);

			g_OwnDrawButton[i].hIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(resId));
		}
	}


	void DrawIconButton(DRAWITEMSTRUCT* pdis, HINSTANCE hInst)
	{
		for (DWORD i = 0; i < g_OwnDrawButton.size(); i++) {
			if (pdis->CtlID == g_OwnDrawButton[i].resId) {

				RECT rect = pdis->rcItem;

				DrawIconEx(
					pdis->hDC,
					(int) 0.5 * (rect.right - rect.left - ICON_WIDTH),
					(int) 0.5 * (rect.bottom - rect.top - ICON_HEIGHT),
					g_OwnDrawButton[i].hIcon,
					ICON_WIDTH,
					ICON_HEIGHT,
					0, NULL, DI_NORMAL);

				break;
			}
		}
	}


	void MyOwnerDrawMoveWindow(WPARAM wParam)
	{
		if (wParam == SIZE_MINIMIZED)
			return;

		for (DWORD i = 0, x = 0,y=0; i < g_OwnDrawButton.size(); i++)
		{
			MoveWindow(g_OwnDrawButton[i].hButton,
				x,
				y,
				ICON_WIDTH,  // Width
				ICON_HEIGHT,  // Height
				TRUE);  // Repaint (bool)

			//换行了则x从0开始
			if (i / LINE_COUNT &&i % LINE_COUNT ==0) {
				x = 0;
				y += ICON_HEIGHT;
			}
			else {
				x += ICON_WIDTH;
			}
		}
	}

	void MyOwnerDrawProcessClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{

		DWORD resId = LOWORD(wParam);


		for (DWORD i = 0; i < g_OwnDrawButton.size(); i++)
		{
			if (g_OwnDrawButton[i].resId == resId) {
				return  g_pCallBack(g_OwnDrawButton[i].strFileName);
			}
		}

		 DefWindowProc(hWnd, message, wParam, lParam);
	}

	void MyOwnerDrawDestoryWindow()
	{
		for (DWORD i = 0; i < g_OwnDrawButton.size(); i++)
		{
			DestroyWindow(g_OwnDrawButton[i].hButton);
		}
	}


	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		PAINTSTRUCT ps;
		HDC hdc;
		static HINSTANCE hInst;


		static DRAWITEMSTRUCT* pdis;
		static long cxClient, cyClient;

	

		switch (message)
		{
		case WM_DRAWITEM:
			pdis = (DRAWITEMSTRUCT*)lParam;
			DrawIconButton(pdis, hInst);
			return(TRUE);


		case WM_CREATE:
			hInst = ((LPCREATESTRUCT)lParam)->hInstance;
			InitOwnerDrawButtonWindow(hWnd, hInst);
			break;


		case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			MyOwnerDrawMoveWindow(wParam);
			break;


		case WM_COMMAND:
			MyOwnerDrawProcessClick(hWnd, message, wParam, lParam);
			break;


		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;



		case WM_CLOSE:
			MyOwnerDrawDestoryWindow();
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

}