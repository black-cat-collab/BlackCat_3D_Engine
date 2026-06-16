#pragma once

namespace bc
{
	class TranslateAnimateParam : public BaseAnimateParam
	{
	public:
		std::vector<Vector3ProValue> vecValue;

		TranslateAnimateParam()
		{
			BaseAnimateParam();
			eType = ANIMATE_OBJECT_TRANSLATE;
		}

		class TranslateAnimateParam& operator = (const class TranslateAnimateParam& src)
		{
			BaseAnimateParam::operator=(src);
			vecValue = src.vecValue;

			return *this;
		}

	};

	class PROJECT_BASED_API TranslateAnimateObject : public BaseAnimateObject
	{
	public:
		TranslateAnimateObject(INode* pTarget, TranslateAnimateParam stParam);
		virtual ~TranslateAnimateObject();

		TranslateAnimateParam* GetAnimateParam() { return &m_stAnimateParam; }
		virtual void Start();


	protected:
		//设置缓动动画参数
		virtual void iEasingAnimateUpdate(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> m_mapINTPTR,
			int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop);
		//设置EasingAnimateParam
		virtual EasingAnimateParam iGetEasingAnimateParam();

	protected:
		TranslateAnimateParam m_stAnimateParam;

		Vector3d	m_vPreOrigin;			//上一个点，用来判断是否有外部设置了位置
		Vector3d m_vStandardOrigin;		//基准位置
		Vector2 m_vDistanceToTargetOrigin;

	};
}


