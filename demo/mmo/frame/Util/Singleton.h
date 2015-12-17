#pragma once
#include <mutex>

#define _DECLARE_SINGLETON(cls)\
public:\
	static cls* Get() {\
    std::lock_guard<std::mutex> lg(s_singleton_mutex);\
		if (!s_singleton)\
			s_singleton = new cls;\
		return s_singleton;\
	}\
private:\
	static cls* s_singleton;\
  static std::mutex s_singleton_mutex;\

#define DECLARE_SINGLETON(cls)\
_DECLARE_SINGLETON(cls)\
private:\
	cls() {}\
	~cls() {}\

#define DEFINE_SINGLETON(cls)\
cls* cls::s_singleton = nullptr;\
std::mutex cls::s_singleton_mutex;

