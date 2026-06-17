#pragma once

namespace bc
{
	class SelectInfo : public ProjectBasedSceneElement
	{
	public:
		FactoryAssemble* pAssemble;

	public:
		SelectInfo():
			ProjectBasedSceneElement()
		{
			pAssemble = nullptr;
		}

	};

	//选中标签类
	class SelectBoardFactory : public CommonLabelFactory
	{
	public:
		SelectBoardFactory();
		virtual ~SelectBoardFactory();

	protected:
		virtual INode* iCreateNodeForLabel(int nType, ProjectBasedSceneElement* pInfo);
		virtual void iUpdateDataForLabel(FactoryAssemble* pAssemble);
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
	};


	class InstanceSelector
	{
	public:
		InstanceSelector(CommonLabelFactory* pComFactory);
		virtual ~InstanceSelector();

		void SetSelect(FactoryAssemble* pAssemble,bool bSelect);
		void ClearAllSelect();
		void SetTypeVisible(std::vector<int>& vecType);
		CommonLabelFactory* GetComFactory();
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		INode* FindSelectBoardNode(ProjectBasedSceneElement* pInfo);
		CommonLabelFactory* GetSelectFactory() { return m_pSelectBoardFactory; }

	protected:
		CommonLabelFactory* m_pComFactory;
		std::vector<SelectInfo*> m_vecSelectData;
		SelectBoardFactory* m_pSelectBoardFactory;

	};
}


