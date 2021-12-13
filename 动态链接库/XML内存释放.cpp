#include "pch.h"
#include "XML内存释放.h"
#include "pugixml.hpp"
using namespace pugi;
void XML内存释放(const char* 指针)
{
	static const deallocation_function 释放函数 = get_memory_deallocation_function();
	释放函数((void*)指针);
}