#pragma once
#include <windows.h>



class CFileReadWrite
{
public:
	CFileReadWrite();
	~CFileReadWrite(void);
	void Open(LPWSTR lpPath);              //���ļ�
	void ReadData  (PVOID pBuffer,int nSize);//������

	void ReadStr(PVOID pBuffer); //���ַ��� 
	void WriteStr(PVOID pBuffer);//д�ַ���
	void AppendStr(PVOID pBuffer);//׷��һ���ַ���


	void WriteData (PVOID pBuffer,int nSize);//����
	void SetOffset(LONG lOffset);		   //����ƫ��
	void Close();						   //�ر��ļ����
	BOOL IsEnd();//�����Ƿ񵽽�β��
	
public:
	LPCTSTR m_lpPath;//·��
	int     m_nType; //�ļ��Ĵ򿪷�ʽ
	HANDLE  m_hFile; //�ļ�ָ��
};

