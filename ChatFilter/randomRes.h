#ifndef RANDOM_RED_HEADER_DEF
#define RANDOM_RED_HEADER_DEF

namespace randomRes
{
	struct Pos {
		float x{};
		float y{};
		float z{};
	};

	ShaiyaUtility::CMyInlineHook g_obj;
	std::map<WORD, std::vector<Pos>> g_data;

	int get_rand_idx(int maximum)
	{
		auto tickcount = GetTickCount();
		return tickcount % maximum;
	}

	void __stdcall fun(void* Player, WORD map, Pos* player_pos)
	{
		auto item = g_data.find(map);
		if (item == g_data.end()) {
			return;
		}

		auto idx = get_rand_idx(item->second.size());
		auto pos = item->second[idx];
		*player_pos = pos;
	}


	__declspec(naked) void  Naked()
	{
		_asm {
			pushad
			mov eax,dword ptr [esp+0x2c]
			mov edi,dword ptr [esp+0x24]
			MYASMCALL_3(fun, ecx, eax, edi)
			popad
			sub esp, 0x98
			jmp g_obj.m_pRet
		}
	}


	void Start()
	{
		g_obj.Hook(PVOID(0x00466da0), Naked, 6);

		for (int i = 0; i < 200; i++) {

			char section[MAX_PATH]{};
			sprintf_s(section, MAX_PATH, "randomRes_%02d", i);

			// find map pos
			std::vector<Pos> positions;
			for (auto i = 0; i < 20; i++) {

				Pos pos;
				char key[MAX_PATH]{};
				sprintf_s(key, "%d", i);

				auto pos_str = GameConfiguration::Get(section, key, "");
				if (pos_str == "") {
					break;
				}
				sscanf(pos_str.c_str(), "%f,%f,%f", &pos.x, &pos.y, &pos.z);
				positions.push_back(pos);
			}

			// no pos then we skip it
			if (positions.size() == 0) {
				continue;
			}

			g_data[i] = positions;
		}

		LOGD << "size of g_data:" << g_data.size();
	}
}









#endif








