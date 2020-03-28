// stdafx.cpp : 只包括标准包含文件的源文件
// pvpDrops.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO:  在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用


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
	return dwRate >= GetRand(dwRange) ? TRUE : FALSE;  //在范围内就是成功
}
