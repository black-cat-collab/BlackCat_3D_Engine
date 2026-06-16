#pragma once

namespace bc
{
	class ITimeLine;
	typedef void(*TimeLineCompleteFunc)(ISystemAPI* pSystemAPI, ITimeLine* pTimeLine, void* pUser);

	// 时间轴对象属性值
	class TimeLineObjectValue
	{
	public:
		TimeLineObjectValue() : m_fValue(0.0f), m_nValue(0), m_vValue(Vector3d(0, 0, 0)), m_qValue(Quaterniond(1, 0, 0, 0)) {}
		TimeLineObjectValue(const float& fValue) : m_fValue(fValue), m_nValue(0), m_vValue(Vector3d(0, 0, 0)), m_qValue(Quaterniond(1, 0, 0, 0)) {}
		TimeLineObjectValue(const int& nValue) : m_fValue(0.0), m_nValue(nValue), m_vValue(Vector3d(0, 0, 0)), m_qValue(Quaterniond(1, 0, 0, 0)) {}
		TimeLineObjectValue(const Vector3d& vValue) : m_fValue(0.0), m_nValue(0), m_vValue(vValue), m_qValue(Quaterniond(1, 0, 0, 0)) {}
		TimeLineObjectValue(const Quaterniond &qValue) : m_fValue(0.0), m_nValue(0), m_vValue(Vector3d(0, 0, 0)), m_qValue(qValue) {}
		virtual ~TimeLineObjectValue() {}

		TimeLineObjectValue& operator =(const TimeLineObjectValue& rhs)
		{
			m_fValue = rhs.m_fValue;
			m_nValue = rhs.m_nValue;
			m_vValue = rhs.m_vValue;
			m_qValue = rhs.m_qValue;

			return *this;
		}

		// 值
		float			m_fValue;
		int				m_nValue;
		Vector3d			m_vValue;	
		Quaterniond		m_qValue;

		static float TimeLineFrameValue(const float& current, const float& last, float fScale)
		{
			return (current - last) * fScale + last;
		}

		static int TimeLineFrameValue(const int& current, const int& last, float fScale)
		{
			return (current - last) * fScale + last;
		}

		static Vector3d TimeLineFrameValue(const Vector3d& current, const Vector3d& last, float fScale)
		{
			return (current - last) * fScale + last;
		}

		static Quaterniond TimeLineFrameValue(const Quaterniond& current, const Quaterniond& last, float fScale)
		{
			return Quaterniond::Slerp(fScale, current, last);
		}
	};

	// 时间轴对象基类
	class TimeLineObject
	{
	public:
		TimeLineObject(ISystemAPI *pSystemAPI) : m_pSystemAPI(pSystemAPI) {}
		virtual ~TimeLineObject() {}

