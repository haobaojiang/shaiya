#ifndef SHAIYA_CALLBACK_HEADER
#define SHAIYA_CALLBACK_HEADER


#include "onLogin.hpp"

namespace GameEventCallBack {

	template <class F>
	void AddLoginCallBack(F fun) {
		onLogin::AddCallBack(fun);
	}
}


#endif