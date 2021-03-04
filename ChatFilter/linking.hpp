

#ifndef SHAIYA_LINKING_HEADER
#define SHAIYA_LINKING_HEADER

#include "stdafx.h"
#include "../utility/utility.h"
#include "config.hpp"

namespace linking
{
	ShaiyaUtility::CMyInlineHook g_hookPrice, g_hookRate1, g_hookRate2, g_hookRate3;

	std::array<DWORD,100> g_prices;

	DWORD __stdcall GetLinkPrice(DWORD reqig)
	{
		DWORD idx = reqig - 101;
		return g_prices.at(idx);
	}
	
	DWORD g_rate3Addr = 0x0046faf9;
	__declspec(naked) void Naked_Rate3()
	{
		_asm{
			movzx ecx,word ptr [edi+0x136]
			
			cmp edx, 0x64
			jl _org
			cmp edx, 0xc8
			jg _org

			sub edx,0x64
			mov eax,edx

			jmp g_rate3Addr


			_org :
				jmp g_hookRate3.m_pRet
		}
	}

	

	__declspec(naked) void Naked_Rate2()
	{
		_asm
		{
			mov edx,dword ptr[ebx+0x30]
			mov edx, dword ptr[edx + 0x32]
			cmp edx, 0x64
			jl _org
			cmp edx, 0xc8
			jg _org

			sub edx, 0x64
			mov eax, edx


			_org :
			movzx edx, byte ptr[ebp + 0x12d]
				jmp g_hookRate2.m_pRet
		}
	}


	__declspec(naked) void Naked_Rate1()
	{
		_asm
		{
			mov edx,dword ptr [esp+0x1c]
			mov edx,dword ptr [edx+0x30]
			mov edx,dword ptr [edx+0x32]
			cmp edx, 0x64
			jl _org
			cmp edx, 0xc8
			jg _org

			sub edx, 0x64
			mov eax, edx


			_org:
			movzx edx ,byte ptr [ebp+0x12d]
			jmp g_hookRate1.m_pRet
		}
	}
	
	__declspec(naked) void Naked_Price()
	{
		_asm
		{
			cmp eax,0x64
			jl _org
			cmp eax,0xc8
			jg _org

			push ebx
			push ecx
			push edx
			push edi
			push esi
			push ebp
			push esp	
			MYASMCALL_1(GetLinkPrice,eax)
			pop esp
			pop ebp
			pop esi
			pop edi
			pop edx
			pop ecx
			pop ebx
			ret

			_org:
			lea ecx, dword ptr ds : [eax - 0x1E]
			cmp ecx, 0xA
			jmp g_hookPrice.m_pRet
		}
	}

	void start()
	{
		if (!GameConfiguration::IsFeatureEnabled("custom_linking")) {
			return;
		}



		auto path = ShaiyaUtility::GetCurrentExePathA();
		path += "\\custom_linking.ini";
		auto p = new INIReader(path.c_str());
		


		// read price
		for (auto i = 0;i < 100;i++)
		{
			char key[200]{};
			sprintf_s(key, 200, "%d", i + 1);
			int price = p->GetInteger("price", key, 0);
			if (price == 0)
			{
				if (i > 0)
				{
					price = g_prices.at(i - 1);
				}
				else
				{
					price = 50000; // default rate
				}
			}
			LOGD << "i:" << i << "  price:" << price;
			g_prices[i] = price+20;
		}

		
		
		// 0046C720 | 8D48 E2                  | lea ecx,dword ptr ds:[eax-1E]           | calc_stone_link_money
		//0046C723 | 83F9 0A                  | cmp ecx,A                               | A:'\n'
		g_hookPrice.Hook(reinterpret_cast<void*>(0x0046C720), Naked_Price, 6);

		// 0046E5F5 | 0FB695 2D010000          | movzx edx,byte ptr ss:[ebp+12D]         | eax = basic rate
		g_hookRate1.Hook(reinterpret_cast<void*>(0x0046E5F5), Naked_Rate1,7);

		//
		g_hookRate2.Hook(reinterpret_cast<void*>(0x0046F1D5), Naked_Rate2,7 );

		//
		g_hookRate3.Hook(reinterpret_cast<void*>(0x0046faf9), Naked_Rate3, 7);
	}
}


#endif
