// Filename		:	tstring.h
// Author		:	Siddharth Barman
// Date			:	18 April 2012
// Description	:	Wrap a critical section to provide automatic locking/unlocking
//--------------------------------------------------------------------------------

#ifndef TSTRING_H_DEFINED
#define TSTRING_H_DEFINED

#include <string>
#include <tchar.h>

#if defined(UNICODE) || defined(_UNICODE)
	#define tstring std::wstring
	#define tout std::wcout
	#define tin std::wcin
	#define tostream std::wostream
	#define tofstream std::wofstream
	#define tifstream std::wifstream
	#define tfstream std::wfstream
#else
	#define tstring std::string
	#define tout std::cout
	#define tin std::cin
	#define tostream std::ostream
	#define tofstream std::ofstream
	#define tifstream std::ifstream
	#define tfstream std::fstream
#endif


#endif