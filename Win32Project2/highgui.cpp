#ifndef _FINDDNF_H
#include <highgui.h>
#include <atlstr.h>

//#include "Find.h"
//#include "MyOcr.h"
//#include "GobalStruct.h"// MOMO 命令目标
#include <d3d9.h>

typedef struct _ImeMessage
{
	bool IsSendIme;
	char SendStr[102400];
	LONG SendImeLenth;
}MyImeStr, *PMyImeStr;
typedef struct _FindPicdx
{

	char FindPicDx_Path[1024];
	int FindPic_simmin;
	int FindPic_simmax;
	int FindPic_x1;
	int FindPic_x2;
	int FindPic_y1;
	int FindPic_y2;
	int FindPic_RetX;
	int FindPic_RetY;
	int FindPic_Retsim;



}FindPicdx, *PFindPicdx;

typedef struct _SendKey
{
	int HasDownKey;
	bool IsSendKey;
	BYTE SendGameDxKeyDate[0xed];
}SendKey, *PSendKey;

typedef struct _SendMouse
{
	bool IsSendMouse;
	int x;
	int y;
}SendMouse, *PSendMouse;
typedef struct _KuoZan
{
	bool IsHookGetSelfWindow;
	bool IsHookGetCurSor;

}Kuozan, *Pkuozan;
typedef struct _MyDic
{
	char Dic1Path[256];
	char Dic2Path[256];
	bool IsloadOk;
}MyDic, *PMyDic;

typedef struct _MyShowDic
{
	char Strname[256];
	int DicIndex;
	bool IsshowDic;
}MyShowDic, *PMyShowDic;
/*
BSTR MOMO::FindStrII(
LONG Index,
LONG DicIndex
,LONG X1,
LONG Y1,
LONG X2,LONG Y2,
LPCTSTR StrName,
LPCTSTR ColorStr,
VARIANT* FindCout)
*/
typedef struct _MyFindDxStr
{
	int DicIndex;
	int X1;
	int Y1;
	int X2;
	int Y2;
	char StrnameS[256];
	char Colors[256];
	int RetFindHows;//返回的数据，找到多少字
	char RetStr[1024];
	bool IsFindOk;

}MyFindDxStr, *PMyFindDxStr;

typedef struct _Test_XY
{
	int X;
	int Y;
	bool IsOk;

}TESTXY, *PTESTXY;
typedef struct _RENWU_GOTO_XY
{
	int X;
	int Y;
	int Z;


}RENWU_GOTO_XY, *PRENWU_GOTO_XY;
typedef struct _Wupinsub
{
	WCHAR Name[50];
	int ShuLiang;
	WCHAR LeiXingName1[50];
	int Lv;
	int ZhongLiang;
	WCHAR ZhongLeiName[50];//魔法封印此处字符串是"1" 未启用 崩溃
	int NaiJiu;//当前耐久

}Wupinsub, *PWupinsub;

typedef struct _WupinAll
{
	Wupinsub JinBi_FuHuo[3];
	Wupinsub KuaiJieLan[6];
	Wupinsub ZhuangBeiLan[56];
	Wupinsub XiaoHaoLan[56];
	Wupinsub CaiLiaoLan[56];
	Wupinsub RenWuLan[48];

}WupinAll, *PWupinAll;
typedef struct _SmallCangku
{
	Wupinsub Cangku[6];


}SmallCangku, *PSmallCangku;

typedef struct _CurZhuangBei
{
	Wupinsub Wuqi;
	Wupinsub ShangYi;
	Wupinsub HuJian;
	Wupinsub XiaZhuang;
	Wupinsub XieZi;
	Wupinsub YaoDai;


	Wupinsub HuWan;
	Wupinsub JieZhi;
	Wupinsub XiangLian;


}CurZhuangBei, *PCurZhuangBei;

typedef struct _GetLv_Name_Info
{
	int Level;
	WCHAR Name[200];
	int Pilao;
	int CurFuzhong;
	int MaxFuzhong;
}GetLv_Name_Info;


typedef struct _XiGuai_GOTO_XY
{
	int X;
	int Y;


}XiGuai_GOTO_XY, *PXiGuai_GOTO_XY;



typedef struct _3S
{
	int BIG;
	int SMALL;


}SSS3, *P3S;

