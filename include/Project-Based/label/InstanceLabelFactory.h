#pragma once

namespace bc
{
	class InstanceSelector;
	class PROJECT_BASED_API InstanceLabelFactory : public CommonLabelFactory
	{
	public:
		InstanceLabelFactory();
		virtual ~InstanceLabelFactory();

		virtual bool IsInstanceFactory() { return true; }
		virtual void SetTypeVisible(std::vector<int>& vecType);
		virtual void RefreshZero();
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		virtual void SetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		bool IsContainerNode(INode* pNode, bool bIncludeDetail = true);		//判断节点是否是这个Factory的
		INode* GetSelectBoardNode(ProjectBasedSceneElement* pInfo);	//获取选中的那个Board节点
		FactoryAssemble* FindAssembleBySelectNode(INode* pNode);	//通过选中的INode查找Assemble
		void ThreadFilterShowAssemble();
		void HandleRefreshInstanceData();
		virtual bool IsAllStop();

	protected:
		/*内部方法*/

		//添加实例数据
		virtual BoardInstanceData* iAddInstanceData(INode* pInstanceNode, ProjectBasedSceneElement* pData);
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);

	protected:
		//处理数据
		virtual FactoryAssemble* handleData(ProjectBasedSceneElement* pData);
		virtual void setAssembleVisible(FactoryAssemble* pAssemble, bool bVisible, bool bDynamicNode);

		virtual void updateDataForLabel(FactoryAssemble* pAssemble);

	private:
		std::map<int, INode*> m_mapInstanceNode;
		InstanceSelector* m_pInstanceSelector;
		std::atomic<bool> m_bRefreshInstanceData;
		BCMutexLock m_RefreshInstanceDataLock;
		std::map<int, std::vector<BoardInstanceData*>> m_mapShowInstanceData;
		bool m_bDoingThreadFilter;

	protected:

	};
}


