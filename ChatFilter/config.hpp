#pragma once
#ifndef SHAIYA_CONFIGURATION_HEADER
#define SHAIYA_CONFIGURATION_HEADER

#pragma warning( push )
#pragma warning( disable : 4996 )
#include <INIReader.h>
#pragma warning( pop )
#include <windows.h>



namespace GameConfiguration {

	INIReader* g_iniReader = nullptr;


	std::string Get(std::string section, std::string name, std::string default_value = "") {
		return g_iniReader->Get(section, name, default_value);
	}

	long GetInteger(std::string section, std::string name, long default_value = 0) {
		return g_iniReader->GetInteger(section, name, default_value);
	}

	double GetReal(std::string section, std::string name, double default_value = 0) {
		return g_iniReader->GetReal(section, name, default_value);
	}


	bool GetBoolean(std::string section, std::string name, bool default_value = false) {
		return g_iniReader->GetBoolean(section, name, default_value);
	}

	bool IsFeatureEnabled(std::string section) {
		auto result = GetBoolean(section, "enable");
		if (result) {
			LOGD << section << "  enable";
		}
		return result;
	}

	INIReader* GenerateReader() {

		auto path = ShaiyaUtility::GetCurrentExePathA();
		path += "\\shaiya.ini";
		auto p = new INIReader(path.c_str());
		if (p->ParseError() != 0) {
			delete p;
			SetLastError(ERROR_BAD_FORMAT);
			return nullptr;
		}
		return p;

	}

	void ReLoad(void*) {

		while (true)
		{
			Sleep(40 * 1000);
			auto temp = GenerateReader();
			if (temp == nullptr) {
				continue;
			}

			auto oldPointer = reinterpret_cast<INIReader*>(InterlockedExchange(reinterpret_cast<unsigned long*>(&g_iniReader),
				reinterpret_cast<unsigned long>(temp)));
			Sleep(10 * 1000);
			delete oldPointer;
		}
	}

	BOOL Init() {


		g_iniReader = GenerateReader();
		_beginthread(ReLoad, 0, 0);


		return g_iniReader != nullptr;
	}



}

#endif