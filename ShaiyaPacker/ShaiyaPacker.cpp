// ShaiyaPacker.cpp : �������̨Ӧ�ó������ڵ㡣
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


//���ù�������
#define _Encrypt 1  //����
#define _Decrypt 2  //����
#define _PrintSah 3  //ֻ��ӡ
BYTE  g_byToolType = _PrintSah;

//ֻ�ĵ�һ���ֽ�
DWORD  g_strHashTable[] = { 0x18, 0x68, 0x61,0x74};


void getfile();
void menuItem2_Click();
void getdir(int dir_count);

//������һ����ǰĿ¼
//����Ŀ¼���ļ�--��ȡ��ǰĿ¼���ļ�������--��ȡ�׸��ļ�����--�������ļ���

//��������һ��ѭ��

void menuItem2_Click()
{
	start = 56;
	filecount = 0;
	filecountnow = 0;

	//1. ��ȡ�ļ���
#ifndef _DEBUG
	printf("please type the file name:update.sah or data.sah or others\n");
	char filePatch[MAX_PATH] = { 0 };
	scanf_s("%s", filePatch, MAX_PATH);
#else
	char* filePatch = "E:\\shaiya-tmp\\sah.bak is not crypted\\New folder\\data.sah";
#endif

	//2. �����ļ�����
	char strBackFile[MAX_PATH] = { 0 };
	sprintf_s(strBackFile, MAX_PATH, "%s.bak", filePatch);
	CopyFileA(filePatch, strBackFile, 0);

	//3. ���ļ�
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

	//4. ��ȡ�ļ���ʽ
	g_dwSize = GetFileSize(g_hFile, 0);         //��ȡ�ļ���С
	sah = new BYTE[g_dwSize];                   //�����ڴ�ռ��С
	DWORD dwRet;
	ReadFile(g_hFile, sah, g_dwSize, &dwRet, 0);//�����ڴ�

	//5. ��ȡ�ļ�ͷȷ���ļ��Ǽ��ܻ��ǽ���״̬
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

	//6. ���û�ѡ���Ƿ���Ҫ�����ӽ���
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

	//7. ��д�ļ�ͷ��д��ӽ��ܱ�ʶ
	DWORD dwWriteen;
	SetFilePointer(g_hFile, 0, 0, FILE_BEGIN);
	WriteFile(g_hFile, strSignature, strlen(strSignature), &dwWriteen, 0);

	/********************************************************************************/

	//��ȡ�ļ�����
	filecount = + sah[7];

	//�����ļ�������������ռ�



	getfile();

	//��¼�ļ�������,��������ǵ�һ�����ļ�������
	DWORD dircount = *(DWORD*)&sah[start];
	start += 4;
	getdir(dircount);


	delete[] sah;
}

void getfile()
{
	g_Temp++;

	//��ѯ�ļ������ļ�������
	DWORD dirfilecount = *(DWORD*)&sah[start];

	if (g_byToolType == _Decrypt)
	{
		//����Ǽ��ܵľ���Ҫ����
		dirfilecount = DecryptSah(dirfilecount);
	}

	//���Ҫ����ܾͼ���һ��
	if (g_byToolType == _Encrypt)
	{
		EncryptSah(dirfilecount);
	}

	if (g_byToolType == _PrintSah)
	{
		printf("FileCount:%u\n", dirfilecount);
		Sleep(1000);
	}



	start += 4;//��ʱ��60����λ���ļ�������
	if (dirfilecount == 0)
	{
		return;
	}

	//�����ļ�������ļ�
	for (int i = 0; i < dirfilecount; i++)
	{
		//�ļ���
		DWORD filenamelen = *(DWORD*)&sah[start] - 1;
		start += 4;                          //start��ʱ��64,��λ���ļ���
		char strName[200] = {0};

		//....ȡ�ļ���
/*		memcpy_s(strName, 200, &sah[start], filenamelen);*/


		start += filenamelen + 1;          //64+�ļ�������+1=76
	
/*		filehoffset[filecountnow] = start; //���Ӧ��û��ʲô��*/

		//......ȡƫ�Ƶ�ַ
// 		fileoffset[filecountnow] = 
// 			  (DWORD)sah[start + 4] * 256 * 256 * 256 * 256 
// 			+ (DWORD)sah[start + 3] * 256 * 256 * 256
// 			+ (DWORD)sah[start + 2] * 256 * 256
// 			+ (DWORD)sah[start + 1] * 256
// 			+ (DWORD)sah[start];

		start += 8;    //83

		//.......ȡ��С
/*		filelen[filecountnow] = (*(PDWORD)&sah[start]); //��������4���ֽ��ټ�ȥ10,Ҳ�п�����8���ֽ�*/

		start += 8;    //91 ��ʱ��λ����Ŀ¼����ļ�������
		filecountnow++;
	}
}

bool aaa = false;
void getdir(int dir_count)
{
	for (int i = 0; i < dir_count; i++)
	{
	errnext:
		//��ȡ�ļ������ĳ���
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
				if (sah[start] < 32)         //�����ǲ��ܶ�ȡ������
					goto errnext;
			}

		}
		start += dirnamelen_temp + 1;

		//�ֻص�֮ǰ���ļ�������ȥ
		int dircount_temp = sah[start - dirnamelen_temp - 1 - 8];

		//��ȡ����ļ���������ļ�
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

//���ܺ���
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

//���ܺ���
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