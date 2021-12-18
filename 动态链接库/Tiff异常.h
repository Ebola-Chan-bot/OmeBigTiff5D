#pragma once
#include "Tiff异常类型.h"
struct Tiff异常
{
	const Tiff异常类型 类型;
	const char* const 错误消息;
	Tiff异常(Tiff异常类型 类型, const char* 错误消息);
};