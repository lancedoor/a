#pragma once

#define _DECLARE_SINGLETON(cls)\
public:\
	static cls* Get() {\
		if (!s_singleton)\
			s_singleton = new cls;\
		return s_singleton;\
	}\
private:\
	static cls* s_singleton;\

#define DECLARE_SINGLETON(cls)\
_DECLARE_SINGLETON(cls)\
private:\
	cls() {}\
	~cls() {}\

#define DEFINE_SINGLETON(cls) cls* cls::s_singleton = nullptr;

