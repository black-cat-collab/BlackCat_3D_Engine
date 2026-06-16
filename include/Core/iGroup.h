#pragma once

namespace bc
{
	class IGroup : public IComponent
	{
	public:
		/*
		 *	获取部件类型
		 *	return		[out]	渲染节点
		 */
		virtual int					iGetComponentType() { return COMPONENT_TYPE_GROUP; }
		virtual void				iSetSubScene(bool bSubScene) = 0;
		virtual void              iSetSubScenePath(const std::string& dir) = 0;
		virtual int                 iLoad(XMLNodePtr pXmlNode) = 0;
		//strFilePath 可外部传入文件路径
		virtual bool				iSave(const std::string &strFilePath = "") = 0;
	};
}

