#pragma once

#include <windows.h>

namespace framework
{
	namespace Threading
	{
		class CIntraProcessLock
		{
		public:
			CIntraProcessLock()
			{
				InitializeCriticalSection(&m_cs);
			}
			
			~CIntraProcessLock()
			{
				DeleteCriticalSection(&m_cs);
			}

			inline bool Lock()
			{
				return TryEnterCriticalSection(&m_cs) > 0 ? true : false;
			}

			inline void Unlock()
			{
				return LeaveCriticalSection(&m_cs);
			}

		private:
			CRITICAL_SECTION m_cs;
		};

		class CNoLock
		{
		public:
			inline bool Lock() 
			{ 
				return true; 
			}
			
			inline void Unlock()
			{
				return;
			}
		};
	}
}