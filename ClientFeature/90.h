
#include <windows.h>
#include <mutex>
#include "../utility/utility.h"
#include "kiero.h"

// #define  SHAIYA_WINDOW 0
#ifdef SHAIYA_WINDOW
#include <d3d9.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#endif // SHAIYA_WINDOW
#include <assert.h>


// #pragma comment(lib,"E:\\code\\ShaiyaFixProject\\ClientFeature\\Debug\\libMinHook.x86.lib")
namespace Shaiya90
{
#ifdef SHAIYA_WINDOW
	namespace GameWindow {

		// Create the type of function that we will hook
		typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
		static EndScene oEndScene = nullptr;

		typedef long(__stdcall* Reset)(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresParam);
		static Reset oReset = nullptr;

		typedef HRESULT(__stdcall* Present)(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
		static Present oPresent = nullptr;

		//
		static WNDPROC g_oldWndProc = nullptr;
		static HWND g_hwnd = nullptr;

		//
		static bool g_menuInit = false;
		static bool g_menuEnable = false;


		LRESULT __stdcall ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			ImGuiIO& io = ImGui::GetIO();
			switch (msg)
			{
			case WM_LBUTTONDOWN:
				io.MouseDown[0] = true;
				return true;
			case WM_LBUTTONUP:
				io.MouseDown[0] = false;
				return true;
			case WM_RBUTTONDOWN:
				io.MouseDown[1] = true;
				return true;
			case WM_RBUTTONUP:
				io.MouseDown[1] = false;
				return true;
			case WM_MBUTTONDOWN:
				io.MouseDown[2] = true;
				return true;
			case WM_MBUTTONUP:
				io.MouseDown[2] = false;
				return true;
			case WM_MOUSEWHEEL:
				io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
				return true;
			case WM_MOUSEMOVE:
				io.MousePos.x = (signed short)(lParam);
				io.MousePos.y = (signed short)(lParam >> 16);
				return true;
			case WM_KEYDOWN:
				if (wParam < 256)
					io.KeysDown[wParam] = 1;
				return true;
			case WM_KEYUP:
				if (wParam < 256)
					io.KeysDown[wParam] = 0;
				return true;
			case WM_CHAR:
				// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
				if (wParam > 0 && wParam < 0x10000)
					io.AddInputCharacter((unsigned short)wParam);
				return true;
			}
			return 0;
		}

		long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
		{
			return oEndScene(pDevice);
		}

		long __stdcall hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresParam)
		{
			if (!g_menuEnable) {
				return oReset(pDevice, pPresParam);
			}

			ImGui_ImplDX9_InvalidateDeviceObjects();

			return oReset(pDevice, pPresParam);
		}

		HRESULT __stdcall hkPresent(LPDIRECT3DDEVICE9 pDevice, 
			const RECT* pSourceRect, 
			const RECT* pDestRect, 
			HWND hDestWindowOverride, 
			const RGNDATA* pDirtyRegion)
		{

			if (!g_menuInit) {
				ImGui::CreateContext();
				ImGui_ImplWin32_Init(g_hwnd);
				ImGui_ImplDX9_Init(pDevice);
				ImGui::StyleColorsDark();
				ImGuiIO& io = ImGui::GetIO(); (void)io;
				g_menuInit = true;
			}

			if (!g_menuEnable) {
				return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
			}

			static bool show_demo_window = true;
			static bool show_another_window = false;
			static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



			// Start the Dear ImGui frame
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!",nullptr, ImGuiWindowFlags_AlwaysAutoResize);                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)& clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			// 3. Show another simple window.
			if (show_another_window)
			{
				ImGui::Begin("Another Window", &show_another_window, ImGuiWindowFlags_AlwaysAutoResize);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}

			// Rendering
			ImGui::EndFrame();
			pDevice->SetRenderState(D3DRS_ZENABLE, false);
			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
			pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
			if (pDevice->BeginScene() >= 0)
			{
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				pDevice->EndScene();
			}
			return oPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		}

		
		LRESULT __stdcall Hooked_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

			if (uMsg == WM_KEYUP && 
				wParam == VK_INSERT) 
			{
				g_menuEnable = !g_menuEnable;
			}

			if (!g_menuEnable) {
				return CallWindowProc(g_oldWndProc, hwnd, uMsg, wParam, lParam);
			}

