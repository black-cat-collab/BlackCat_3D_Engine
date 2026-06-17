#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/animate/EasingAnimate.h"
#include "Project-Based/utils/animate/BaseAnimateObject.h"
#include "Project-Based/utils/animate/TranslateAnimateObject.h"
#include "Project-Based/utils/animate/RotateAnimateObject.h"
#include "Project-Based/utils/animate/ScaleAnimateObject.h"
#include "Project-Based/utils/AnimateWithNodeMg.h"

using namespace bc;

AnimateWithNodeMg* AnimateWithNodeMg::m_Instance = NULL;
AnimateWithNodeMg::GC AnimateWithNodeMg::gc;

AnimateWithNodeMg::AnimateWithNodeMg()
{
	
}

AnimateWithNodeMg::~AnimateWithNodeMg()
{
	Clear();
}

AnimateWithNodeMg* AnimateWithNodeMg::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new AnimateWithNodeMg();
	}
	return m_Instance;
}

void AnimateWithNodeMg::Clear()
{
	for (std::map<INode*, std::vector<BaseAnimateObject*>>::iterator it = m_mapAnimateWithNode.begin(); it != m_mapAnimateWithNode.end(); it++)
	{
		for (size_t i = 0; i < it->second.size(); i++)
		{
			DELETE_PTR(it->second[i]);
		}
		it->second.resize(0);
	}
	m_mapAnimateWithNode.clear();
}

void AnimateWithNodeMg::FrameUpdate()
{
	for (std::map<INode*, std::vector<BaseAnimateObject*>>::iterator it = m_mapAnimateWithNode.begin(); it != m_mapAnimateWithNode.end(); it++)
	{
		for (size_t i = 0; i < it->second.size(); i++)
		{
			it->second[i]->FrameUpdate();
		}
	}
}

BaseAnimateObject* AnimateWithNodeMg::AddAnimate(INode* pTarget, BaseAnimateParam* pAnimateParam, const std::string& strAnimateID)
{
	BaseAnimateObject* pObject = nullptr;
	if (pAnimateParam->eType == ANIMATE_OBJECT_TRANSLATE)
	{
		TranslateAnimateParam* pParam = (TranslateAnimateParam*)pAnimateParam;
		pObject = new TranslateAnimateObject(pTarget, *pParam);
	}
	else if (pAnimateParam->eType == ANIMATE_OBJECT_ROTATE)
	{
		RotateAnimateParam* pParam = (RotateAnimateParam*)pAnimateParam;
		pObject = new RotateAnimateObject(pTarget, *pParam);
	}
	else if (pAnimateParam->eType == ANIMATE_OBJECT_SCALE)
	{
		ScaleAnimateParam* pParam = (ScaleAnimateParam*)pAnimateParam;
		pObject = new ScaleAnimateObject(pTarget, *pParam);
	}
	if (!pObject)
	{
		return nullptr;
	}
	RemoveAnimate(strAnimateID,pTarget);
	pObject->Start();
	pObject->SetAnimateID(strAnimateID);
	std::vector<BaseAnimateObject*>& vec = m_mapAnimateWithNode[pTarget];
	vec.push_back(pObject);

	return pObject;
}

void AnimateWithNodeMg::RemoveAnimate(const std::string& strAnimateID,INode* pTargetNode)
{
	if (pTargetNode)
	{
		std::map<INode*, std::vector<BaseAnimateObject*>>::iterator mapIt = m_mapAnimateWithNode.find(pTargetNode);
		if (mapIt == m_mapAnimateWithNode.end())
		{
			return;
		}
		std::vector<BaseAnimateObject*>& vec = mapIt->second;
		for (std::vector<BaseAnimateObject*>::iterator it = vec.begin(); it != vec.end(); it++)
		{
			if ((*it)->GetAnimateID() == strAnimateID)
			{
				DELETE_PTR(*it);
				vec.erase(it);
				return;
			}
		}
	}
	else
	{
		for (std::map<INode*, std::vector<BaseAnimateObject*>>::iterator it = m_mapAnimateWithNode.begin(); it != m_mapAnimateWithNode.end(); it++)
		{
			for (std::vector<BaseAnimateObject*>::iterator animateIt = it->second.begin(); animateIt != it->second.end(); animateIt++)
			{
				if ((*animateIt)->GetAnimateID() == strAnimateID)
				{
					DELETE_PTR(*animateIt);
					it->second.erase(animateIt);
					return;
				}
			}
		}
	}
}

void AnimateWithNodeMg::RemoveAnimate(INode* pTargetNode)
{
	if (!pTargetNode)
	{
		return;
	}
	std::map<INode*, std::vector<BaseAnimateObject*>>::iterator mapIt = m_mapAnimateWithNode.find(pTargetNode);
	if (mapIt == m_mapAnimateWithNode.end())
	{
		return;
	}
	std::vector<BaseAnimateObject*>& vec = mapIt->second;
	for (std::vector<BaseAnimateObject*>::iterator it = vec.begin(); it != vec.end(); it++)
	{
		DELETE_PTR(*it);
	}
	vec.clear();
	m_mapAnimateWithNode.erase(mapIt);
}

BaseAnimateObject* AnimateWithNodeMg::GetAnimateObject(const std::string& strAnimateID, INode* pTargetNode)
{
	if (pTargetNode)
	{
		std::vector<BaseAnimateObject*>& vec = m_mapAnimateWithNode[pTargetNode];
		for (std::vector<BaseAnimateObject*>::iterator it = vec.begin(); it != vec.end(); it++)
		{
			if ((*it)->GetAnimateID() == strAnimateID)
			{
				return *it;
			}
		}
	}
	else
	{
		for (std::map<INode*, std::vector<BaseAnimateObject*>>::iterator it = m_mapAnimateWithNode.begin(); it != m_mapAnimateWithNode.end(); it++)
		{
			for (size_t i = 0; i < it->second.size(); i++)
			{
				if (it->second[i]->GetAnimateID() == strAnimateID)
				{
					return it->second[i];
				}
			}
		}
	}
	return nullptr;
}

