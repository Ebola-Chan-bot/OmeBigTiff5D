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
		function Color=ChannelColor(varargin)
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
		function Pixels=ReadPixels5D(obj,options)
			%根据指定的XYCZT范围读入像素值
			%注意，读入的单帧图面维度顺序是XY，而imshow按照维度顺序YX显示图像，因此需要转置才能正确显示。
			%% 名称值参数
			%X, Y, C, Z, T(1,:)uint64，各维度的读入像素范围。默认依次读入该维度的全部。
			%ToPointer(1,1)uint64=0，如果设置非0值，将把像素数据拷入对应的内存指针位置，而不返回到MATLAB。例如可以从OmeBigTiff5D.PixelPointer5D返回一个内存指针。使用时请注意源文件和目标文件的维度顺序是否一致。
			%% 返回值
			%Pixels(:,:,:)，像素数据，维度顺序与DimensionOrder一致，数据类型与PixelType一致。如果指定了ToPointer参数，将不设置该返回值。
			%% 用例
			%Pixels=obj.ReadPixels5D(T=1000:-1:0,Z=0); %读入第0层、时间反演的像素数据并返回给MATLAB
			arguments
				obj(1,1)OBT5.OmeTiffReader
				options.X(1,:)uint64=uint64.empty
				options.Y(1,:)uint64=uint64.empty
				options.C(1,:)uint64=uint64.empty
				options.Z(1,:)uint64=uint64.empty
				options.T(1,:)uint64=uint64.empty
				options.ToPointer(1,1)uint64=0
			end
			if options.ToPointer
				MexInterface(uint8(OBT5.Internal.ApiCode.ReadPixels5D),obj.Pointer,options.X,options.Y,options.C,options.Z,options.T,options.ToPointer);
			else
				Pixels=MexInterface(uint8(OBT5.Internal.ApiCode.ReadPixels3D),obj.Pointer,options.X,options.Y,options.C,options.Z,options.T);
			end
		end
		function Pointer=PixelPointer5D(obj,options)
			%返回指定位置的只读像素指针
			%不能使用此方法返回的指针进行跨IFD对拷。只有OmeBigTiff5D对象返回的指针才支持跨IFD对拷。
			%% 名称值参数
			%X, Y, T(1,1)uint16=0，指针要指向的XYT
			%C, Z(1,1)uint8=0，指针要指向的CZ位置
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