			switch (uMsg)
			{
			case WM_LBUTTONDOWN:
				ImGui::GetIO().MouseDown[0] = true; return DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			case WM_LBUTTONUP:
				ImGui::GetIO().MouseDown[0] = false; return DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			case WM_RBUTTONDOWN:
				ImGui::GetIO().MouseDown[1] = true; return DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			case WM_RBUTTONUP:
				ImGui::GetIO().MouseDown[1] = false; return DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			case WM_MBUTTONDOWN:
				ImGui::GetIO().MouseDown[2] = true; return DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			case WM_MBUTTONUP:
				ImGui::GetIO().MouseDown[2] = false; return DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			case WM_MOUSEWHEEL:
				ImGui::GetIO().MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f; return DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			case WM_MOUSEMOVE:
				ImGui::GetIO().MousePos.x = (signed short)(lParam); ImGui::GetIO().MousePos.y = (signed short)(lParam >> 16); return DefWindowProc(hwnd, uMsg, wParam, lParam);
				break;
			case WM_KEYUP:
				// ImGui_ImplDX9_WndProcHandler(hwnd, uMsg, wParam, lParam);
				break;
			default:
				break;
			}

			return CallWindowProc(g_oldWndProc, hwnd, uMsg, wParam, lParam);
		}





		void HookWindowProc() {

			g_hwnd = FindWindow(L"GAME", nullptr);
			assert(g_hwnd != 0);
			if (g_hwnd == nullptr) {
				return;
			}
			
			g_oldWndProc= (WNDPROC)SetWindowLong(g_hwnd, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(Hooked_WndProc));
			assert(g_oldWndProc != 0);
		}


		void Hook(void*)
		{
			Sleep(5000);
			HookWindowProc();

			if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success) {

				kiero::bind(16, (void**)& oReset, hkReset);
				kiero::bind(17, (void**)& oPresent, hkPresent);

				return;
			}
		}


		void Start() {
			
			_beginthread(Hook, 0, 0);
		}
	}
