#pragma once
#include <fstream>

using std::fstream;
using std::ios_base;

class CFileReadWrite
{
public:
	CFileReadWrite();
	~CFileReadWrite(void);
	BOOL Open(LPWSTR lpPath);              //���ļ�
	void ReadData  (PVOID pBuff,int nSize);//������
	void WriteData (PVOID pBuff,int nSize);//����
	void SetOffset(LONG lOffset);		   //����ƫ��
	void Close();						   //�ر��ļ����
public:
	LPCTSTR m_lpPath;//·��
	int     m_nType; //�ļ��Ĵ򿪷�ʽ
	HANDLE  m_hFile; //�ļ�ָ��
};

