#include "pch.h"
#include "CppUnitTest.h"
#include "IOmeBigTiff5D.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace 单元测试
{
	TEST_CLASS(单元测试)
	{
	private:
		static constexpr WCHAR Tiff路径[] = LR"(C:\Users\vhtmf\source\repos\OmeBigTiff5D\测试用例\测试图像.tif)";
		static constexpr WCHAR OmeBigTiff5D路径[] = LR"(C:\Users\vhtmf\source\repos\OmeBigTiff5D\测试用例\测试图像.5D.tif)";
		static constexpr WCHAR 单张路径[] = LR"(C:\Users\vhtmf\source\repos\OmeBigTiff5D\单元测试\0x80000000.tif)";
	public:
		TEST_METHOD(Tiff转OmeBigTiff5D)
		{
			ITiffReader* Tiff读入器 = 创建TiffReader(Tiff路径);
			IOmeBigTiff5D* OmeBigTiff5D写出器 = 创建OmeBigTiff5D();
			OmeBigTiff5D写出器->覆盖创建(OmeBigTiff5D路径, Tiff读入器->SizeX(), Tiff读入器->SizeY(), 1, 2, Tiff读入器->SizeI() / 2, 维度顺序::XYCZT, 像素类型::UINT16);
			Tiff读入器->读入像素3D(0, 0, 0, nullptr, nullptr, nullptr, OmeBigTiff5D写出器->变内部像素指针常(0, 0, 0));
			销毁TiffReader(Tiff读入器);
			销毁OmeBigTiff5D(OmeBigTiff5D写出器);
		}
		TEST_METHOD(OmeTiff转OmeBigTiff5D)
		{
			IOmeTiffReader* Tiff读入器 = 创建OmeTiffReader(Tiff路径);
			IOmeBigTiff5D* OmeBigTiff5D写出器 = 创建OmeBigTiff5D();
			尝试结果 结果 = OmeBigTiff5D写出器->覆盖创建(OmeBigTiff5D路径, Tiff读入器->ImageDescription());
			颜色 Colors[1];
			Colors[0].整数值 = 0xff00ffff;
			OmeBigTiff5D写出器->修改基本参数(0, 0, 0, 1, 0, 维度顺序::缺省, 像素类型::缺省, Colors, nullptr);
			UINT64 ZRange[] = { 0 };
			Tiff读入器->读入像素5D(0, 0, 0, 1, 0, nullptr, nullptr, nullptr, ZRange, nullptr, OmeBigTiff5D写出器->变内部像素指针常(0, 0, 0, 0, 0));
			销毁OmeTiffReader(Tiff读入器);
			销毁OmeBigTiff5D(OmeBigTiff5D写出器);
		}
		TEST_METHOD(ZC转置并变色)
		{
			IOmeBigTiff5D* const 文件 = 创建OmeBigTiff5D();
			文件->打开现存(OmeBigTiff5D路径);
			constexpr UINT32 通道颜色[] = { 0x00ff00ff,0x0000ffff };
			文件->修改基本参数(0, 0, 2, 1, 0, 维度顺序::缺省, 像素类型::缺省, (颜色*)通道颜色, nullptr);
			销毁OmeBigTiff5D(文件);
		}
		TEST_METHOD(ImageJBug)
		{
			IOmeTiffReader* Tiff读入器 = 创建OmeTiffReader(Tiff路径);
			IOmeBigTiff5D* OmeBigTiff5D写出器 = 创建OmeBigTiff5D();
			尝试结果 结果 = OmeBigTiff5D写出器->覆盖创建(OmeBigTiff5D路径, Tiff读入器->ImageDescription());
			颜色 Color;
			Color.整数值 = 0x00ff0000;
			OmeBigTiff5D写出器->写ChannelColor(Color, 0);
			Tiff读入器->读入像素3D(0, 0, 0, nullptr, nullptr, nullptr, OmeBigTiff5D写出器->变内部像素指针常(0, 0, 0));
			销毁OmeTiffReader(Tiff读入器);
			销毁OmeBigTiff5D(OmeBigTiff5D写出器);
		}
	};
}
