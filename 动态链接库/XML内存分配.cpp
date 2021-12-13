#include "pch.h"
#include "XML内存分配.h"
#include "pugixml.hpp"
using namespace pugi;
char* XML内存分配(size_t Size)
{
	static const allocation_function 分配函数 = get_memory_allocation_function();
	return (char*)分配函数(Size);
}