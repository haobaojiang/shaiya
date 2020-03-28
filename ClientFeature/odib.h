#ifndef __ODIB_H__
#define __ODIB_H__

namespace SHAIYAOWNERDRAW
{
// #define _UNICODE
// #define UNICODE

	// Application name and name of log file:
	static TCHAR sAppName[] = L"odib";


	// Return values:
#define RET_OK 1
#define RET_ERR_CREATE -1
#define RET_ERR_LOAD_ICON -2

#define BUFFSIZE 2048


	// Width and height values:
#define MARGIN 2
#define ICON_WIDTH 32
#define ICON_HEIGHT 32
#define LINE_COUNT (6)

typedef void(*fun_IconCallBack)(char*);


#pragma  pack(1)
	typedef struct {
		DWORD resId;
		HWND hButton;
		HICON hIcon;
		char strFileName[MAX_PATH];
	}OWNDRAWBUTTON, *POWNDRAWBUTTON;
#pragma pack()

//导出的函数
void AddEmoji(char* strFileName,DWORD resId);
int InitWindow(HINSTANCE hInstance, HWND hParent);
void SetCallBack(fun_IconCallBack pfun);
void ShowWindow();
void HideWindow();


}





#endif  /* __ODIB_H__ */

