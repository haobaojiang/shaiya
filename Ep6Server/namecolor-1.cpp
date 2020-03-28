
#include <string>
#include <tchar.h>
#include <vector>
#include <windows.h>
#include <atlstr.h>
#include <MyInlineHook.h>
#include <MyClass.h>
#include <common.h>
#include "namecolor.h"
#include <MyPython.h>
#include "shaiya_namecolor.h"

using std::vector;
using std::string;

using namespace EP6CALL;

extern vector<fun_loop> g_vecLoopFunction;

namespace NameColor
{
	MyPython* g_objPy = NULL;

	bool IsExcuteableTime()
	{
		static DWORD nextTime = GetTickCount();
		DWORD currentTime = GetTickCount();
		if (currentTime < nextTime) {
			return false;
		}
		nextTime += 30 * 1000;
		return true;
	}

	void loop()
	{
		if (!IsExcuteableTime())
			return;


		cmd_color stcColor[200] = { 0 };
		ZeroMemory(stcColor, sizeof(stcColor));
		
		std::string retStr = "";

		g_objPy->lock();
		try
		{
			retStr = get_namecolor();
		}
		catch (...)
		{
			;
		}
		g_objPy->Unlock();

		
		if (retStr.empty())
			return;

		char* str = new char[retStr.size() + 1];
		strcpy(str, retStr.c_str());

		//±éÀúÑÕÉ«
		DWORD i = 0;
		while (*str) {

			sscanf(str, "%ld,%ld", &stcColor[i].dwCharid, &stcColor[i].dwRGB);
			if (stcColor[i].dwCharid == 0 || stcColor[i].dwRGB == 0)
				break;

			stcColor[i].dwRGB |= 0xff000000;

			i++;
			str = strstr(str, "\r\n");
			if (str == NULL)
				break;
			str = str +strlen("\r\n");
		}

		if (i){
			DWORD dwSize = (i + 2) * sizeof(cmd_color);
			send_packet_all(PacketOrder::VipColor, (PBYTE)stcColor, dwSize);
		}
		delete[] str;
	}

	void Start()
	{
		g_objPy = new MyPython();
		initshaiya_namecolor();
		g_vecLoopFunction.push_back(loop);
		Sleep(3000);
	}
}

