// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FIXDROPRATE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FIXDROPRATE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef FIXDROPRATE_EXPORTS
#define FIXDROPRATE_API extern "C" __declspec(dllexport)
#else
#define FIXDROPRATE_API __declspec(dllimport)
#endif




DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter);

FIXDROPRATE_API int fnFixDroprate(void);
