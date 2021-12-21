#pragma once
#include "结果分类.h"
#include "Tiff异常类型.h"
#include "XML解析结果.h"
struct 尝试结果
{
	结果分类 结果;
	union
	{
		Tiff异常类型 异常类型;
		DWORD 错误代码;
		XML解析结果 XML异常;
	};
	const char* 错误消息;
};