// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// pvpDrops.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO:  �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������


DWORD GetRand(DWORD dwRange, DWORD dwStart)
{
	PVOID pCall = (PVOID)0x0051BD66;
	DWORD dwRand = 0;
	_asm
	{
		call pCall
			mov dwRand, eax
	}
	return  dwRand % dwRange + dwStart;
}

BOOL IsRandSuccessful(DWORD dwRate, DWORD dwRange)
{
	return dwRate >= GetRand(dwRange) ? TRUE : FALSE;  //�ڷ�Χ�ھ��ǳɹ�
}
