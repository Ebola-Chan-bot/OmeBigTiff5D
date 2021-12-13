#include "pch.h"
#include "下标转索引.h"
#include <numeric>
void 下标转索引(UINT64 基础权重, UINT8 维数, UINT32* 各维尺寸, UINT32* 下标长度, UINT64** 下标, 偏移向量& 索引)
{
	各维尺寸[0] *= 基础权重;
	bool* 需要释放 = (bool*)malloc(维数);
	memset(需要释放, false, 维数);
	UINT32 该维尺寸;
	for (UINT8 D = 0; D < 维数; ++D)
		if (!下标[D])
		{
			该维尺寸 = 各维尺寸[D];
			下标长度[D] = 该维尺寸;
			下标[D] = (UINT64*)malloc(sizeof(UINT64) * 该维尺寸);
			需要释放[D] = true;
			std::iota(下标[D], 下标[D] + 该维尺寸, 0);
		}
	UINT64* const 各维权重 = (UINT64*)malloc(sizeof(UINT64) * 维数);
	各维权重[0] = 基础权重;
	UINT64* const 元素重复 = (UINT64*)malloc(sizeof(UINT64) * 维数);
	元素重复[0] = 1;
	UINT64* const 序列重复 = (UINT64*)malloc(sizeof(UINT64) * 维数);
	序列重复[维数 - 1] = 1;
	for (UINT8 D = 0; D < 维数 - 1; ++D)
	{
		各维权重[D + 1] = 各维权重[D] * 各维尺寸[D];
		元素重复[D + 1] = 元素重复[D] * 下标长度[D];
	}
	for (UINT8 D = 维数 - 1; D > 0; --D)
		序列重复[D - 1] = 序列重复[D] * 下标长度[D];
	const UINT64 索引长度 = 序列重复[0] * 下标长度[0];
	索引 = 偏移向量::Zero(索引长度);
	for (UINT8 D = 0; D < 维数; ++D)
		索引 += (Map<偏移向量>(下标[D], 下标长度[D]) * 各维权重[D]).replicate(1, 元素重复[D]).transpose().reshaped().replicate(序列重复[D], 1);
	free(各维权重);
	free(元素重复);
	free(序列重复);
	for (UINT8 D = 0; D < 维数; ++D)
		if (需要释放[D])
			free(下标[D]);
	free(需要释放);
}