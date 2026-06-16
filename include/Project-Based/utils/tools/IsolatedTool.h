#pragma once

/***************/
//单体化工具
/***************/

namespace bc
{
	class PROJECT_BASED_API IsolatedTool
	{
	public:
		IsolatedTool();
		virtual ~IsolatedTool();

		void				IndividualBuilding(bool bIsIn = false);	//单体楼栋
		void				VerticalTorusCut(bool bIsIn = false);		//垂直环切
		void				VerticalFlushCut(bool bIsIn = false);		//垂直面切
		void				VerticalSingleCut(bool bIsIn = false);	//垂直单切
		void				HorizontalCut(bool bIsIn = false);		//水平单切
		void				HorizontalFullCut(bool bIsIn = false);	//水平全切
		void				SaveChanges();			//保存修改
		void				Cancel();				//取消
		void				CancelPonlygonSelector();//取消选择器的线

		void				SetNodeOnlyVisible(INode* pNode, bool bVisible);

		INode* GetIsolatedNode();

	private:
		void				excBuilding();
		std::map<INode*, bool> m_mapChildVisible;				//记录进入编辑之前的状态
	protected:
		ISystemAPI* m_pSystemAPI;
	};
}

