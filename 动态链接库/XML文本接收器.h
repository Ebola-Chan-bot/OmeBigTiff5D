#pragma once
#include "pugixml.hpp"
using namespace pugi;
struct XML文本接收器:xml_writer
{
	std::string 缓存;
	void write(const void* data, size_t size);
};