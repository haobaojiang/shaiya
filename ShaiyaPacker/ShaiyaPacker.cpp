// ShaiyaPacker.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <string>
#include "FileReadWrite.h"

#pragma comment(lib,"user32.lib")

void EncryptSah(DWORD dwNumber);
DWORD DecryptSah(DWORD dwNumber);
using std::string;


DWORD g_Temp = 0;
int m_x = 0;
int m_y = 0;
int filecountnow = 0;
string ziyuanpath = "";
PBYTE sah;
int start = 56;
int filecount = 0;
string* filename;

PDWORD filehoffset;
PDWORD filelen;
PDWORD fileoffset;
int filecount_temp = 0;

HANDLE g_hFile = nullptr;
DWORD  g_dwSize = 0;


//设置工具类型
#define _Encrypt 1  //加密
#define _Decrypt 2  //解密
#define _PrintSah 3  //只打印
BYTE  g_byToolType = _PrintSah;

//只改第一个字节
DWORD  g_strHashTable[] = { 0x18, 0x68, 0x61,0x74};


void getfile();
void menuItem2_Click();
void getdir(int dir_count);

//假如有一个当前目录
//遍历目录的文件--读取当前目录的文件夹数量--读取首个文件夹名--遍历该文件夹

//就是这样一个循环

