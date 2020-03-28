#include "stdafx.h"
#include "FindDnf.h"
#include <highgui.h>
//#include "Find.h"
//#include "MyMouseKey.h"
//#include "ImeInject.h"
#include <fstream>
#include <d3d9.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800 //本来800  改700
#include <dinput.h>
//#include "OlsApi.h"
//#include "Cdmsoft1.h"
//#include "NTFUNCTION.h"
MyMOMO::~MyMOMO()
{
	if (this->data != NULL)
	{
		OutputDebugStringA("un reset");
		UnmapViewOfFile(this->data);
	}

}
MyMOMO::MyMOMO()
{
	this->data = NULL;
	NewData.Is_Bind2_ok = FALSE;
	NewData.Is_Bind1_ok = FALSE;
	NewData.Is_Bind_KEY1_ok = FALSE;
	NewData.NeedWait = false;
	NewData.iscpu = FALSE;
	this->IsBind = FALSE;
	this->NewData.cpu_sleepTime = 0;
	this->NewData.IsScreen = FALSE;
	this->NewData.IsScreenXY = FALSE;
	this->NewData.DxKeyTogame.IsSendKey = FALSE;

	memset(&NewData.DxKeyTogame, 0, sizeof(SendKey));
	memset(&NewData.SuperKuozan, 0, sizeof(Kuozan));
	memset(&NewData.DxMouseTogame, 0, sizeof(SendMouse));
	memset(NewData.ScreenPath, 0, 1024);
	memset(NewData.FindPci_Path, 0, 1024);
	memset(NewData.findPicdx_struct.FindPicDx_Path, 0, 1024);
	memset(NewData.findPicdx_xy_struct.FindPicDx_Path, 0, 1024);
	memset(NewData.myImeMessage.SendStr, 0, 102400);
	NewData.myImeMessage.IsSendIme = FALSE;
	NewData.myImeMessage.SendImeLenth = 0;
	this->NewData.IsFindPic_QuanPing = FALSE;
	this->NewData.Begin_CF = FALSE;
	this->NewData.IS_FindPicDX = FALSE;

	/*
	this->NewData.IS_FindPicDX_XY=FALSE;
	this->NewData.findPicdx_struct.FindPic_Retsim=-1;
	this->NewData.findPicdx_struct.FindPic_x1=-1;
	this->NewData.findPicdx_xy_struct.FindPic_simmax=-1;
	*/

}


void MyMOMO::SendTo_Game(DATA_TO_DX* data)
{

	/*  memcpy(this->FileMapDATA,data,sizeof(DATA_TO_DX));*/
	FlushViewOfFile(this->FileMapDATA, sizeof(DATA_TO_DX));

}

LPVOID GetClassVirtualFnAddress(LPVOID pthis, int Index) //Add 2010.8.6
{
	LPVOID FnAddress;

	*(int*)&FnAddress = *(int*)pthis;                       //lpvtable
	*(int*)&FnAddress = *(int*)((int*)FnAddress + Index);
	return FnAddress;
}
LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
HRESULT InitD3D1(HWND hWnd)
{
	// Create the D3D object.
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create the D3DDevice
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		AfxMessageBox("CreateDevice erro");
		return E_FAIL;
	}

	return S_OK;
}



LPDIRECTINPUT8         lpDirectInput;  // DirectInput object
LPDIRECTINPUTDEVICE8   lpKeyboard = NULL;     // DirectInput device





BOOL InitDInput(HWND hWnd)
{
	HRESULT hr;


	// 创建一个 DIRECTINPUT 对象
	hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&lpDirectInput, NULL);
	//hr = DirectInputCreateA(GetModuleHandle(NULL), DIRECTINPUT_VERSION, &lpDirectInput, NULL);
	if FAILED(hr)
	{
		OutputDebugStringA("\r\n\r\n初始化1失败");
		// 失败
		return FALSE;
	}

	// 创建一个 DIRECTINPUTDEVICE 界面
	hr = lpDirectInput->CreateDevice(GUID_SysKeyboard, &lpKeyboard, NULL);
	if FAILED(hr)
	{
		// 失败
		OutputDebugStringA("\r\n\r\n初始化2失败");
		return FALSE;
	}

	// 设定为通过一个 256 字节的数组返回查询状态值
	hr = lpKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if FAILED(hr)
	{
		// 失败
		OutputDebugStringA("\r\n\r\n初始化3失败");
		return FALSE;
	}

	// 设定协作模式
	hr = lpKeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);//DISCL_BACKGROUND后台
	if FAILED(hr)
	{
		// 失败
		OutputDebugStringA("\r\n\r\n初始化4失败");
		return FALSE;
	}

	// 设定缓冲区大小
	// 如果不设定，缓冲区大小默认值为 0，程序就只能按立即模式工作
	// 如果要用缓冲模式工作，必须使缓冲区大小超过 0
	DIPROPDWORD     property;

	property.diph.dwSize = sizeof(DIPROPDWORD);
	property.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	property.diph.dwObj = 0;
	property.diph.dwHow = DIPH_DEVICE;
	property.dwData = 16;

	hr = lpKeyboard->SetProperty(DIPROP_BUFFERSIZE, &property.diph);

	if FAILED(hr)
	{
		OutputDebugStringA("\r\n\r\n初始化5失败");
		// 失败
		return FALSE;
	}


	hr = lpKeyboard->Acquire();
	if FAILED(hr)
	{
		// 失败
		OutputDebugStringA("\r\n\r\n初始化6失败");
		return FALSE;
	}

	return TRUE;
}

