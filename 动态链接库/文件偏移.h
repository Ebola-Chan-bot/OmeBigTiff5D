#pragma once
#include "NumberType.h"
#pragma pack(4)
template <NumberType 偏移类型,typename 对象类型>
struct 文件偏移
{
	偏移类型 偏移;
	对象类型* 取指针(const char* 基地址) const
	{
		return (对象类型*)(基地址 + 偏移);
	}
	偏移类型 operator+(偏移类型 增量)const
	{
		return 偏移 + 增量;
	}
	文件偏移(const 偏移类型& 偏移):偏移(偏移){}
	bool operator<(偏移类型 偏移)
	{
		return 文件偏移::偏移 < 偏移;
	}
};
#pragma pack()