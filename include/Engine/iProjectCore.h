#pragma once

namespace bc
{
	class IProjectCore
	{
	public:
		virtual void		iInitialize(ISystemAPI* pSystemAPI) = 0;

		/*
		 *	创建GIS球
		 *	strPath  [in]  路径参数
		 *	nLevel[in] 精度，数值越大精度越高，所占内存越大，性能要求越高，举例：3
		 *	fRadisu 数值越大精度越高，所占内存越大，性能要求越高，举例：4.0f
		 *  示例: iCreateEarth("F:\\GoogleEarthExport\\tms")
		 */
		//virtual INode*		iCreateEarth(const std::string& strPath, const int& nLevel, const float& fRadius) = 0;
	};
}

