
#include <iostream>
#include <Windows.h>





namespace Game {

	/*
	static DWORD 执行GM命令call = 0x470A10;
	static DWORD 账号基址内存地址 = 0x40A182;
	static DWORD 账号基址 = 0x22258A0;
	static DWORD 开启坐标显示基址 = 0x21F22E0;
	static DWORD GM权限基址 = 0x84F6AC;
	static DWORD 显示怪物血内存地址 = 0x004FA554;
	//------------------------------ - 游戏基址------------------------;
	// 偏移 = 0xE;
	static DWORD 游戏基址内存地址 = 0x40DC1E;
	static DWORD 游戏基址 = 0x7071E0;

	static DWORD 人物血偏移内存地址 = 0x4F6DBF;
	static DWORD 人物血偏移 = 0x130;
	//------------------------------ - 回车栏基址------------------------;
	// 偏移 = 0x16;
	static DWORD 回车栏内存地址 = 0x46D55D;
	static DWORD 回车栏基址 = 0x84EEE8;
	static DWORD 回车栏基址2 = 0x703B58;
	//------------------------------ - 快捷键基址------------------------;
	// 偏移 = 0xFFFFFFFC;
	static DWORD 快捷键内存地址 = 0x40999D;
	static DWORD 快捷键基址 = 0x7070A0;
	//------------------------------ - 地图偏移------------------------;
	//偏移 = 0xFFFFFFFC;
	static DWORD 地图1级偏移内存地址 = 0x4BB785;
	static DWORD 地图1级偏移 = 0x5DC;
	//偏移 = 0x5;
	static DWORD 地图2级偏移内存地址 = 0x42FB20;
	static DWORD 地图2级偏移 = 0x19C;
	//------------------------------ - 使用物品call------------------------;
	// 偏移 = 0x19;
	static DWORD 使用物品call内存地址 = 0x4E7315;
	static DWORD 使用物品call = 0x4C8C80;
	static DWORD 使用物品基址内存地址 = 0x4E7310;
	static DWORD 使用物品基址 = 0x84F6A8;
	static DWORD 使用物品冷却内存地址 = 0x4C8D2F;
	static DWORD 使用物品冷却 = 0x19978E0;
	//------------------------------ - 读取物品名字call------------------------;
	// 偏移 = 0x11;
	static DWORD 读取物品名字call内存地址 = 0x461110;
	static DWORD 读取物品名字call = 0x461110;
	static DWORD 读取物品名偏移1_品质 = 0x10C2;
	static DWORD 读取物品名偏移2_类型 = 0x10C1;
	static DWORD 读取物品名偏移3 = 0x7894;
	//------------------------------ - 背包基址------------------------;
	// 偏移 = 0xB;
	static DWORD 背包基址内存地址 = 0x45E70B;
	static DWORD 背包基址 = 0x850769;
	//------------------------------ - 捡物品call------------------------;
	// 偏移 = 0xFFFFFFF6;
	static DWORD 捡物品call内存地址 = 0x5872B0;
	static DWORD 捡物品call = 0x5872B0;
	//------------------------------ - 目标异常内存地址------------------------;
	// 偏移 = 0x0;
	static DWORD 目标异常内存地址 = 0x41C41C;
	//------------------------------ - 遍历怪物偏移------------------------;
	// 偏移 = 0x2;
	static DWORD 遍历怪物偏移内存地址 = 0x449779;
	static DWORD 遍历怪物偏移 = 0x13D89C;
	//------------------------------ - 人物基址偏移------------------------;
	//偏移 = 0xE;
	static DWORD 基址 = 0x8470AC;
	static DWORD 人物基址偏移内存地址 = 0x4409B8;
	static DWORD 人物基址偏移 = 0x13FECC;
	static DWORD 无视地形内存地址 = 0x59FAA5;
	static DWORD 无视地形2内存地址 = 0x58DCCD;
	static DWORD 穿墙内存地址 = 0x594885;
	static DWORD 穿墙内存地址2 = 0x59336A;
	static DWORD 穿墙内存地址_视角拉近 = 0x5932AC;
	//------------------------------ - 卖物品call------------------------;
	//偏移 = 0x15;
	static DWORD 卖物品call内存地址 = 0x4F93E6;
	static DWORD 卖物品call = 0x587C30;
	//------------------------------ - 快捷键call------------------------;
	//偏移 = 0x14;
	static DWORD 快捷键call内存地址 = 0x4D6BF6;
	static DWORD 快捷键call = 0x4D5FC0;
	//------------------------------ - static DWORD 公告call1------------------------;
	//偏移 = 0x0;
	static DWORD 公告call1内存地址 = 0x41C390;
	static DWORD 公告call1 = 0x41C390;
	//------------------------------ - static DWORD 公告call2------------------------;
	//偏移 = 0x0;
	static DWORD 公告call2内存地址 = 0x41E4E0;
	static DWORD 公告call2 = 0x41E4E0;
	//------------------------------ - 公告call基址------------------------;
	//偏移 = 0xFFFFFFFC;
	static DWORD 公告call基址内存地址 = 0x438486;
	static DWORD 公告call基址 = 0x703418;
	//------------------------------ - 选怪call------------------------;
	static DWORD 选怪call内存地址 = 0x41ED11;
	static DWORD 选怪call = 0x447DA0;
	static DWORD 选怪call基址内存地址 = 0x41ECE7;
	static DWORD 选怪call基址 = 0x851A88;
	//------------------------------ - 穿墙打怪内存地址------------------------;
	static DWORD 穿墙打怪内存地址 = 0x43852F;
	static DWORD 穿墙打怪内存地址1 = 0x438579;
	static DWORD 穿墙打怪内存地址2 = 0x4385C1;
	static DWORD 穿墙打怪内存地址3 = 0x4385E1;
	static DWORD 穿墙打怪内存地址4 = 0x43859D;
	//------------------------------ - 穿地形障碍物技能内存地址------------------------;
	static DWORD 穿墙打怪内存地址 = 0x441394;
	static DWORD 超远程技能call = 0x587BA0;
	static DWORD 技能call = 0x587BA0;
	//------------------------------ - 物理攻击call------------------------;
	static DWORD 物理攻击call内存地址 = 0x44154C;
	static DWORD 物理攻击call = 0x587A50;
	static DWORD 物理攻击call_玩家内存地址 = 0x587A20;
	static DWORD 物理攻击call_玩家 = 0x587A20;


	static DWORD 物理攻击穿墙内存地址 = 0x4414F6;
	static DWORD 走路标示基址内存地址 = 0x441557;
	static DWORD 走路标示基址 = 0x7070F0;
	static DWORD 选怪偏移 = 0x160;
	static DWORD 走路标示基址内存地址 = 0x44AF39;
	static DWORD 走路标示偏移X = 0x148;
	static DWORD 走路标示偏移Y = 0x150;
	//------------------------------ - 复活call------------------------;
	static DWORD 复活call内存地址 = 0x588050;
	static DWORD 复活call = 0x588050;
	static DWORD 复活call标示内存地址 = 0x4B8557;
	static DWORD 复活call标示 = 0x279;
	//------------------------------ - 技能序号获取技能ID------------------------;
	static DWORD 技能序号获取技能ID内存地址 = 0x43B7C0;
	static DWORD 技能序号获取技能IDcall = 0x43B7C0;
	static DWORD 技能序号获取技能ID基址内存地址 = 0x4407C2;
	static DWORD 技能序号获取技能ID基址 = 0x21E6854;
	//------------------------------ - 读取技能名字基址 + 技能状态------------------------;
	static DWORD 读取技能名字基址内存地址 = 0x4212D7;
	static DWORD 读取技能名字基址 = 0x856F3C;
	static DWORD 读取技能名字call内存地址 = 0x4212DC;
	static DWORD 读取技能名字call = 0x461FE0;
	//------------------------------ - 读取技能使用冷却基址------------------------;
	static DWORD 读取技能使用冷却基址内存地址 = 0x4B95C5;
	static DWORD 读取技能使用冷却基址 = 0x21E685C;
	//------------------------------ - 读取状态基址------------------------;
	static DWORD 读取状态名字基址内存地址 = 0x4B96B3;
	static DWORD 读取状态名字基址 = 0x21E68A4;
	static DWORD 读取负面状态名字基址内存地址 = 0x4B96DE;
	static DWORD 读取负面状态名字基址 = 0x856F6C;
	//------------------------------ - 遍历地面物品偏移------------------------;
	static DWORD 遍历地面物品偏移内存地址 = 0x43BB23;
	static DWORD 遍历地面物品偏移 = 0x13D88C;
	//------------------------------ - 辅助技能call------------------------;
	static DWORD 辅助技能call内存地址 = 0x587B60;
	static DWORD 辅助技能call = 0x587B60;
	//------------------------------ - 读取怪物名字内存地址------------------------;
	static DWORD 读取怪物名字call内存地址 = 0x461650;
	static DWORD 读取怪物名字call = 0x461650;
	static DWORD 读取怪物名字call基址内存地址 = 0x553223;
	static DWORD 读取怪物名字call基址 = 0x856F3C;
	static DWORD 读取怪物名字call基址2 ＝ 进制_十六到十(static DWORD 读取怪物名字call基址)
		//------------------------------ - 人物视野基址------------------------;
		static DWORD 人物视野基址内存地址 = 0x40F8A0;
	static DWORD 人物视野基址 = 0x21F1F90;
	//------------------------------ - 喊话call------------------------;
	static DWORD 喊话call内存地址 = 0x5889B0;
	static DWORD 喊话call = 0x5889B0;
	static DWORD 喊话call_普通内存地址 = 0x5887C0;
	static DWORD 喊话call_普通 = 0x5887C0;
	static DWORD 喊话call_行会内存地址 = 0x588940;
	static DWORD 喊话call_行会 = 0x588940;
	static DWORD 喊话call_地区内存地址 = 0x588A20;
	static DWORD 喊话call_地区 = 0x588A20;
	//------------------------------ - 购买物品call------------------------;
	static DWORD 购买物品call内存地址 = 0x587BE0;
	static DWORD 购买物品call = 0x587BE0;
	//' ''“------------------------强化合成装备------------------------;
		static DWORD 强化装备call内存地址 = 0x587CF0;
	static DWORD 强化装备call = 0x587CF0;
	static DWORD 合成装备call内存地址 = 0x587C80;
	static DWORD 合成装备call = 0x587C80;
	static DWORD 更换装备call内存地址 = 0x587260;
	static DWORD 更换装备call = 0x587260;
	static DWORD 属性改造call = 0x00587D60;
	//------------------------------ - 确认接受组队call------------------------;
	static DWORD 确认接受组队call基址 = 0x850764;
	static DWORD 确认接受组队call偏移 = 0x238;
	static DWORD 确认接受组队call内存地址 = 0x48EFDD;
	static DWORD 确认接受组队call = 0x588350;
	static DWORD 关闭确认接受组队基址 = 0x852ACC;
	static DWORD 组玩家call内存地址 = 0x588320;
	static DWORD 组玩家call = 0x588320;

	//------------------------------ - 普通组队遍历基址------------------------;
	static DWORD 普通组队遍历基址 = 0x21E68B8;
	static DWORD 普通组队遍历基址内存地址 = 0x44BD7D;
	static DWORD 联盟组队遍历基址 = 0x21E68CC;
	static DWORD 联盟组队遍历基址内存地址 = 0x505472;
	static DWORD 遍历玩家死亡偏移 = 0x250;
	static DWORD 遍历玩家死亡偏移内存地址 = 0x418E8F;
	static DWORD 遍历玩家队长权限偏移 = 0x2C0;
	static DWORD 遍历玩家队长权限偏移内存地址 = 0x44C063;
	static DWORD 遍历玩家攻击怪物ID偏移 = 0x1B8;
	static DWORD 遍历玩家攻击怪物ID偏移内存地址 = 0x54EFD7;
	static DWORD 遍历玩家阵营偏移 = 0x276;  //' 可以在邀请决斗call 下面不远就有偏移
	static DWORD 遍历玩家阵营偏移内存地址 = 0x490FE0;

	//------------------------------ - 打坐call------------------------;
	static DWORD 打坐call内存地址 = 0x587A80;
	static DWORD 打坐call = 0x587A80;
	//------------------------------ - 交任务call------------------------;
	static DWORD 交任务call内存地址 = 0x587F40;
	static DWORD 交任务call = 0x587F40;
	//------------------------------ - 接任务call------------------------;
	static DWORD 接任务call内存地址 = 0x587F00;
	static DWORD 接任务call = 0x587F00;
	//------------------------------ - 当前对话NPC基址------------------------;
	static DWORD 当前对话NPC基址 = 0x852AD4;
	static DWORD 当前对话NPC基址内存地址 = 0x48D40A;
	static DWORD 当前对话任务ID = 0x856F0E;
	static DWORD 当前对话任务ID内存地址 = 0x44387C;
	//------------------------------ - 升级技能call------------------------;
	static DWORD 升级技能call内存地址 = 0x5873B0;
	static DWORD 升级技能call = 0x5873B0;
	//------------------------------ - 圣光复活队员call------------------------;
	static DWORD 圣光复活队员call内存地址 = 0x4C902B;
	static DWORD 圣光复活队员call = 0x58AE40;
	//------------------------------------------------------ - ;
	static DWORD 丢弃物品call内存地址 = 0x5872E0;
	static DWORD 丢弃物品call = 0x5872E0;
	static DWORD 地图ID基址 = 0x850750;
	static DWORD 加点call = 0x587330;
	static DWORD 交易喊话限制 = 0x4782f1;


	HRESULT GetGamePid(__inout DWORD* Pid) {

		HRESULT hr = S_OK;
		*Pid = 0;

		auto callBack = [&](PROCESSENTRY32W entry) {

			if (wcsicmp(entry.szExeFile, L"shaiya.exe") == 0 ||
				wcsicmp(entry.szExeFile, L"game.exe") == 0)
			{
				*Pid = entry.th32ProcessID;
			}
		};

		hr = Utility::Process::EnumProcess(callBack);
		if (FAILED(hr)) {
			return hr;
		}

		return hr;
	}


	HRESULT GetGameVersion() {

		DWORD pid = 0;
		auto hr = GetGamePid(&pid);
		if (FAILED(hr)) {
			return hr;
		}
	}

	namespace Self {

		struct Info {
			BYTE padding[0x10];
			float x;    //0x10
			float y;
			float z;

			char name[20]; //0xf0

			BYTE padding_1[0x11];
			int hp;     //0x130
			int maxhp;
			int mp;
			int maxmp;
			int sp;
			int maxsp;
		};

		BYTE GetMap() {
		//	return Game::Process::Read(地图ID基址);
		}
	}
	*/



}



#include "inventory.h"

int main(int , char** )
{	
	Game::Inventory::GetAllItems();
	return 0;
}