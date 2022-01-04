#pragma once
#include "NumberType.h"
template <NumberType 偏移类型,typename 对象类型>
struct 文件偏移
{
	偏移类型 偏移;
	对象类型* 取指针(const BYTE* 基地址) const
	{
		return (对象类型*)(基地址 + 偏移);
	}
	文件偏移& operator+=(偏移类型 增量)
	{
		偏移 += 增量;
		return *this;
	}
	偏移类型 operator+(偏移类型 增量)const
	{
		return 偏移 + 增量;
	}
	bool operator<(偏移类型 偏移)
	{
		return 文件偏移::偏移 < 偏移;
	}
};