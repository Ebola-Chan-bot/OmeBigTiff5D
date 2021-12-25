classdef TiffReader<handle
	%基础Tiff读取器。此类不完全支持完整的TIFF规范文件，要求所有IFD都只有唯一的Strip，且所有IFD高度相等，宽度也相等，位深度也相等。
	properties(GetAccess=protected,SetAccess=immutable)
		Pointer(1,1)uint64
	end
	methods(Access=protected)
		function obj = TiffReader(Pointer)
			obj.Pointer=Pointer;
		end
		function ReleasePointer(obj)
			%子类重写此函数可避免重复释放指针，delete无法被重写。
			MexInterface(uint8(OBT5.Internal.ApiCode.DestroyTiffReader),obj.Pointer);
		end
	end
	methods(Static)
		function obj=Create(FilePath,varargin)
			% 创建类对象
			%% 语法
			% obj=TiffReader.Create(FilePath);
			% obj=TiffReader.Create(FilePath,Try);
			%% 参数说明
			% FilePath，文件路径
			% Try(1,1)logical=false，若设为true，将假定指定的文件可能不符合格式规范，如果读取出错，将输出更详细的调试信息，且不会导致MATLAB崩溃。
			%% 返回值
			% obj(1,1)OBT5.TiffReader
			%% 用例
			% obj=TiffReader.Create('D:\Image.tif');
			obj=OBT5.TiffReader(CheckPos(MexInterface(uint8(OBT5.Internal.ApiCode.CreateTiffReader),FilePath,varargin{:})));
		end
	end
	methods
		function delete(obj)
			obj.ReleasePointer;
		end
		function Size=SizeX(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeX),obj.Pointer);
		end
		function Size=SizeY(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeY),obj.Pointer);
		end
		function Size=SizeI(obj)
			Size=MexInterface(uint8(OBT5.Internal.ApiCode.SizeI),obj.Pointer);
		end
		function Type=PixelType(obj)
			Type=OBT5.PixelType(MexInterface(uint8(OBT5.Internal.ApiCode.PixelType),obj.Pointer));
		end
		function Bytes=BytesPerSample(obj)
			Bytes=MexInterface(uint8(OBT5.Internal.ApiCode.BytesPerSample),obj.Pointer);
		end
		function Pixels=ReadPixels3D(obj,options)
			%根据指定的XYI范围读入像素值
			%注意，读入的单帧图面维度顺序是XY，而imshow按照维度顺序YX显示图像，因此需要转置才能正确显示。
			%% 名称值参数
			%X, Y, I (1,:)uint64，各维度的读入像素范围。默认依次读入该维度的全部。注意索引是从0开始，不同于MATLAB的索引规范。
			%ToPointer(1,1)uint64=0，如果设置非0值，将把像素数据拷入对应的内存指针位置，而不返回到MATLAB。例如可以从OmeBigTiff5D.PixelPointer5D返回一个内存指针。
			%% 返回值
			%Pixels(:,:,:)，像素数据，维度顺序XYI，数据类型与PixelType一致。如果指定了ToPointer参数，将不设置该返回值。
			%% 用例
			%Pixels=obj.ReadPixels3D(Y=511:-1:0,I=0:2:100); %读入上下翻转、偶数序号的IFD图像。
			arguments
				obj(1,1)OBT5.TiffReader
				options.X(1,:)uint64=uint64.empty
				options.Y(1,:)uint64=uint64.empty
				options.I(1,:)uint64=uint64.empty
				options.ToPointer(1,1)uint64=0
			end
			if options.ToPointer
				MexInterface(uint8(OBT5.Internal.ApiCode.ReadPixels3D),obj.Pointer,options.X,options.Y,options.I,options.ToPointer);
			else
				Pixels=MexInterface(uint8(OBT5.Internal.ApiCode.ReadPixels3D),obj.Pointer,options.X,options.Y,options.I);
			end
		end
		function Pointer=PixelPointer3D(obj,options)
			%返回指定位置的只读像素指针
			%不能使用此方法返回的指针进行跨IFD对拷。只有OBT5对象返回的指针才支持跨IFD对拷。
			%% 名称值参数
			%X, Y(1,1)uint16=0，指针要指向的XY位置
			%I(1,1)uint32=0，指针要指向的IFD序号
			%注意索引是从0开始，不同于MATLAB的一般索引规范
			%% 返回值
			%Pointer(1,1)uint64，指向指定像素位置的内存指针。此方法返回的像素指针指向只读内存段，请勿对其进行写入操作，可以用作OmeBigTiff5D.WritePixels5D的指针参数，实现直接拷贝。但因为标准Tiff的像素数据在IFD之间不是连续排列的，此方法不能可靠地连续拷贝多个IFD的像素值。
			arguments
				obj(1,1)OBT5.TiffReader
				options.X(1,1)uint16=0
				options.Y(1,1)uint16=0
				options.I(1,1)uint32=0
			end
			Pointer=MexInterface(uint8(OBT5.Internal.ApiCode.PixelPointer3D),obj.Pointer,options.X,options.Y,options.I);
		end
	end
end