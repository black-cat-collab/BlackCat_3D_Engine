#pragma once

namespace bc
{
	class DefaultDetailFactory : public LabelDetailFactory
	{
	public:
		DefaultDetailFactory();
		~DefaultDetailFactory();

	protected:
		//创建节点
		virtual INode* iCreateNode(int nType,ProjectBasedSceneElement *pData);
		virtual int iGetTypeByData(ProjectBasedSceneElement* pInfo);
		//节点打开时
		virtual void iOpenWithNode(FactotryAssembleDetail* pAssemble);
		//更新节点
		virtual void iUpdateWithNode(FactotryAssembleDetail* pAssemble);
		//节点关闭时
		virtual void iCloseWithNode(FactotryAssembleDetail* pAssemble);
		// 更新详细信息面板位置
		virtual void iFrameUpdateWithNode(FactotryAssembleDetail* pAssemble);

	private:
		void sendDetailScreenPos(FactotryAssembleDetail *pAssemble);		//发送详情面板对应的屏幕坐标

	};
}


