#pragma once
class I使用内存映射文件
{
protected:
	HANDLE File;
	HANDLE FileMappingObject;
	virtual ~I使用内存映射文件()noexcept;
};