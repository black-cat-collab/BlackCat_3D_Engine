#pragma once

#include <iostream>

namespace bc
{
	class BaseAnimateObject;
	class BaseAnimateParam;

	class PROJECT_PACK_API AnimateWithNodeMg
	{
	public:
		virtual ~AnimateWithNodeMg();

		static AnimateWithNodeMg* GetInstance();
		void FrameUpdate();

		BaseAnimateObject* AddAnimate(INode* pTarget, BaseAnimateParam *pAnimateParam,const std::string &strAnimateID);

		void RemoveAnimate(const std::string &strAnimateID, INode* pTargetNode = nullptr);
		BaseAnimateObject* GetAnimateObject(const std::string& strAnimateID,INode *pTargetNode = nullptr);

	protected:
		AnimateWithNodeMg();

	private:
		static AnimateWithNodeMg* m_Instance;

		std::map<INode*, std::vector<BaseAnimateObject*>> m_mapAnimateWithNode;

	private:
		class GC // 垃圾回收类
		{
		public:
			GC()
			{
				std::cout << "GC construction" << std::endl;
			}

			~GC()
			{
				cout << "GC destruction" << endl;
				if (m_Instance != NULL)
				{
					delete m_Instance;
					m_Instance = NULL;
					std::cout << "AnimateWithNodeMg construction" << std::endl;
				}
			}
		};
		static GC gc;  //垃圾回收类的静态成员
	};
}


