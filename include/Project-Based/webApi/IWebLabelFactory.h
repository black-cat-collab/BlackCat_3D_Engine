#pragma once

namespace bc
{
	class CommonLabelFactory;
	class ProjectBasedClient;
	class IWebLabelFactory
	{
	public:
		IWebLabelFactory();
		virtual ~IWebLabelFactory();

		void SetLabelParam(NodeParamBase* pParam);
		void SetLabelDetailParam(NodeParamBase* pParam);
		void SetLabelClusterParam(NodeParamBase* pParam);		//聚合标签
		void SetLabelOption(Json::Value jOption) { m_jLabelOption = jOption; }
		Json::Value& GetLabelOption() { return m_jLabelOption; }
		NodeParamBase* GetLabelParam() { return m_pLabelNodeParam; }
		NodeParamBase* GetLabelDetailParam() { return m_pDetailNodeParam; }
		NodeParamBase* GetLabelClusterParam() { return m_pLabelClusterParam; }
		void SetStyleMD5(const std::string& strLabelStyleMD5, const std::string& strDetailStyleMD5, const std::string& strClusterStyleMD5);
		bool CheckStyleChanged(const std::string &strLabelStyleMD5, const std::string& strDetailStyleMD5, const std::string& strClusterStyleMD5);

		void SetLabelNodeType(NodeType_e eType);
		NodeType_e GetLabelNodeType() { return m_eLabelNodeType; }
		void SetLabelType(int nLabelType);
		int GetLabelType();
		CommonLabelFactory* GetFactory();
		bool IsInstanceFactory() { return m_bInstanceFactory; }
		virtual void ClearData() {}

	protected:
		void setText(INode* pNode, Json::Value& j3DTextMatchObject);
		void setFactory(CommonLabelFactory* pFactory);
		void iToWebSelectLabelPos(FactoryAssemble* pAssemble);		//发送给网页选中标签位置

	protected:
		NodeParamBase* m_pLabelNodeParam;
		NodeParamBase* m_pDetailNodeParam;
		NodeParamBase* m_pLabelClusterParam;
		NodeType_e m_eLabelNodeType;
		int m_nLabelType;			//图层id,唯一
		CommonLabelFactory* m_pFactory;
		bool m_bInstanceFactory;			//是否是实例
		Json::Value m_jLabelOption;		//图层配置

		std::string m_strLabelNodeParamMD5;
		std::string m_strDetailNodeParamMD5;
		std::string m_strClusterNodeParamMD5;

	};
}