void menuItem2_Click()
{
	start = 56;
	filecount = 0;
	filecountnow = 0;

	//1. 获取文件名
#ifndef _DEBUG
	printf("please type the file name:update.sah or data.sah or others\n");
	char filePatch[MAX_PATH] = { 0 };
	scanf_s("%s", filePatch, MAX_PATH);
#else
	char* filePatch = "E:\\shaiya-tmp\\sah.bak is not crypted\\New folder\\data.sah";
#endif

	//2. 创建文件备份
	char strBackFile[MAX_PATH] = { 0 };
	sprintf_s(strBackFile, MAX_PATH, "%s.bak", filePatch);
	CopyFileA(filePatch, strBackFile, 0);

	//3. 打开文件
	g_hFile = CreateFileA(filePatch,
		GENERIC_ALL,
		FALSE,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (g_hFile == INVALID_HANDLE_VALUE)
	{
		DeleteFileA(strBackFile);
		MessageBox(0, L"file not exists", 0, 0);
		return;
	}

	//4. 获取文件方式
	g_dwSize = GetFileSize(g_hFile, 0);         //获取文件大小
	sah = new BYTE[g_dwSize];                   //申请内存空间大小
	DWORD dwRet;
	ReadFile(g_hFile, sah, g_dwSize, &dwRet, 0);//读进内存

	//5. 读取文件头确定文件是加密还是解密状态
	char strSignature[MAX_PATH] = { 0 };
	if (strcmp((PCHAR)sah, "SAH") == 0)
	{
		printf("it looks like a orginal file,wants to Encrypt it?\t(Y/N)\n");
		g_byToolType = _Encrypt;
		strcpy_s(strSignature, MAX_PATH, "fff");
	}
	else if (strcmp((PCHAR)sah, "fff") == 0)
	{
		printf("it looks like a Encryption file,wants to decrypt it?\t(Y/N)\n");
		g_byToolType = _Decrypt;
		strcpy_s(strSignature, MAX_PATH, "SAH");
	}
	else
	{
		MessageBox(0, L"Unknow File format!",0,0);
		return;
	}

	//6. 让用户选择是否需要继续加解密
	while (1)
	{
		char Choose;
		scanf_s("%c", &Choose,1);
		if (Choose == 'y')
		{
			break;
		}
		else if (Choose == 'n')
		{
			CloseHandle(g_hFile);
			g_hFile = NULL;
			return;
		}
		else
		{
			continue;
		}
	}

	//7. 改写文件头部写入加解密标识
	DWORD dwWriteen;
	SetFilePointer(g_hFile, 0, 0, FILE_BEGIN);
	WriteFile(g_hFile, strSignature, strlen(strSignature), &dwWriteen, 0);

	/********************************************************************************/

	//获取文件数量
	filecount = + sah[7];

	//根据文件数量申请变量空间



	getfile();

	//记录文件夹数量,这个可能是第一个的文件夹数量
	DWORD dircount = *(DWORD*)&sah[start];
	start += 4;
	getdir(dircount);


	delete[] sah;
}

void getfile()
{
	g_Temp++;

	//查询文件夹里文件的数量
	DWORD dirfilecount = *(DWORD*)&sah[start];

	if (g_byToolType == _Decrypt)
	{
		//如果是加密的就需要解密
		dirfilecount = DecryptSah(dirfilecount);
	}

	//如果要求加密就加密一下
	if (g_byToolType == _Encrypt)
	{
		EncryptSah(dirfilecount);
	}

	if (g_byToolType == _PrintSah)
	{
		printf("FileCount:%u\n", dirfilecount);
		Sleep(1000);
	}



	start += 4;//这时是60，定位到文件名长度
	if (dirfilecount == 0)
	{
		return;
	}

	//遍历文件夹里的文件
	for (int i = 0; i < dirfilecount; i++)
	{
		//文件名
		DWORD filenamelen = *(DWORD*)&sah[start] - 1;
		start += 4;                          //start这时是64,定位到文件名
		char strName[200] = {0};

		//....取文件名
/*		memcpy_s(strName, 200, &sah[start], filenamelen);*/


		start += filenamelen + 1;          //64+文件名长度+1=76
	
/*		filehoffset[filecountnow] = start; //这个应该没有什么用*/

		//......取偏移地址
// 		fileoffset[filecountnow] = 
// 			  (DWORD)sah[start + 4] * 256 * 256 * 256 * 256 
// 			+ (DWORD)sah[start + 3] * 256 * 256 * 256
// 			+ (DWORD)sah[start + 2] * 256 * 256
// 			+ (DWORD)sah[start + 1] * 256
// 			+ (DWORD)sah[start];

		start += 8;    //83

		//.......取大小
/*		filelen[filecountnow] = (*(PDWORD)&sah[start]); //长度是那4个字节再减去10,也有可能是8个字节*/

		start += 8;    //91 这时定位到该目录里的文件夹数量
		filecountnow++;
	}
}

bool aaa = false;
void getdir(int dir_count)
{
	for (int i = 0; i < dir_count; i++)
	{
	errnext:
		//获取文件夹名的长度
	int dirnamelen_temp = 
		sah[start + 3] * 256 * 256 * 256 
		+ sah[start + 2] * 256 * 256 
		+ sah[start + 1] * 256 
		+ sah[start] - 1;
		start += 4;

		if (dirnamelen_temp == 0)
			goto errnext;



		for (int j = 0; j < dirnamelen_temp; j++)
		{
			if (j == 0)
			{
				if (sah[start] < 32)         //好像是不能读取中文名
					goto errnext;
			}

		}
		start += dirnamelen_temp + 1;

		//又回到之前的文件夹数量去
		int dircount_temp = sah[start - dirnamelen_temp - 1 - 8];

		//又取这个文件夹里面的文件
		getfile();

		int nextdircount = sah[start + 3] * 256 * 256 * 256 + sah[start + 2] * 256 * 256 + sah[start + 1] * 256 + sah[start];
		if (nextdircount != 0)
			getdir(nextdircount);

		if (i != dir_count - 1)
			start += 4;
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	menuItem2_Click();
	if (start > 56)
	{
		printf("done");
	}
	system("pause");
	return 0;
}

//加密函数
void EncryptSah(DWORD dwNumber)
{
	dwNumber ^= g_strHashTable[0];
	dwNumber ^= g_strHashTable[1];
	dwNumber ^= g_strHashTable[2];
	dwNumber ^= g_strHashTable[3];

	SetFilePointer(g_hFile, start, 0, FILE_BEGIN);
	DWORD dwWriteen;
	WriteFile(g_hFile, &dwNumber, 4, &dwWriteen, 0);
}

//解密函数
DWORD DecryptSah(DWORD dwNumber)
{
	dwNumber ^= g_strHashTable[3];
	dwNumber ^= g_strHashTable[2];
	dwNumber ^= g_strHashTable[1];
	dwNumber ^= g_strHashTable[0];

	if (g_byToolType == _Decrypt)
	{
		SetFilePointer(g_hFile, start, 0, FILE_BEGIN);
		DWORD dwWriteen;
		WriteFile(g_hFile, &dwNumber, 4, &dwWriteen, 0);
	}
	return dwNumber;
}