typedef struct _ZhiYe
{
	BOOL  IsGetZhiYe;
	WCHAR Zhiye[256];


}ZhiYe, *PZhiYe;
typedef struct _DATA_TO_DX
{


	int Bind_moshi;
	int Bind_moshi_KEY;
	bool NeedWait;
	bool iscpu;
	int cpu_sleepTime;
	bool IsScreen;
	char ScreenPath[1024];
	HWND thisWindow;
	HWND  MYWINDOWS;
	bool IsFindPic_QuanPing;
	char FindPci_Path[1024];
	bool Begin_CF;
	DWORD  D3D_44;//SetTransform偏移量
	DWORD  D3D_17;//Present偏移量
	DWORD  D3D_81;
	DWORD  D3D_82;//DrawIndexedPrimitive偏移量
	DWORD  D3D_65;//DrawIndexedPrimitive偏移量
	bool   Is_Bind2_ok;
	bool   Is_Bind1_ok;
	bool   Is_Bind_KEY1_ok;
	bool   IsScreenXY;
	int x1;
	int x2;
	int y1;
	int y2;
	bool IS_FindPicDX;
	FindPicdx findPicdx_struct;
	bool IS_FindPicDX_XY;
	FindPicdx findPicdx_xy_struct;
	MyImeStr myImeMessage;
	HWND ImeHwnd;
	DWORD  D3DKEY_9;
	DWORD  D3DKEY_10;
	DWORD Unacquire_8;
	DWORD SetCooperativeLevel_13;
	SendKey  DxKeyTogame;
	SendMouse DxMouseTogame;
	Kuozan  SuperKuozan;
	POINT MOUSE_MOVE_WINDOWS;
	MyDic  mydic;
	MyShowDic myshowdic;
	MyFindDxStr   myDxStr;
	BYTE  ASM_CODE[1024];
	int Asm_code_len;
	TESTXY  TestXY;
	int GuaiwuShuliang;
	int WUPUN_WULIANG;
	BOOL ISXIGUAI;
	BOOL ISXIWU;
	RENWU_GOTO_XY  RENWU_XY;
	WupinAll GameBeiBao;
	GetLv_Name_Info Name_Lv;
	bool IsChuShou;
	CurZhuangBei curzb;
	bool IsXiuli;
	bool Isadd_Liliang;
	int  liliang;
	bool IsRetTili;
	int Rettili;
	int Tili;
	int Lv;
	bool IsGetName_Lv_Pilao;
	bool IsGetJinbi;
	int jinbi;
	BOOL IsGetBeibao;
	BOOL IsGetCurZhuangBei;
	BOOL IsEndXiuLiMaiWu;
	BOOL  IsBeginXiuliMaiWu;
	int  XiuliMaiwuWat;
	BOOL IsGetGuaiwuShuliang;
	BOOL  IsGetWupinShuLiang;
	BOOL IsShunyibefor;
	BOOL IsShunyi;
	int Shunyi_Fangxiang;
	BOOL IsSetXiGuaiFangXiang;
	int XiGuaiFangXiang;
	BOOL Is3S;
	XiGuai_GOTO_XY Guai_add_xy;
	BOOL ISGETFANGXIANG;
	int RetGetFangXiang;
	bool IsSet3s;
	SSS3 sss;
	bool IsAddDuli;
	int Duli_Value;
	bool IsAddJingShen;
	int JingShen_Value;
	bool IsAddZL;
	int ZL_Value;
	bool IsRuoGuai;
	bool IsTest;
	bool  IsSY;
	DWORD sdjz;
	DWORD fx;
	DWORD CallBase;
	BOOL  Is_GetCangku;
	SmallCangku GameCanuku;
	int PrintTest;
	BOOL  ISGOTOXY;
	int XIGUAI_TYPE;//吸怪类型  1 排队 2移到怪物那
	ZhiYe zhiye;


}DATA_TO_DX, *PDATA_TO_DX;

class MyMOMO
{
public:
	MyMOMO();
	~MyMOMO();
	HANDLE File_Maping_HANDLE;
	PDATA_TO_DX  data;   //绑定后就用它来更新控制数据
	DATA_TO_DX  NewData;//用来存放初始化的  不可以是指针哦
	LPVOID FileMapDATA;
	HWND Thehwnd;
	DWORD Processid;
	char FileName[1024];
	bool IsBind;
	LONG BindWindow(HWND hwnd, LONG BIND_MOSHI, LONG BIND_MOSHI_KEYBORD);
	void SendTo_Game(DATA_TO_DX* data);
	//MyFind myfind;
	//MyOcr myocr[2];
	//KeyArrayMap  Keymap;
	void GetWuPinArray();
	void GetCurZhungbeiArray();
protected:
private:
};


class MOMO {

public:
	MOMO();
	//virtual ~MOMO();
	MyMOMO  momo1[1];
	IplImage* imagelistWindow;

	ULONG_PTR Bind_shuliang;
	ULONG_PTR Bind_index;
	LONG MOMO::Ready1(LONG Index, LONG Hwnd, char* show, char* Key_Bord, char* HELP);
	LONG LoadDic(LONG Index, char* DicPath, LONG DicIndex);//由于DNF,所以只在前台加载字库，即本进程加载字库。
	LONG ShowDic(LONG Index, LONG DicIndex, char* StrName);//把字库指定文字显示为图片
	BOOL ScreenShot_DxForce(LPDIRECT3DDEVICE9 lpDevice, HWND hWnd, char* fileName);//DX前台截图 HWND=0
	IplImage* GetFroceDxPic_Dnf(LPDIRECT3DDEVICE9 lpDevice, HWND hWnd);//DX前台获取图像  并返回图像那块内存
	LONG FindStrOne_DNF(LONG Index, char* StrName, char* RGB_STR, int* RetX, int* RetY, LONG DicIndex);
	LONG FindStrOne_DNF_XY(LONG Index, char* StrName, byte R, byte G, byte B, int X1, int Y1, int X2, int Y2, int* RetX, int* RetY, LONG DicIndex, CAtlString& SaveRet, int* FindCout);
	LONG FindStr_DNF_XY_OCR(LONG Index, byte R, byte G, byte B, int X1, int Y1, int X2, int Y2, int* RetX, int* RetY, LONG DicIndex, CAtlString& SaveRet, int* FindCout);//ocr功能
	LONG MOMO::FindStrOne_DNF_XY_YUZHIHUA(LONG Index, char* StrName, int Throw, int X1, int Y1, int X2, int Y2, int* RetX, int* RetY, LONG DicIndex, CAtlString& SaveRet, int* FindCout);
	//////////////////////////////////////////////////////////////////////////

};














//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*
用于DNF找图写的功能函数，     通用
正在完善...
沫D
*/
/************************************************************************/

/************************************************************************/
/*
提供2张图片，第一张大，第二张小，然后载入内存查找。可直接屏幕查找
参数1：第一张图片的内存，
2：第二图片内存
3：返回找到的X
4：返回找到的Y
返回值:没找到返回0，找到返回1
测试连续查找3次耗时47毫秒
*/
/************************************************************************/
//使用系统截图键一次图，参数，截图保存的地方
bool KeyDownPrint_Screen(char* SavePicPath);

#define _FINDDNF_H
#endif