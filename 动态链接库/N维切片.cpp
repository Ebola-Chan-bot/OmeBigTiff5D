#include "pch.h"
#include "N维切片.h"
#include "N维切片参数.h"
void N维切片(UINT64 单位尺寸, UINT8 维数, UINT32* 各维尺寸, UINT32* 下标长度, UINT64** 下标, const BYTE* 源, BYTE* 目标, bool 切源)
{
	偏移向量 偏移;
	UINT64 段长度;
	N维切片参数(单位尺寸, 维数, 各维尺寸, 下标长度, 下标, 偏移, 段长度);
	if (切源)
		for (UINT64 O : 偏移)
		{
			memcpy(目标, 源 + O, 段长度);
			目标 += 段长度;
		}
	else
		for (UINT64 O : 偏移)
		{
			memcpy(目标 + O, 源, 段长度);
			源 += 段长度;
		}
}