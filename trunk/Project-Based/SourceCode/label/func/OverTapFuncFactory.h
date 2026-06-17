#pragma once

namespace bc
{
	class OverTapFuncFactory : public CommonLabelFactory
	{
	public:
		OverTapFuncFactory(OverTapFunc *pOverTapFunc);
		virtual ~OverTapFuncFactory();

		void RefreshExpandData(OverTapFunc::OverTapData* pInfo);
		void RefreshExpandData();

	protected:
		//创建节点
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);
		//更新数据
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);
		//设置标签是否选中
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void iFrameUpdateForLabel(FactoryAssemble* pAssemble);
		virtual void iHideAssembleForLabel(FactoryAssemble* pAssemble);
		virtual void iShowAssembleForLabel(FactoryAssemble* pAssemble);

	private:
		Vector3d calcAbsolutePos(INode* pNode, Vector2 vOffset);

	protected:
		OverTapFunc* m_pOverTapFunc;
	};
}