		void SetStartValue(const TimeLineObjectValue& tValue)
		{
			m_vecValue.insert(m_vecValue.begin(), tValue);
		}
		void AddKeyFrameValue(const TimeLineObjectValue& tValue)
		{
			m_vecValue.push_back(tValue);
		}
		void ClearFrameValue()
		{
			m_vecValue.clear();
		}
		virtual void FrameUpdate(const int& nCurrenIndex, const int& nLastIndex, const float& fScale)
		{
			if (nCurrenIndex < m_vecValue.size() && nCurrenIndex >= 0 &&
				nLastIndex < m_vecValue.size() && nLastIndex >= 0)
			{
				TimeLineObjectValue tCurrent = m_vecValue[nCurrenIndex];
				TimeLineObjectValue tLast = m_vecValue[nLastIndex];
				frameUpdate(tCurrent, tLast, fScale);
			}
		}

	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale) {}
		std::vector<TimeLineObjectValue>		m_vecValue;
		ISystemAPI*								m_pSystemAPI;
		
	};

	// 时间变化
	class TimeLineObjectSceneTime : public TimeLineObject
	{
	public:
		TimeLineObjectSceneTime(ISystemAPI *pSystemAPI, std::string strSceneView = "") : TimeLineObject(pSystemAPI), m_strSceneView(strSceneView) {}
		virtual ~TimeLineObjectSceneTime() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale) 
		{
			float fValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_fValue, tLast.m_fValue, fScale);
			m_pSystemAPI->iEngineAPI->iGetProjectConfig().sRenderConfig.fSceneTime = fValue;
		}
		std::string m_strSceneView;
	};

	// Node相关对象
	class TimeLineObjectNode : public TimeLineObject
	{
	public:
		TimeLineObjectNode(ISystemAPI *pSystemAPI, INode* pNode) : TimeLineObject(pSystemAPI), m_pNode(pNode) {}
		virtual ~TimeLineObjectNode() {}
	protected:
		INode*	m_pNode;
	};

	// Instance相关对象
	class TimeLineObjectInstance : public TimeLineObjectNode
	{
	public:
		TimeLineObjectInstance(ISystemAPI* pSystemAPI, INode* pNode, IInstance* pInstance) : 
			TimeLineObjectNode(pSystemAPI, pNode), m_pInstance(pInstance) {}
		virtual ~TimeLineObjectInstance() {}
	protected:
		IInstance* m_pInstance;
	};

	// 位移对象
	class TimeLineObjectTransform : public TimeLineObjectNode
	{
	public:
		TimeLineObjectTransform(ISystemAPI *pSystemAPI, INode* pNode) : TimeLineObjectNode(pSystemAPI, pNode) {}
		virtual ~TimeLineObjectTransform() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			Vector3d vValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_vValue, tLast.m_vValue, fScale);
			m_pNode->iSetOrigin(vValue);
		}
	};

	// 位移实例对象
	class TimeLineObjectInstanceTransform : public TimeLineObjectInstance
	{
	public:
		TimeLineObjectInstanceTransform(ISystemAPI* pSystemAPI, INode* pNode, IInstance* pInstance, const int& nIndex) : 
			TimeLineObjectInstance(pSystemAPI, pNode, pInstance), m_nIndex(nIndex) {}
		virtual ~TimeLineObjectInstanceTransform() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			Vector3d vValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_vValue, tLast.m_vValue, fScale);
			m_pInstance->iSetOrigin(vValue, m_nIndex);
		}
	protected:
		int	m_nIndex;
	};

	// 旋转对象
	class TimeLineObjectRotation : public TimeLineObjectNode
	{
	public:
		TimeLineObjectRotation(ISystemAPI *pSystemAPI, INode* pNode) : TimeLineObjectNode(pSystemAPI, pNode) {}
		virtual ~TimeLineObjectRotation() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			Vector3d vValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_vValue, tLast.m_vValue, fScale);
			m_pNode->iSetAngles(vValue);		
		}
	};

	// 旋转实例对象
	class TimeLineObjectInstanceRotation : public TimeLineObjectInstance
	{
	public:
		TimeLineObjectInstanceRotation(ISystemAPI* pSystemAPI, INode* pNode, IInstance* pInstance, const int& nIndex) : 
			TimeLineObjectInstance(pSystemAPI, pNode, pInstance), m_nIndex(nIndex) {}
		virtual ~TimeLineObjectInstanceRotation() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			Vector3d vValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_vValue, tLast.m_vValue, fScale);
			m_pInstance->iSetAngles(vValue, m_nIndex);
		}
	protected:
		int m_nIndex;
	};

	// 缩放对象
	class TimeLineObjectScale : public TimeLineObjectNode
	{
	public:
		TimeLineObjectScale(ISystemAPI* pSystemAPI, INode* pNode) : TimeLineObjectNode(pSystemAPI, pNode) {}
		virtual ~TimeLineObjectScale() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			Vector3d vValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_vValue, tLast.m_vValue, fScale);
			m_pNode->iSetScale(vValue);
		}
	};

	// 设置透明度
	class TimeLineObjectTransparent : public TimeLineObjectNode
	{
	public:
		TimeLineObjectTransparent(ISystemAPI *pSystemAPI, INode* pNode) : TimeLineObjectNode(pSystemAPI, pNode) {}
		virtual ~TimeLineObjectTransparent() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			float fValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_fValue, tLast.m_fValue, fScale);
			std::vector<IMaterial*> vecMaterial;
			IRenderNode* pRenderNode = static_cast<IRenderNode*>(m_pNode->iGetComponent(COMPONENT_TYPE_RENDER_NODE));
			if (pRenderNode)
			{
				pRenderNode->iGetMaterial(vecMaterial);
				for (int i = 0; i < vecMaterial.size(); ++i)
				{
					vecMaterial[i]->iSetAlpha(fValue);
				}
			}
		}
	};

	//带方向和位置的物体运动
	class TimeLineObjectAnmation : public TimeLineObjectNode
	{
	public:
		TimeLineObjectAnmation(ISystemAPI *pSystemAPI, INode* pNode) : TimeLineObjectNode(pSystemAPI, pNode) {}
		virtual ~TimeLineObjectAnmation() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			Vector3d vValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_vValue, tLast.m_vValue, fScale);
			Quaterniond qValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_qValue, tLast.m_qValue, fScale);

			m_pNode->iSetOrigin(vValue);
			Vector3d vAngle = qValue.ToAxis().ToAngles();
			//if (abs(vAngle.z) > 90)
			//{
			//	vAngle.z = -vAngle.z;		//导出插件算法和引擎不一样 导致自转角有问题
			//}
			m_pNode->iSetAngles(vAngle);
		}
	};

	//带方向和位置的实例物体运动
	class TimeLineObjectInstanceAnmation : public TimeLineObjectInstance
	{
	public:
		TimeLineObjectInstanceAnmation(ISystemAPI* pSystemAPI, INode* pNode, IInstance* pInstance, const int& nIndex) : 
			TimeLineObjectInstance(pSystemAPI, pNode, pInstance), m_nIndex(nIndex) {}
		virtual ~TimeLineObjectInstanceAnmation() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			Vector3d vValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_vValue, tLast.m_vValue, fScale);
			Quaterniond qValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_qValue, tLast.m_qValue, fScale);

			m_pInstance->iSetOrigin(vValue, m_nIndex);
			Vector3d vAngle = qValue.ToAxis().ToAngles();
			//if (abs(vAngle.z) > 90)
			//{
			//	vAngle.z = -vAngle.z;		//导出插件算法和引擎不一样 导致自转角有问题
			//}
			m_pInstance->iSetAngles(vAngle, m_nIndex);
		}
	protected:
		int	m_nIndex;
	};

	// 自定义回调对象
	template <class T>
	class TimeLineObjectCustom : public TimeLineObject
	{
	public:
		typedef void (*TimeLineObjectFrameUpdateFunc)(ISystemAPI*, void*, T);
	public:
		TimeLineObjectCustom(ISystemAPI* pSystemAPI) :
			TimeLineObject(pSystemAPI), m_pFrameUpdateFunc(nullptr), m_pFrameUpdateUser(nullptr) {}
		TimeLineObjectCustom(ISystemAPI* pSystemAPI, TimeLineObjectFrameUpdateFunc pFunc, void* pUser) :
			TimeLineObject(pSystemAPI), m_pFrameUpdateFunc(pFunc), m_pFrameUpdateUser(pUser) {}
		virtual void SetFrameUpdateFunc(TimeLineObjectFrameUpdateFunc pFunc, void* pUser)
		{
			m_pFrameUpdateFunc = pFunc;
			m_pFrameUpdateUser = pUser;
		}
		virtual ~TimeLineObjectCustom() {}
	protected:
		TimeLineObjectFrameUpdateFunc	m_pFrameUpdateFunc;
		void*							m_pFrameUpdateUser;
	};

	class TimeLineObjectCustomFloat : public TimeLineObjectCustom<double>
	{
	public:
		TimeLineObjectCustomFloat(ISystemAPI* pSystemAPI) :
			TimeLineObjectCustom<double>(pSystemAPI) {}
		TimeLineObjectCustomFloat(ISystemAPI* pSystemAPI, TimeLineObjectCustom<double>::TimeLineObjectFrameUpdateFunc pFunc, void* pUser) :
			TimeLineObjectCustom<double>(pSystemAPI, pFunc, pUser) {}
		virtual ~TimeLineObjectCustomFloat() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const double& fScale)
		{
			float fValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_fValue, tLast.m_fValue, fScale);
			m_pFrameUpdateFunc(m_pSystemAPI, m_pFrameUpdateUser, fValue);
		}
	};

	class TimeLineObjectCustomInt : public TimeLineObjectCustom<int>
	{
	public:
		TimeLineObjectCustomInt(ISystemAPI* pSystemAPI) :
			TimeLineObjectCustom<int>(pSystemAPI) {}
		TimeLineObjectCustomInt(ISystemAPI* pSystemAPI, TimeLineObjectCustom<int>::TimeLineObjectFrameUpdateFunc pFunc, void* pUser) :
			TimeLineObjectCustom<int>(pSystemAPI, pFunc, pUser) {}
		virtual ~TimeLineObjectCustomInt() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const float& fScale)
		{
			int nValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_nValue, tLast.m_nValue, fScale);
			m_pFrameUpdateFunc(m_pSystemAPI, m_pFrameUpdateUser, nValue);
		}
	};

	class TimeLineObjectCustomVector : public TimeLineObjectCustom<Vector3d>
	{
	public:
		TimeLineObjectCustomVector(ISystemAPI* pSystemAPI) :
			TimeLineObjectCustom<Vector3d>(pSystemAPI) {}
		TimeLineObjectCustomVector(ISystemAPI* pSystemAPI, TimeLineObjectCustom<Vector3d>::TimeLineObjectFrameUpdateFunc pFunc, void* pUser) :
			TimeLineObjectCustom<Vector3d>(pSystemAPI, pFunc, pUser) {}
		virtual ~TimeLineObjectCustomVector() {}
	protected:
		virtual void frameUpdate(const TimeLineObjectValue& tCurrent, const TimeLineObjectValue& tLast, const double& fScale)
		{
			Vector3d vValue = TimeLineObjectValue::TimeLineFrameValue(tCurrent.m_vValue, tLast.m_vValue, fScale);
			m_pFrameUpdateFunc(m_pSystemAPI, m_pFrameUpdateUser, vValue);
		}
	};

	enum TimeLinePlayState
	{
		TIME_LINE_PLAY_FROM_START = 0,		// 从起始播放一次
		TIME_LINE_PLAY_FROM_END,			// 从最后播放一次
		TIME_LINE_PLAY,						// 继续播放
		TIME_LINE_PLAY_FROM_START_LOOP,		// 从起始循环播放
		TIME_LINE_PLAY_FROM_END_LOOP,		// 从最后循环播放
		TIME_LINE_PLAY_LOOP,				// 继续循环播放
		TIME_LINE_STOP						// 停止播放
	};

	class ITimeLine
	{
	public:
		// 绑定对象
		virtual void iBindObject(TimeLineObject *pObject) = 0;
		// 解绑对象
		virtual void iUnbindAllObject() = 0;

		// 设置关键帧时间 单位：毫秒 
		virtual void iAddKeyFrame(const float& fMSecond) = 0;

		// 启动
		virtual void iPlay(const TimeLinePlayState& eState, const bool& bReverse = false, const float& fDelayMS = 0.0f) = 0;

		// 获取状态
		virtual TimeLinePlayState iGetState() = 0;

		// 设置完成回调函数
		virtual void iSetCompleteFunc(TimeLineCompleteFunc pFunc, void* pUser) = 0;
	};
}

