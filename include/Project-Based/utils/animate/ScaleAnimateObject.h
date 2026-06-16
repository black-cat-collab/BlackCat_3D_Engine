#pragma once

namespace bc
{
	class ScaleAnimateParam : public BaseAnimateParam
	{
	public:
		std::vector<Vector3ProValue> vecValue;

		ScaleAnimateParam()
		{
			BaseAnimateParam();
			eType = ANIMATE_OBJECT_SCALE;
		}

		class ScaleAnimateParam& operator = (const class ScaleAnimateParam& src)
		{
			BaseAnimateParam::operator=(src);
			vecValue = src.vecValue;

			return *this;
		}

	};

	class PROJECT_BASED_API ScaleAnimateObject : public BaseAnimateObject
	{
	public:
		ScaleAnimateObject(INode* pTarget, ScaleAnimateParam stParam);
		virtual ~ScaleAnimateObject();

		ScaleAnimateParam* GetAnimateParam() { return &m_stAnimateParam; }


	protected:
		//设置缓动动画参数
		virtual void iEasingAnimateUpdate(EasingAnimateParam* pEasingParam, std::map<std::string, INT_PTR> m_mapINTPTR,
			int nCurValueIndex, double dCurRate, int nCurLoopCount, int nAllLoopCount, bool bComplete, bool& bManualStop);
		//设置EasingAnimateParam
		virtual EasingAnimateParam iGetEasingAnimateParam();

	private:
		class OriData {
		public:
			float fDynamicScale;
			float fScale;
		};
		ScaleAnimateParam m_stAnimateParam;
		std::map<std::string, OriData> m_mapOriData;		//最初始的缩放

	};
}


