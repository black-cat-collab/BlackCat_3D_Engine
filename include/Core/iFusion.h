#pragma once

namespace bc
{
	class IFusion : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_FUSION; }
		virtual void		iGetFusionParam(FusionParam& tParam) = 0;
		virtual void		iSetFusionParam(const FusionParam& tParam) = 0;
		virtual void        iOutputToFile(const std::string& path) = 0;
		//设置文件保存文件夹路径（主要用于业务保存，不设置则使用默认）
		virtual void        iSetSaveDir(const std::string& path) = 0;
	};
} // namespace bc


