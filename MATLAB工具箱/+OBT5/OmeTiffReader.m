classdef OmeTiffReader<OBT5.TiffReader
	%支持OME规范的Tiff读取器
	methods(Access=protected)
		function obj = OmeTiffReader(Pointer)
			obj@OBT5.TiffReader(Pointer);
		end
		function ReleasePointer(obj)
			%子类重写此函数可避免重复释放指针，delete无法被重写。
			MexInterface(uint8(OBT5.Internal.ApiCode.DestroyOmeTiffReader),obj.Pointer);
		end
	end
	methods(Static)		
		function obj = Create(FilePath,varargin)
			% 创建类对象
			%% 语法
			% obj=OmeTiffReader.Create(FilePath);
			% obj=OmeTiffReader.Create(FilePath,Try);
			%% 参数说明
			% FilePath，文件路径
			% Try(1,1)logical=false，若设为true，将假定指定的文件可能不符合格式规范，如果读取出错，将输出更详细的调试信息。
			%% 返回值
			% obj(1,1)OBT5.OmeTiffReader
			%% 用例
			% obj=OmeTiffReader.Create('D:\Image.tif');
			obj=OBT5.OmeTiffReader(CheckPos(MexInterface(uint8(OBT5.Internal.ApiCode.CreateOmeTiffReader),FilePath,varargin{:})));
		end
	end
	methods
		function Name=FileName(obj)
			Name=MexInterface(uint8(OBT5.Internal.ApiCode.FileName),obj.Pointer);
		end
		function Size=SizeC(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeC),obj.Pointer);
		end
		function Size=SizeZ(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeZ),obj.Pointer);
		end
		function Size=SizeT(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeT),obj.Pointer);
		end
		function Order=DimensionOrder(obj)
			Order=OBT5.DimensionOrder(MexInterface(uint8(OBT5.Internal.ApiCode.DimensionOrder),obj.Pointer));
		end
		function Description=ImageDescription(obj)
			Description=MexInterface(uint8(OBT5.Internal.ApiCode.ImageDescription),obj.Pointer);
		end
		function Color=ChannelColor(obj,varargin)
			%获取全部或指定通道的颜色
			%返回的每个颜色值为uint32类型，可通过typecast函数转换为ABGR分量。例如如果返回颜色值16711935(0xff00ff)，则可计算如下：
			% >>typecast(0xff00ff,'uint8')
			% ans =
			% 1×4 uint8 行向量
			% 255     0   255     0
			% 4个uint8值依次为该颜色的不透明度(Alpha)=255、蓝色分量(Blue)=0、绿色分量(Green)=255、红色分量(Red)=0
			%% 语法
			% Colors=obj.ChannelColor; %获取所有通道的颜色
			% Color=obj.ChannelColor(Channel); %获取指定通道的颜色
			%% 可选参数
			% Channel(1,1)uint8，要获取哪个通道的颜色？
			%% 返回值
			% Color(:,1)uint32，如果指定了Channel参数则为标量，表示指定通道的颜色；否则为列向量，依次排列所有通道的颜色。
			Color=MexInterface(uint8(OBT5.Internal.ApiCode.ChannelColor),obj.Pointer,varargin{:});
		end
		function Pixels=ReadPixels5D(obj,varargin)
			%根据指定的XYCZT范围读入像素值
			%% 语法

			%#像数组切片一样读入指定位置的像素值
			%Pixels=obj.ReadPixels3D([XRange][,YRange][,Range3][,Range4][,Range5]);
			%5个范围参数均可省略代表顺序读入该维度全长，或者用[]表示读入全长，例如
			%Pixels=obj.ReadPixels3D([],0:99,1:2:100,100:-2:0);

			%#像素值不返回MATLAB，而是直接写出到指定的内存指针。可与上述语法组合使用
			%obj.ReadPixels3D(ToPointer,___);

			%#按照指定的维度顺序和切片方式返回数组
			%Pixels=obj.ReadPixels3D(Dimension1=Range1,Dimension2=Range2,…);
			%例如：
			%Pixels=obj.ReadPixels3D(T=[],X=10:-1:0,Z=0:1,C=[]);
			%上述代码读入T和C轴全部，X轴从10开始递减到0的反转，Z轴读入0:1，Y轴读入全部，输出数组的维度顺序是TXZCY。使用该语法时，不支持写出到内存指针。
			%% 参数说明
			%Pixels(:,:,:)，返回的像素数组，数据类型与PixelType一致。
			%XRange, YRange(1,:)uint16，XY像素值范围，用向量表示依次读入哪些位置。不指定或指定[]表示读入全部。
			%Range3, Range4, Range5(1,:)uint16，CZT像素值范围，用向量表示依次读入哪些位置。不指定或指定[]表示读入全部。3、4、5对应的CZT顺序取决于文件本身的DimensionOrder。
			%ToPointer(1,1)uint64，要写出到的内存指针，这个指针应当来自OmeBigTiff5D.PixelPointer5D函数。指定该参数时，将不返回Pixels。
			%Dimension1, Dimension2, …，可用的维度包括XYCZT。不同于一般的无序名称值参数，此处指定名称值参数的顺序将决定返回数组的维度顺序。
			%Range1, Range2, …，对应维度的读入范围。可用[]表示读入该维度全部。
			Pixels=obj.ReadPixels(varargin,'XYCZT',OBT5.Internal.ApiCode.ReadPixels5D);
		end
		function Pointer=PixelPointer5D(obj,options)
			%返回指定位置的只读像素指针
			%不能使用此方法返回的指针进行跨IFD对拷。只有OmeBigTiff5D对象返回的指针才支持跨IFD对拷。
			%% 名称值参数
			%X, Y, T(1,1)uint16=0，指针要指向的XYT
			%C, Z(1,1)uint8=0，指针要指向的CZ位置
			%注意索引是从0开始，不同于MATLAB的一般索引规范
			%% 返回值
			%Pointer(1,1)uint64，指向指定像素位置的内存指针。此方法返回的像素指针指向只读内存段，请勿对其进行写入操作，可以用作OmeBigTiff5D.WritePixels5D的指针参数，实现直接拷贝。但因为标准Tiff的像素数据在IFD之间不是连续排列的，此方法不能可靠地连续拷贝多个IFD的像素值。
			arguments
				obj(1,1)OBT5.TiffReader
				options.X(1,1)uint16=0
				options.Y(1,1)uint16=0
				options.C(1,1)uint8=0
				options.Z(1,1)uint8=0
				options.T(1,1)uint16=0
			end
			Pointer=MexInterface(uint8(OBT5.Internal.ApiCode.PixelPointer5D),obj.Pointer,options.X,options.Y,options.C,options.Z,options.T);
		end
	end
end