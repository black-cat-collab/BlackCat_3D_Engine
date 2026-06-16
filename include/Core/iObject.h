#pragma once

namespace bc
{
	enum FrameUpdateObjectType
	{
		OBJECT_TYPE_NONE		= -1,		// 无效
		OBJECT_TYPE_TIME_LINE	= 0,		// 时间轴
		OBJECT_TYPE_CLUSTERER_HEIGHT,		// 根据相机高度聚合，可多层
		OBJECT_TYPE_CLUSTERER_DISTANCE		// 根据与相机距离聚合，只可一层
	};

	class ObjectParamBase
	{
	public:
		std::string	strID;
		int			nObjectType;

		ObjectParamBase() :
			nObjectType(OBJECT_TYPE_NONE)
		{

		}
	};

	class IObject
	{
	public:
		// 获取参数
		virtual const ObjectParamBase*	iGetParam() = 0;

		// 是否帧更新
		virtual bool					iIsFrameUpdate() = 0;
	};
}

