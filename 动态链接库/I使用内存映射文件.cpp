#include "pch.h"
#include "I使用内存映射文件.h"
I使用内存映射文件::~I使用内存映射文件()noexcept
{
	CloseHandle(FileMappingObject);
	CloseHandle(File);
}