#endif // SHAIYA_WINDOW
	namespace SkillCutting {

		ShaiyaUtility::CMyInlineHook g_fully;


		void* g_fullySkipAddr = reinterpret_cast<void*>(0x0043d635);
		void* g_fullyBlockAddr = reinterpret_cast<void*>(0x004418ca);
		__declspec(naked) void Naked_fully()
		{
			_asm
			{
				cmp eax, 0x2    // normal attack
				je _skip
				cmp eax, 0x9    // skill
				je _skip

				cmp eax, 0x8       // org
				jne _blockAddr
				jmp g_fully.m_pRet


				_blockAddr :
				jmp g_fullyBlockAddr

					_skip :
				jmp g_fullySkipAddr
			}
		}

		void FullySkillCutting() {
			static std::once_flag flag;
			std::call_once(flag, []() {
				g_fully.Hook(reinterpret_cast<void*>(0x0043d62f), Naked_fully, 6);
				});
		}

		void ProcessPacket(PVOID Packet) {
			FullySkillCutting();
		}
	}

	namespace namecolor
	{
		ShaiyaUtility::CMyInlineHook  objNormal, ObjParty;
		DWORD g_randomColor = 0xffffff;
#define MAX_COLORS  (100000)
		DWORD g_colors[MAX_COLORS] = { 0 };

		void StartWorker();


		void __stdcall SetColor(DWORD Player, PDWORD ColotOutput) {

			DWORD charid = *PDWORD(Player + 0x34);

			if (charid > MAX_COLORS - 1) {
				return;
			}

			auto rgb = g_colors[charid];
			if (rgb == ShaiyaUtility::Packet::RandomColor) {
				*ColotOutput = g_randomColor;
				return;
			}
			else if (rgb == 0) {
				return;
			}

			*ColotOutput = rgb;
		}


		void ProcessPacket(PVOID Packet) {

			auto nameColorPacket = reinterpret_cast<ShaiyaUtility::Packet::NameColorContent*>(Packet);

			StartWorker();

			for (DWORD i = 0; i < ARRAYSIZE(nameColorPacket->players); i++) {

				auto charid = nameColorPacket->players[i].charid;
				auto rgb = nameColorPacket->players[i].rgb;


				if (!charid) {
					continue;
				}

				if (!rgb) {
					continue;
				}

				if (charid > MAX_COLORS - 1) {
					return;
				}

				g_colors[charid] = 0xff000000 | rgb;
			}
		}

		void GenerateRandomColor(void*) {

			DWORD dwTemp = 0;
			while (true)
			{
				for (int i = 0; i < ARRAYSIZE(ShaiyaUtility::Packet::s_RandomColorList); i++) {
					g_randomColor = 0xff000000 | ShaiyaUtility::Packet::s_RandomColorList[i];
					Sleep(500);
				}

			}

		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				mov dword ptr ss : [esp + 0x18] , 0xffffffff
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call SetColor
				popad
				sub eax, 0x4
				jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr ss : [esp + 0x18] , 0xFF00FF00
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call SetColor
				popad
				jmp ObjParty.m_pRet
			}
		}

		void StartWorker() {

			static std::once_flag flag;
			std::call_once(flag, []() {
				objNormal.Hook((PVOID)0x0044b5c6, Naked_Normal, 11);
				ObjParty.Hook((PVOID)0x0044b664, Naked_Party, 8);
				_beginthread(GenerateRandomColor, 0, nullptr);
				});
		}
	}

	namespace killsRanking {

		void ProcessPacket(PVOID p) {

			auto packet = reinterpret_cast<ShaiyaUtility::Packet::KillsRankingContent*>(p);

			static std::once_flag flag;
			std::call_once(flag, [&]() {
				ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708298),
					packet->killsNeeded,
					sizeof(packet->killsNeeded));

				ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708118),
					packet->normalModePoints,
					sizeof(packet->normalModePoints));


				ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708198),
					packet->hardModePoints,
					sizeof(packet->hardModePoints));

				ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708218),
					packet->ultimateModePoints,
					sizeof(packet->ultimateModePoints));
				});
		}

	}

	namespace Combination {

		void Process(void* P) {
			{
				BYTE temp[] = { 0x90,0x90 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A954D, temp, sizeof(temp));
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A9555, temp, sizeof(temp));
			}

			{
				BYTE temp[] = { 0xE9 ,0xA6 ,0x00 ,0x00 ,0x00 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A945B, temp, sizeof(temp));
			}


			{
				BYTE temp[] = { 0xb0,0x01 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A9600, temp, sizeof(temp));
			}
		}
	}

	namespace custompacket
	{
		ShaiyaUtility::CMyInlineHook objPacket;
		static DWORD codeBoundary = ShaiyaUtility::Packet::Code::begin;

		void __stdcall CustomPacket(PVOID P) {

			auto cmd = static_cast<ShaiyaUtility::Packet::Header*>(P);
			switch (cmd->command) {

			case ShaiyaUtility::Packet::Code::nameColor: {
				namecolor::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::skillCutting: {
				SkillCutting::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::killsRanking: {
				killsRanking::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::itemCombines: {
				Combination::Process(P);
				break;
			}

			default:
				break;
			}
		}

		__declspec(naked) void Naked1()
		{
			_asm
			{
				cmp eax, codeBoundary
				jl _orginal
				pushad
				mov eax, [esp + 0x28]
				push eax            //packets
				call CustomPacket
				popad
				_orginal :
				cmp eax, 0xFA07
					jmp objPacket.m_pRet
			}
		}



		void Start()
		{
			//	0059BC9B | > \3D 07FA0000         cmp eax, 0xFA07
			objPacket.Hook(reinterpret_cast<void*>(0x0059BC9B), Naked1, 5);
		}
	}

	namespace ijl15Detection {

		void Start() {

			auto removeDetection = [](PVOID addr, PVOID OrgData, PVOID changedata, DWORD len) {
				if (memcmp(OrgData, addr, len) == 0) {
					ShaiyaUtility::WriteCurrentProcessMemory(addr, changedata, len);
				}
			};

			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x52 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xEB ,0x52 };
				removeDetection(PVOID(0x0040c513), temp, temp2, sizeof(temp));
			}


			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040B5B6), temp, temp2, sizeof(temp));
			}

			// 9.0 3.14mb
			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040C88F), temp, temp2, sizeof(temp));
			}

			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040bd16), temp, temp2, sizeof(temp));
			}
		}
	}

	void Start()
	{
		custompacket::Start();
		ijl15Detection::Start();
#ifdef SHAIYA_WINDOW
		 GameWindow::Start();
#endif
	}

}

