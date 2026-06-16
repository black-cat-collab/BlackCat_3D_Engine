#pragma once

namespace bc
{
	enum class EAnimateObject
	{
		ANIMATE_TRANSLATE = 0,		//位移动画
		ANIMATE_ROTATE,			//选中动画
		ANIMATE_SCALE,			//缩放动画
	};

	class TranslateAnimateParam
	{
	public:
		Vector3 vOffset;		//移动的目标位置
		float fSpeed;				//速度
		bool bSpeedUniform;			//是否匀速（不是则是贝塞尔曲线）
		long lIntervalMill;		//每个周期的间隔时间（默认是0,毫秒）

		TranslateAnimateParam()
		{
			fSpeed = 30.0f;
			bSpeedUniform = false;
			lIntervalMill = 0;
		}

		TranslateAnimateParam& operator = (const TranslateAnimateParam& src)
		{
			vOffset = src.vOffset;
			fSpeed = src.fSpeed;
			bSpeedUniform = src.bSpeedUniform;
			lIntervalMill = src.lIntervalMill;

			return (*this);
		}

	};

	class RotateAnimateParam
	{
	public:
		Vector3 vOffset;		//移动的目标位置
		float fSpeed;				//速度
		bool bSpeedUniform;			//是否匀速（不是则是贝塞尔曲线）
		long lIntervalMill;		//每个周期的间隔时间（默认是0,毫秒）

		RotateAnimateParam()
		{
			fSpeed = 30.0f;
			bSpeedUniform = false;
			lIntervalMill = 0;
		}

		RotateAnimateParam& operator = (const RotateAnimateParam& src)
		{
			vOffset = src.vOffset;
			fSpeed = src.fSpeed;
			bSpeedUniform = src.bSpeedUniform;
			lIntervalMill = src.lIntervalMill;

			return (*this);
		}

	};

	class IAnimateWithNode
	{
	public:
		virtual EAnimateObject iGetAnimateObjectType() = 0;
		virtual void iFrameUpdate() = 0;
	};
}

