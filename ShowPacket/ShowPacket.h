// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� SHOWPACKET_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// SHOWPACKET_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef SHOWPACKET_EXPORTS
#define SHOWPACKET_API __declspec(dllexport)
#else
#define SHOWPACKET_API __declspec(dllimport)
#endif

// �����Ǵ� ShowPacket.dll ������
class SHOWPACKET_API CShowPacket {
public:
	CShowPacket(void);
	// TODO:  �ڴ�������ķ�����
};

extern SHOWPACKET_API int nShowPacket;

extern "C" __declspec(dllexport) int fnShowPacket(void);
