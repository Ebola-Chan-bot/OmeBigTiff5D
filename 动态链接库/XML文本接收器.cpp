#include "pch.h"
#include "XML文本接收器.h"
void XML文本接收器::write(const void* data, size_t size)
{
	缓存.append((char*)data, size);
}