// ShowPacket.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "ShowPacket.h"


// ���ǵ���������һ��ʾ��
SHOWPACKET_API int nShowPacket=0;

// ���ǵ���������һ��ʾ����
extern "C" __declspec(dllexport) int fnShowPacket(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� ShowPacket.h
CShowPacket::CShowPacket()
{
	return;
}
