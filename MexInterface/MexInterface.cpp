#include "MexInterface.h"
#include <Windows.h>
#include "IOmeBigTiff5D.h"
//这一部分要与MATLAB端共享，因此必须全英文
using namespace matlab::mex;
static ArrayFactory 数组工厂;
template <typename T>
T Get标量(Array& 输入数组)
{
#ifdef DEBUG
	const T 返回值 = TypedArray<T>(std::move(输入数组))[0];
	return 返回值;
#else
	return TypedArray<T>(std::move(输入数组))[0];
#endif
}
template <typename T>
T Get数值(Array& 输入数组)
{
	switch (输入数组.getType())
	{
	case ArrayType::UINT8:
		return T(Get标量<UINT8>(输入数组));
	case ArrayType::UINT16:
		return T(Get标量<UINT16>(输入数组));
	case ArrayType::UINT32:
		return T(Get标量<UINT32>(输入数组));
	case ArrayType::UINT64:
		return T(Get标量<UINT64>(输入数组));
	case ArrayType::INT8:
		return T(Get标量<INT8>(输入数组));
	case ArrayType::INT16:
		return T(Get标量<INT16>(输入数组));
	case ArrayType::INT32:
		return T(Get标量<INT32>(输入数组));
	case ArrayType::INT64:
		return T(Get标量<INT64>(输入数组));
	case ArrayType::SINGLE:
		return T(Get标量<float>(输入数组));
	case ArrayType::DOUBLE:
		return T(Get标量<double>(输入数组));
	}
}
template <typename T>
T* Get对象指针(Array& 输入数组)
{
	return (T*)Get标量<UINT64>(输入数组);
}
String GetString(Array& 输入数组)
{
	switch (输入数组.getType())
	{
	case ArrayType::CHAR:
		return CharArray(std::move(输入数组)).toUTF16();
	case ArrayType::MATLAB_STRING:
		return Get标量<MATLABString>(输入数组);
	}
}
//调用方负责释放
char* GetUtf8(Array& 输入数组)
{
	const String MatlabString = GetString(输入数组);
	const size_t 缓冲区大小 = MatlabString.size() * 3;
	char* const Utf8String = (LPSTR)malloc(缓冲区大小);
	WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)MatlabString.c_str(), -1, Utf8String, 缓冲区大小, NULL, NULL);
	return Utf8String;
}
//调用方负责释放，空数组将返回nullptr
char* GetNullableUtf8(Array& 输入数组)
{
	if (输入数组.isEmpty())
		return nullptr;
	else
		return GetUtf8(输入数组);
}
CharArray GetCharArray(const char* 字符串)
{
	const size_t 缓冲区大小 = strlen(字符串) + 1;
	wchar_t* const 宽字符串 = (wchar_t*)malloc(sizeof(wchar_t) * 缓冲区大小);
	MultiByteToWideChar(CP_UTF8, 0, 字符串, 缓冲区大小, 宽字符串, 缓冲区大小);
	CharArray 字符数组 = 数组工厂.createCharArray((char16_t*)宽字符串);
	free(宽字符串);
	return std::move(字符数组);
}
//将数组返回为指针，空数组将返回nullptr。指针指向MATLAB内部受保护的内存，调用方不应当free之
template <typename T>
T* Get数组指针(Array& 数组)
{
	if (数组.isEmpty())
		return nullptr;
	else
		return TypedArray<T>(std::move(数组)).release().get();
}
#define 位置3D XSize, YSize, ISize, XRange, YRange, IRange
template <typename T>
TypedArray<T> 读入像素3D(ITiffReader* 对象指针, UINT16 XSize, UINT16 YSize, UINT32 ISize, UINT64* XRange, UINT64* YRange, UINT64* IRange)
{
	buffer_ptr_t<T> 缓冲区 = 数组工厂.createBuffer<T>(UINT64(XSize) * YSize * ISize);
	对象指针->读入像素3D(位置3D, (BYTE*)缓冲区.get());
	return std::move(数组工厂.createArrayFromBuffer({ XSize,YSize,ISize }, std::move(缓冲区)));
}
#define 位置5D XSize, YSize, CSize,ZSize,TSize, XRange, YRange, CRange,ZRange,TRange
template <typename T>
TypedArray<T> 读入像素5D(IOmeTiffReader* 对象指针, UINT16 XSize, UINT16 YSize, UINT8 CSize,UINT8 ZSize,UINT16 TSize, UINT64* XRange, UINT64* YRange, UINT64* CRange,UINT64* ZRange,UINT64* TRange)
{
	buffer_ptr_t<T> 缓冲区 = 数组工厂.createBuffer<T>(UINT64(XSize) * YSize * CSize * ZSize * TSize);
	对象指针->读入像素5D(位置5D, (BYTE*)缓冲区.get());
	switch (对象指针->DimensionOrder())
	{
	case 维度顺序::XYCZT:
		return std::move(数组工厂.createArrayFromBuffer({ XSize,YSize,CSize,ZSize,TSize }, std::move(缓冲区)));
	case 维度顺序::XYCTZ:
		return std::move(数组工厂.createArrayFromBuffer({ XSize,YSize,CSize,TSize,ZSize }, std::move(缓冲区)));
	case 维度顺序::XYZCT:
		return std::move(数组工厂.createArrayFromBuffer({ XSize,YSize,ZSize,CSize,TSize }, std::move(缓冲区)));
	case 维度顺序::XYZTC:
		return std::move(数组工厂.createArrayFromBuffer({ XSize,YSize,ZSize,TSize,CSize }, std::move(缓冲区)));
	case 维度顺序::XYTCZ:
		return std::move(数组工厂.createArrayFromBuffer({ XSize,YSize,TSize,CSize,ZSize }, std::move(缓冲区)));
	case 维度顺序::XYTZC:
		return std::move(数组工厂.createArrayFromBuffer({ XSize,YSize,TSize,ZSize,CSize }, std::move(缓冲区)));
	}
}
Array 处理结果(const 尝试结果& 结果, ITiffReader* 对象指针)
{
	if (结果.结果 == 结果分类::成功)
		return 数组工厂.createScalar(size_t(对象指针));
	else
	{
		StructArray 异常结构 = 数组工厂.createStructArray({ 1,1 }, { "Type","Code","Message" });
		//异常结构[0]不能单独拎出来做多次字段编辑
		异常结构[0]["Type"] = 数组工厂.createScalar(UINT8(结果.结果));
		switch (结果.结果)
		{
		case 结果分类::Tiff异常:
			异常结构[0]["Code"] = 数组工厂.createScalar(UINT32(结果.异常类型));
			break;
		case 结果分类::Win32异常:
			异常结构[0]["Code"] = 数组工厂.createScalar(UINT32(结果.错误代码));
			break;
		case 结果分类::XML异常:
			异常结构[0]["Code"] = 数组工厂.createScalar(UINT32(结果.XML异常));
			break;
		}
		异常结构[0]["Message"] = GetCharArray(结果.错误消息);
		return std::move(异常结构);
	}
};
#define API声明(函数名) void 函数名(ArgumentList& outputs,ArgumentList& inputs)
API声明(CreateTiffReader)
{
	const String 文件路径 = GetString(inputs[1]);
	if (inputs.size() > 2 && Get标量<bool>(inputs[2]))
	{
		尝试结果 结果;
		ITiffReader* const 对象指针 = 尝试创建TiffReader(LPCWSTR(文件路径.c_str()), 结果);
		outputs[0] = 处理结果(结果, 对象指针);
	}
	else
		outputs[0] = 数组工厂.createScalar(UINT64(创建TiffReader(LPCWSTR(文件路径.c_str()))));
}
API声明(DestroyTiffReader)
{
	销毁TiffReader(Get对象指针<ITiffReader>(inputs[1]));
}
API声明(CreateOmeTiffReader)
{
	const String 文件路径 = GetString(inputs[1]);
	if (inputs.size() > 2 && Get标量<bool>(inputs[2]))
	{
		尝试结果 结果;
		ITiffReader* const 对象指针 = 尝试创建OmeTiffReader(LPCWSTR(文件路径.c_str()), 结果);
		outputs[0] = 处理结果(结果, 对象指针);
	}
	else
		outputs[0] = 数组工厂.createScalar(UINT64(创建OmeTiffReader(LPCWSTR(文件路径.c_str()))));
}
API声明(DestroyOmeTiffReader)
{
	销毁OmeTiffReader(Get对象指针<IOmeTiffReader>(inputs[1]));
}
#define Create5D声明(函数名) void 函数名(ArgumentList& outputs,ArgumentList& inputs)
Create5D声明(打开现存)
{
	IOmeBigTiff5D* const 对象指针 = 创建OmeBigTiff5D();
	const String 文件路径 = GetString(inputs[2]);
	对象指针->打开现存(LPCWSTR(文件路径.c_str()));
	outputs[0]=数组工厂.createScalar(UINT64(对象指针));
}
Create5D声明(尝试打开)
{
	IOmeBigTiff5D* const 对象指针 = 创建OmeBigTiff5D();
	const String 文件路径 = GetString(inputs[2]);
	outputs[0]= 处理结果(对象指针->尝试打开(LPCWSTR(文件路径.c_str())), 对象指针);
}
Create5D声明(打开或创建)
{
	IOmeBigTiff5D* const 对象指针 = 创建OmeBigTiff5D();
	const String 文件路径 = GetString(inputs[2]);
	if (inputs.size() < 5)
	{
		char* 图像描述 = GetUtf8(inputs[3]);
		outputs[1] = 数组工厂.createScalar(对象指针->打开或创建(LPCWSTR(文件路径.c_str()), 图像描述));
		free(图像描述);
	}
	else
		outputs[1] = 数组工厂.createScalar(对象指针->打开或创建(LPCWSTR(文件路径.c_str()), Get数值<UINT16>(inputs[3]), Get数值<UINT16>(inputs[4]), Get数值<UINT8>(inputs[5]), Get数值<UINT8>(inputs[6]), Get数值<UINT16>(inputs[7]), Get数值<维度顺序>(inputs[8]), Get数值<像素类型>(inputs[9])));
	outputs[0]= 数组工厂.createScalar(UINT64(对象指针));
}
Create5D声明(覆盖创建)
{
	IOmeBigTiff5D* const 对象指针 = 创建OmeBigTiff5D();
	const String 文件路径 = GetString(inputs[2]);
	if (inputs.size() < 5)
	{
		char* 图像描述 = GetUtf8(inputs[3]);
		对象指针->覆盖创建(LPCWSTR(文件路径.c_str()), 图像描述);
		free(图像描述);
	}
	else
		对象指针->覆盖创建(LPCWSTR(文件路径.c_str()), Get数值<UINT16>(inputs[3]), Get数值<UINT16>(inputs[4]), Get数值<UINT8>(inputs[5]), Get数值<UINT8>(inputs[6]), Get数值<UINT16>(inputs[7]), Get数值<维度顺序>(inputs[8]), Get数值<像素类型>(inputs[9]));
	outputs[0]= 数组工厂.createScalar(UINT64(对象指针));
}
API声明(CreateOmeBigTiff5D)
{
	constexpr void(*(Create5D方法[]))(ArgumentList&,ArgumentList&) = { 打开现存,尝试打开,打开或创建,覆盖创建 };
	Create5D方法[Get数值<UINT8>(inputs[1])](outputs,inputs);
}
API声明(DestroyOmeBigTiff5D)
{
	销毁OmeBigTiff5D(Get对象指针<IOmeBigTiff5D>(inputs[1]));
}
API声明(FileName)
{
	if (inputs.size() > 2)
	{
		char* 文件名 = GetUtf8(inputs[2]);
		Get对象指针<IOmeBigTiff5D>(inputs[1])->FileName(文件名);
		free(文件名);
	}
	else
		outputs[0] = GetCharArray(Get对象指针<IOmeTiffReader>(inputs[1])->FileName());
}
#define 维度属性(维度,位数,可读类) API声明(Size##维度)\
{\
	if (inputs.size() > 2)\
		Get对象指针<IOmeBigTiff5D>(inputs[1])->Size##维度(Get数值<UINT##位数>(inputs[2]));\
	else\
		outputs[0] = 数组工厂.createScalar(Get对象指针<可读类>(inputs[1])->Size##维度());\
}
维度属性(X, 16, ITiffReader);
维度属性(Y, 16, ITiffReader);
维度属性(C, 8, IOmeTiffReader);
维度属性(Z, 8, IOmeTiffReader);
维度属性(T, 16, IOmeTiffReader);
API声明(SizeI)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<ITiffReader>(inputs[1])->SizeI());
}
API声明(DimensionOrder)
{
	if (inputs.size() > 2)
		Get对象指针<IOmeBigTiff5D>(inputs[1])->DimensionOrder(Get数值<维度顺序>(inputs[2]));
	else
		outputs[0] = 数组工厂.createScalar(UINT8(Get对象指针<IOmeTiffReader>(inputs[1])->DimensionOrder()));
}
API声明(PixelType)
{
	if (inputs.size() > 2)
		Get对象指针<IOmeBigTiff5D>(inputs[1])->PixelType(Get数值<像素类型>(inputs[2]));
	else
		outputs[0] = 数组工厂.createScalar(UINT8(Get对象指针<IOmeTiffReader>(inputs[1])->PixelType()));
}
API声明(ChannelColor)
{
	switch (inputs.size())
	{
	case 2:
	{
		const IOmeTiffReader* const 对象指针 = Get对象指针<IOmeTiffReader>(inputs[1]);
		const UINT8 颜色通道数 = 对象指针->SizeC();
		buffer_ptr_t<UINT32> 颜色缓冲区 = 数组工厂.createBuffer<UINT32>(颜色通道数);
		对象指针->读ChannelColor((颜色*)颜色缓冲区.get());
		outputs[0] = 数组工厂.createArrayFromBuffer({ 颜色通道数,1 }, std::move(颜色缓冲区));
		break; 
	}
	case 3:
		if (inputs[2].getType() == ArrayType::UINT32)
		{
			const buffer_ptr_t<UINT32> 数组缓冲 = TypedArray<UINT32>(std::move(inputs[2])).release();
			Get对象指针<IOmeBigTiff5D>(inputs[1])->写ChannelColor((颜色*)数组缓冲.get());
		}
		else
			outputs[0] = 数组工厂.createScalar(Get对象指针<IOmeTiffReader>(inputs[1])->读ChannelColor(Get数值<UINT8>(inputs[2])).整数值);
		break;
	case 4:
		Get对象指针<IOmeBigTiff5D>(inputs[1])->写ChannelColor(颜色{ .整数值 = Get标量<UINT32>(inputs[2]) }, Get数值<UINT8>(inputs[3]));
		break;
	}
}
API声明(BytesPerSample)
{
	outputs[0] = 数组工厂.createScalar(Get对象指针<IOmeTiffReader>(inputs[1])->BytesPerSample());
}
API声明(ImageDescription)
{
	if (inputs.size() > 2)
	{
		char* 图像描述 = GetUtf8(inputs[2]);
		Get对象指针<IOmeBigTiff5D>(inputs[1])->ImageDescription(图像描述);
		free(图像描述);
	}
	else
#ifdef DEBUG
	{
		const IOmeTiffReader* const 对象指针 = Get对象指针<IOmeTiffReader>(inputs[1]);
		outputs[0] = GetCharArray(对象指针->ImageDescription());
	}
#else
		outputs[0] = GetCharArray(Get对象指针<IOmeTiffReader>(inputs[1])->ImageDescription());
#endif
}
#define 维度SR(参数,位数,维度) UINT##位数 维度##Size=inputs[参数].getNumberOfElements();\
UINT64* const 维度##Range=Get数组指针<UINT64>(inputs[参数]);
#define Size默认值(维度) if (!维度##Range)\
维度##Size = 对象指针->Size##维度();
API声明(ReadPixels3D)
{
	ITiffReader* const 对象指针 = Get对象指针<ITiffReader>(inputs[1]);
	维度SR(2, 16, X);
	维度SR(3, 16, Y);
	维度SR(4, 32, I);
	if (inputs.size() > 5)
	{
		BYTE* const 目标指针 = Get对象指针<BYTE>(inputs[5]);
		if (目标指针)
		{
			对象指针->读入像素3D(位置3D, 目标指针);
			return;
		}
	}
	//只要保证Range是nullptr，Size就会被忽略
	Size默认值(X);
	Size默认值(Y);
	Size默认值(I);
	switch (对象指针->PixelType())
	{
	case 像素类型::DOUBLE:
		outputs[0] = 读入像素3D<double>(对象指针, 位置3D);
		break;
	case 像素类型::FLOAT:
		outputs[0] = 读入像素3D<float>(对象指针, 位置3D);
		break;
#define 读入Type3D(Type) case 像素类型::Type:\
			outputs[0] = 读入像素3D<Type>(对象指针, 位置3D);\
			break;
		读入Type3D(UINT8);
		读入Type3D(UINT16);
		读入Type3D(UINT32);
		读入Type3D(INT8);
		读入Type3D(INT16);
		读入Type3D(INT32);
	}
}
API声明(ReadPixels5D)
{
	IOmeTiffReader* const 对象指针 = Get对象指针<IOmeTiffReader>(inputs[1]);
	维度SR(2, 16, X);
	维度SR(3, 16, Y);
	维度SR(4, 8, C);
	维度SR(5, 8, Z);
	维度SR(6, 16, T);
	if (inputs.size() > 7)
	{
		BYTE* const 目标指针 = Get对象指针<BYTE>(inputs[7]);
		if (目标指针)
		{
			对象指针->读入像素5D(位置5D, 目标指针);
			return;
		}
	}
	//只要保证Range是nullptr，Size就会被忽略
	Size默认值(X);
	Size默认值(Y);
	Size默认值(C);
	Size默认值(Z);
	Size默认值(T);
	switch (对象指针->PixelType())
	{
	case 像素类型::DOUBLE:
		outputs[0] = 读入像素5D<double>(对象指针, 位置5D);
		break;
	case 像素类型::FLOAT:
		outputs[0] = 读入像素5D<float>(对象指针, 位置5D);
		break;
#define 读入Type5D(Type) case 像素类型::Type:\
			outputs[0] = 读入像素5D<Type>(对象指针, 位置5D);\
			break;
		读入Type5D(UINT8);
		读入Type5D(UINT16);
		读入Type5D(UINT32);
		读入Type5D(INT8);
		读入Type5D(INT16);
		读入Type5D(INT32);
	}
}
API声明(WritePixels5D)
{
	IOmeBigTiff5D* const 对象指针 = Get对象指针<IOmeBigTiff5D>(inputs[1]);
	维度SR(2, 16, X);
	维度SR(3, 16, Y);
	维度SR(4, 8, C);
	维度SR(5, 8, Z);
	维度SR(6, 16, T);
	switch (inputs[7].getType())
	{
#define 写出类型(类型) case ArrayType::类型:\
	{\
		const buffer_ptr_t<类型> 缓冲区 = TypedArray<类型>(std::move(inputs[7])).release();\
		对象指针->写出像素(位置5D, (BYTE*)缓冲区.get());\
		break;\
	}
		写出类型(UINT8);
		写出类型(UINT16);
		写出类型(UINT32);
		写出类型(INT8);
		写出类型(INT16);
		写出类型(INT32);
	case ArrayType::SINGLE:
	{
		const buffer_ptr_t<float> 缓冲区 = TypedArray<float>(std::move(inputs[7])).release();
		对象指针->写出像素(位置5D, (BYTE*)缓冲区.get());
		break;
	}
	case ArrayType::DOUBLE:
	{
		const buffer_ptr_t<double> 缓冲区 = TypedArray<double>(std::move(inputs[7])).release();
		对象指针->写出像素(位置5D, (BYTE*)缓冲区.get());
		break;
	}
	default:
		对象指针->写出像素(位置5D, Get对象指针<BYTE>(inputs[7]));
	}
}
API声明(PixelPointer3D)
{
	outputs[0] = 数组工厂.createScalar(UINT64(Get对象指针<ITiffReader>(inputs[1])->内部像素指针3D(Get数值<UINT16>(inputs[2]), Get数值<UINT16>(inputs[3]), Get数值<UINT32>(inputs[4]))));
}
API声明(PixelPointer5D)
{
	outputs[0] = 数组工厂.createScalar(UINT64(Get对象指针<IOmeTiffReader>(inputs[1])->内部像素指针5D(Get数值<UINT16>(inputs[2]), Get数值<UINT16>(inputs[3]), Get数值<UINT8>(inputs[4]), Get数值<UINT8>(inputs[5]), Get数值<UINT16>(inputs[6]))));
}
API声明(ModifyMultiParameters)
{
	char* const 文件名指针 = GetNullableUtf8(inputs[10]);
	颜色* const 颜色指针 = (颜色*)Get数组指针<UINT32>(inputs[9]);
	Get对象指针<IOmeBigTiff5D>(inputs[1])->修改基本参数(Get数值<UINT16>(inputs[2]), Get数值<UINT16>(inputs[3]), Get数值<UINT8>(inputs[4]), Get数值<UINT8>(inputs[5]), Get数值<UINT16>(inputs[6]), Get数值<维度顺序>(inputs[7]), Get数值<像素类型>(inputs[8]), 颜色指针, 文件名指针);
	free(文件名指针);
}
void MexFunction::operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)
{
	constexpr void (*(API[]))(ArgumentList&, ArgumentList&) =
	{
		//1个参数为创建，2个为尝试创建
		CreateTiffReader,
		DestroyTiffReader,
		CreateOmeTiffReader,
		DestroyOmeTiffReader,
		//创建方法，文件路径，基本参数
		CreateOmeBigTiff5D,
		DestroyOmeBigTiff5D,
		FileName,
		SizeX,
		SizeY,
		SizeC,
		SizeZ,
		SizeT,
		SizeI,
		DimensionOrder,
		PixelType,
		//无参数为读全部通道，1个UINT8为读特定通道，1个UINT32数组为写全部通道，(UINT8,UINT32)为写特定通道
		ChannelColor,
		BytesPerSample,
		ImageDescription,
		ReadPixels3D,
		ReadPixels5D,
		WritePixels5D,
		PixelPointer3D,
		PixelPointer5D,
		ModifyMultiParameters,
	};
#ifdef DEBUG
	ArrayType 函数Type = inputs[0].getType();
	ArrayType 属性Type = inputs[0].getType();
	const UINT8 API命令 = Get数值<UINT8>(inputs[0]);
	API[API命令](outputs, inputs);
#else
	API[Get数值<UINT8>(inputs[0])](outputs, inputs);
#endif
}