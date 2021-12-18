#include "pch.h"
#include "寻找字符串.h"
INT8 寻找字符串(const char* 目标, const char* const* 范围, UINT8 范围长度)
{
	for (UINT8 位置 = 0; 位置 < 范围长度; ++位置)
		if (!strcmp(目标, 范围[位置]))
			return 位置;
	return -1;
}