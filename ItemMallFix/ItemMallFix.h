// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 ITEMMALLFIX_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// ITEMMALLFIX_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef ITEMMALLFIX_EXPORTS
#define ITEMMALLFIX_API extern "C" __declspec(dllexport)
#else
#define ITEMMALLFIX_API __declspec(dllimport)
#endif



DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter);
DWORD WINAPI ThreadProc2(_In_  LPVOID lpParameter);

ITEMMALLFIX_API int Start(void);

typedef struct _ITEMMALL
{
	char* szName;
	char* code;
	UINT buycost;
	UINT id1;
	UINT id2;
	UINT id3;
	UINT id4;
	UINT id5;
	UINT id6;
	UINT id7;
	UINT id8;
	UINT id9;
	UINT id10;
	UINT id11;
	UINT id12;
	UINT id13;
	UINT id14;
	UINT id15;
	UINT id16;
	UINT id17;
	UINT id18;
	UINT id19;
	UINT id20;
	UINT id21;
	UINT id22;
	UINT id23;
	UINT id24;
	BYTE count1;
	BYTE count2;
	BYTE count3;
	BYTE count4;
	BYTE count5;
	BYTE count6;
	BYTE count7;
	BYTE count8;
	BYTE count9;
	BYTE count10;
	BYTE count11;
	BYTE count12;
	BYTE count13;
	BYTE count14;
	BYTE count15;
	BYTE count16;
	BYTE count17;
	BYTE count18;
	BYTE count19;
	BYTE count20;
	BYTE count21;
	BYTE count22;
	BYTE count23;
	BYTE count24;
}ITEMMALL, *PITEMMALL;


