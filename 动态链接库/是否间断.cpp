#include "pch.h"
#include "是否间断.h"
template <NumberType T>
bool 是否间断(const T* Range, UINT32 Size)
{
	const T* const 尾 = Range + Size;
	T 上个 = *Range;
	while (++Range < 尾)
		if (*Range != ++上个)
			return false;
	return true;
}
template bool 是否间断(const UINT32* Range, UINT32 Size);
template bool 是否间断(const UINT64* Range, UINT32 Size);