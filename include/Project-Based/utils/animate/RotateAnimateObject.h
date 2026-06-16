#pragma once

namespace bc
{
	class RotateAnimateParam : public BaseAnimateParam
	{
	public:
		std::vector<Vector3ProValue> vecValue;

		RotateAnimateParam()
		{
			BaseAnimateParam();
			eType = ANIMATE_OBJECT_ROTATE;
		}

		class RotateAnimateParam& operator = (const class RotateAnimateParam& src)
		{
			BaseAnimateParam::operator=(src);
			vecValue = src.vecValue;

			return *this;
		}

	};

	class PROJECT_BASED_API RotateAnimateObject : public BaseAnimateObject
	{
	public:
		RotateAnimateObject(INode* pTarget, RotateAnimateParam stParam);
		virtual ~RotateAnimateObject();
		virtual void Start();

		RotateAnimateParam* GetAnimateParam() { return &m_stAnimateParam; }


	protected:
		//设置缓动动画参数
		virtual void iEasingAnimateUpdate(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> m_mapINTPTR,
			int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop);
		//设置EasingAnimateParam
		virtual EasingAnimateParam iGetEasingAnimateParam();

	private:
		RotateAnimateParam m_stAnimateParam;

		Vector3d	m_vPreOrigin;			//上一个点，用来判断是否有外部设置了位置
		Vector3d m_vStandardOrigin;		//基准位置
	};
}


