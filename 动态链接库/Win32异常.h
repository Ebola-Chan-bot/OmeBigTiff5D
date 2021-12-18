#pragma once
struct Win32异常
{
	const DWORD 错误代码;
	const char* const 错误消息;
	Win32异常(DWORD 错误代码, const char* 错误消息);
};