LONG MyMOMO::BindWindow(HWND hwnd, LONG  BIND_MOSHI, LONG BIND_MOSHI_KEYBORD)
{
	//AfxMessageBox("0");
	if (g_pd3dDevice == NULL)
	{
		InitD3D1(::GetDesktopWindow());
	}




	if (lpKeyboard == NULL)
	{
		InitDInput(::GetDesktopWindow());
	}
	//ImeInstallEx(NULL);


	//AfxMessageBox("5"); 
	this->NewData.Bind_moshi = BIND_MOSHI;
	this->NewData.Bind_moshi_KEY = BIND_MOSHI_KEYBORD;

	// TODO: 在此添加控件通知处理程序代码


	//AfxMessageBox("1");


	if (hwnd == NULL)
	{
		//  AfxMessageBox("窗口不存在！");
		return 0;

	}



	DWORD  d3d9_adr = (DWORD)GetModuleHandleA("d3d9.dll");
	DWORD  d3d9KEYBORD_adr = (DWORD)GetModuleHandleA("DINPUT8.dll");
	/*
	if (g_pd3dDevice==NULL)
	{InitD3D1(::GetDesktopWindow());}




	if (lpKeyboard==NULL)
	{
	InitDInput(::GetDesktopWindow());
	}*/
	//g_pd3dDevice->SetTransform() //44
	this->NewData.D3D_44 = (DWORD)GetClassVirtualFnAddress(g_pd3dDevice, 44) - d3d9_adr;
	this->NewData.D3D_65 = (DWORD)GetClassVirtualFnAddress(g_pd3dDevice, 65) - d3d9_adr;
	this->NewData.D3D_81 = (DWORD)GetClassVirtualFnAddress(g_pd3dDevice, 81) - d3d9_adr;
	this->NewData.D3D_82 = (DWORD)GetClassVirtualFnAddress(g_pd3dDevice, 82) - d3d9_adr;
	this->NewData.D3D_17 = (DWORD)GetClassVirtualFnAddress(g_pd3dDevice, 17) - d3d9_adr;
	this->NewData.D3DKEY_9 = (DWORD)GetClassVirtualFnAddress(lpKeyboard, 9) - d3d9KEYBORD_adr;
	this->NewData.D3DKEY_10 = (DWORD)GetClassVirtualFnAddress(lpKeyboard, 10) - d3d9KEYBORD_adr;
	this->NewData.Unacquire_8 = (DWORD)GetClassVirtualFnAddress(lpKeyboard, 8) - d3d9KEYBORD_adr;
	this->NewData.SetCooperativeLevel_13 = (DWORD)GetClassVirtualFnAddress(lpKeyboard, 13) - d3d9KEYBORD_adr;
	char DEBUGOUT[256] = { 0 };
	sprintf(DEBUGOUT, "9号STATE地址偏移：%X , 10号DATA地址偏移:%X，input模块地址:%X", this->NewData.D3DKEY_9, this->NewData.D3DKEY_10, d3d9KEYBORD_adr);

	Thehwnd = hwnd;
	GetWindowThreadProcessId(hwnd, &Processid);
	char Des[1024] = "C:\\";
	char pid_str[1024] = { 0 };
	itoa(Processid, pid_str, 10);
	strcat(Des, pid_str);
	strcat(Des, ".YXP");
	memset(FileName, 0, 1024);
	memcpy(FileName, Des, 1024);
	using namespace std;
	ofstream fin(Des, ios::binary);



	HANDLE FileHandle;
	ULONG_PTR FILESIZE;
	BYTE* BUFER;
	DWORD TEMP;
	FileHandle = ::CreateFileA(Des, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_SYSTEM, NULL);
	if (FileHandle == (HANDLE)0 || FileHandle == (HANDLE)-1)
	{

		//  AfxMessageBox("句柄不存在1");
		return 0;
	}

	//::MessageBoxA(0,Des,Des,0);
	this->File_Maping_HANDLE = ::CreateFileMappingA(FileHandle, NULL, PAGE_READWRITE, 0, 0x800000, pid_str);//0x4000000 16M

	while (this->File_Maping_HANDLE == (HANDLE)0 || this->File_Maping_HANDLE == (HANDLE)-1)
	{

		this->File_Maping_HANDLE = ::CreateFileMappingA(FileHandle, NULL, PAGE_READWRITE, 0, 0x800000, pid_str);//0x4000000 16M
	}
	if (this->File_Maping_HANDLE == (HANDLE)0 || this->File_Maping_HANDLE == (HANDLE)-1)
	{
		DWORD dw = GetLastError();
		char errostr[256] = { 0 };
		sprintf(errostr, "%d", dw);
		AfxMessageBox(errostr);

		CloseHandle(FileHandle);
		//  AfxMessageBox("句柄不存在2");
		return 0;
	}

	this->NewData.thisWindow = hwnd;
	this->NewData.Begin_CF = TRUE;
	//Sleep(5000);
	DWORD offset = 0x4000000;//64的倍数
	FileMapDATA = MapViewOfFile(this->File_Maping_HANDLE, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	while (FileMapDATA == NULL)
	{
		FileMapDATA = MapViewOfFile(this->File_Maping_HANDLE, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	}
	if (FileMapDATA == NULL)
	{
		AfxMessageBox("erro  map");
	}
	memcpy(FileMapDATA, &this->NewData, sizeof(DATA_TO_DX));
	FlushViewOfFile(FileMapDATA, sizeof(DATA_TO_DX));
	data = (PDATA_TO_DX)FileMapDATA;

	// Sleep(1300);
	CloseHandle(FileHandle);
	/*
	if (InputHandle != NULL)
	{
	//激活指定窗口输入法
	::PostMessage(hWnd,WM_INPUTLANGCHANGEREQUEST,0x1,(LPARAM)InputHandle);
	HWND mhWnd = NULL;

	do
	{
	mhWnd = ::FindWindowExA(hWnd,mhWnd,NULL,NULL);

	if (mhWnd != NULL)
	{
	::PostMessage(mhWnd,WM_INPUTLANGCHANGEREQUEST,0x1,(LPARAM)InputHandle);
	}
	} while (mhWnd != NULL);

	//::PostMessage(hWnd,WM_INPUTLANGCHANGE,0x1,(LPARAM)InputHandle);
	}
	else
	{
	//句柄不存在，枚举输入法句柄，激活
	HKL imehandle = EnumIme("沫、D输入法1.123","C:\\WINDOWS\\SYSTEM32\\1.ime");
	if (imehandle != NULL)
	{
	::PostMessage(hWnd,WM_INPUTLANGCHANGEREQUEST,0x1,(LPARAM)imehandle);
	HWND mhWnd = NULL;

	do
	{
	mhWnd = ::FindWindowExA(hWnd,mhWnd,NULL,NULL);

	if (mhWnd != NULL && mhWnd!=0)
	{
	::PostMessage(mhWnd,WM_INPUTLANGCHANGEREQUEST,0x1,(LPARAM)imehandle);
	}
	} while (mhWnd != NULL);

	//::PostMessage(hWnd,WM_INPUTLANGCHANGE,0x1,(LPARAM)InputHandle);
	}
	else
	{
	return FALSE;
	}
	}*/

	//IMEActivEx(hwnd,this->data);

	//Sleep(100);
	if (this->data->Bind_moshi == 1)
	{

		ULONG_PTR coutBind = 0;
		//::SendMessageA(hwnd,WM_USER+2345,0,0);
		while (this->data->Is_Bind1_ok == FALSE)
		{

			Sleep(100);
			//  ::SendMessageA(hwnd,WM_USER+2345,0,0);

			if (coutBind>50)
			{

				//  AfxMessageBox("绑定失败！");

				return 0;
				break;
			}
			coutBind++;
		}
		this->IsBind = TRUE;
		//AfxMessageBox("绑定模式1");
	}
	if (this->data->Bind_moshi == 0)
	{
		this->IsBind = TRUE;
		//AfxMessageBox("绑定模式1");
	}
	if (this->data->Bind_moshi == 2)
	{
		ULONG_PTR coutBind = 0;

		while (this->data->Is_Bind2_ok == FALSE)
		{
			//::SendMessageA((HWND)hwnd,WM_USER+2345,0,0);
			Sleep(100);
			if (coutBind>250)//等待15秒绑定就认为超时 刚2秒 游戏还在初始化呢
			{


				//  AfxMessageBox("绑定失败2！");

				return 0;
				break;
			}
			coutBind++;
		}
		this->IsBind = TRUE;
	}


	if (this->data->Bind_moshi_KEY == 1)
	{
		ULONG_PTR coutBind = 0;
		while (this->data->Is_Bind_KEY1_ok == FALSE)
		{

			Sleep(100);
			if (coutBind>50)
			{

				//  AfxMessageBox("绑定键盘1失败！");

				return 0;
				break;
			}
			coutBind++;
		}

	}

	//Sleep(1000);

	return 1;
	//  momo1.data->Begin_CF=TRUE;

}


MOMO::MOMO()
{
	/*
	Bind_index=All_kehuduan;
	All_kehuduan++;
	tiquan(SE_DEBUG_NAME);
	if (IsFirst)
	{
	//FreeResFile(IDR_IME1,"IME","C:\\WINDOWS\\SYSTEM32\\1.IME");
	IsFirst=FALSE;
	Sleep(1000);
	}
	AfxOleLockApp();*/
}

LONG MOMO::Ready1(LONG Index, LONG Hwnd, char* show, char* Key_Bord, char* HELP)
{


	//momo1[Index]=new MyMOMO;
	using namespace std;
	string helpStr = HELP;
	//  AfxMessageBox("ok");
	if (helpStr.find("防键鼠后台检测") != helpStr.npos)
	{
		//AfxMessageBox("防键鼠后台检测");
		momo1[Index].NewData.SuperKuozan.IsHookGetSelfWindow = TRUE;

	}
	else {
		momo1[Index].NewData.SuperKuozan.IsHookGetSelfWindow = FALSE;

	}

	if (helpStr.find("鼠标位置拦截") != helpStr.npos)
	{
		//AfxMessageBox("鼠标位置拦截");
		momo1[Index].NewData.SuperKuozan.IsHookGetCurSor = TRUE;

	}
	else {
		momo1[Index].NewData.SuperKuozan.IsHookGetCurSor = FALSE;

	}

	CAtlString strShow;
	strShow = show;

	LONG BindShow = -1;
	LONG BindKey = -1;
	if (strShow.Find("普通图色") != -1)
	{
		//AfxMessageBox("普通");
		BindShow = 0;
	}


	if (strShow.Find("GDI后台图色") != -1)
	{
		//AfxMessageBox("GDI后台");
		BindShow = 1;
	}



	if (strShow.Find("DX后台图色") != -1)
	{

		//AfxMessageBox("DX后台");
		BindShow = 2;
	}


	//////////////////////////////////////////////////////////////////////////
	strShow.Empty();
	strShow = Key_Bord;
	if (strShow.Find("DX1键盘") != -1)
	{
		//AfxMessageBox("DX1");
		BindKey = 1;
	}
	if (strShow.Find("普通后台键盘") != -1)
	{
		//AfxMessageBox("普通后台键盘");
		BindKey = 0;
	}

	if (momo1[Index].BindWindow((HWND)Hwnd, BindShow, BindKey) == 0)
	{
		return 0;
	}

	return 1;
}


LONG MOMO::LoadDic(LONG Index, char* DicPath, LONG DicIndex)
{

	if (strlen(DicPath)<1)
	{
		return -1;
	}
	if (DicIndex>1 || DicIndex<0)
	{
		return -2;
	}

	return 0;
}





LONG MOMO::ShowDic(LONG Index, LONG DicIndex, char* StrName)//把字库指定文字显示为图片
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (DicIndex>1 || DicIndex <0)
	{
		AfxMessageBox("仅支持2个字库 0号以及1号");
		return 0;
	}


	return 1;
}


BOOL MOMO::ScreenShot_DxForce(LPDIRECT3DDEVICE9 lpDevice, HWND hWnd, char* fileName)
{
	HRESULT hr;



	return hr;
}

IplImage* MOMO::GetFroceDxPic_Dnf(LPDIRECT3DDEVICE9 lpDevice, HWND hWnd)
{


	return NULL;

}



bool GetDnf_RECT(char* WindowName, LPRECT RetXY)
{
	::GetWindowRect(::FindWindowA(NULL, WindowName), RetXY);
	return TRUE;

}

void MyMOMO::GetWuPinArray()
{
	data->IsGetBeibao = TRUE;
	while (data->IsGetBeibao == TRUE)
	{
		OutputDebugStringA("\r\nGetWuPinArray");

		Sleep(100);
	}

}


void MyMOMO::GetCurZhungbeiArray()
{
	::SendMessageA(Thehwnd, WM_USER + 75741, 1, 1);
}