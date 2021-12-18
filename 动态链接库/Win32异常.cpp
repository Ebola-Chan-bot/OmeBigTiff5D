#include "pch.h"
#include "Win32异常.h"
Win32异常::Win32异常(DWORD 错误代码, const char* 错误消息):错误消息(错误消息),错误代码(错误代码){}