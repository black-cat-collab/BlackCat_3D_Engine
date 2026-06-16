#pragma once

namespace bc
{
	class FlyLineParam : public NodeParamBase
	{
	public:
		std::string					strMaterial;				// 飞线材质
		std::vector<Vector3>	vecVertex;					// 飞线控制点
		int						nTubularSegment;			// 沿长度方向分段数
		int						nRadialSegment;				// 沿圆柱方向分段数
		float					fRadius;					// 圆柱半径
		bool					bAnimation;					// 贴图是否流动
		float					fAnimationSpeed;			// 贴图流动速度
		bool					bGlowEnable;				// 是否开启辉光
		float					fGlowIntensity;				// 辉光强度

		FlyLineParam() : NodeParamBase()
		{
			nNodeType = NODE_FLYLINE;
			strMaterial = "";
			vecVertex.clear();
			nTubularSegment = 2;
			nRadialSegment = 4;
			fRadius = 1.0;
			bAnimation = false;
			fAnimationSpeed = 0.1;
			bGlowEnable = false;
			fGlowIntensity = 5.0;
		}

		FlyLineParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		FlyLineParam& operator = (const FlyLineParam& src)
		{
			NodeParamBase::operator=(src);

			strMaterial = src.strMaterial;
			vecVertex = src.vecVertex;
			nTubularSegment = src.nTubularSegment;
			nRadialSegment = src.nRadialSegment;
			fRadius = src.fRadius;
			bAnimation = src.bAnimation;
			fAnimationSpeed = src.fAnimationSpeed;
			bGlowEnable = src.bGlowEnable;
			fGlowIntensity = src.fGlowIntensity;
			return *this;
		}
	};

	class MultiFlyLineParam : public NodeParamBase
	{
	public:
		std::string					strMaterialFirst;			// 飞线第一段材质
		std::string					strMaterialSecond;			// 飞线第二段材质
		std::vector<Vector3>	vecVertex;					// 飞线控制点
		int						nTubularSegment;			// 沿长度方向分段数
		int						nRadialSegment;				// 沿圆柱方向分段数
		float					fRadius;					// 圆柱半径
		bool					bAnimation;					// 贴图是否流动
		float					fAnimationSpeed;			// 贴图流动速度
		bool					bSwitchAnimation;			// 贴图是否切换
		float					fSwitchAnimationSpeed;		// 贴图切换速度
		bool					bGlowEnable;				// 是否开启辉光
		float					fGlowIntensity;				// 辉光强度

		MultiFlyLineParam() : NodeParamBase()
		{
			nNodeType = NODE_MULTI_FLYLINE;
			strMaterialFirst = "";
			strMaterialSecond = "";
			vecVertex.clear();
			nTubularSegment = 2;
			nRadialSegment = 4;
			fRadius = 1.0;
			bAnimation = false;
			fAnimationSpeed = 0.1;
			bSwitchAnimation = false;
			fSwitchAnimationSpeed = 0.05;
			bGlowEnable = false;
			fGlowIntensity = 5.0;
		}

		MultiFlyLineParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		MultiFlyLineParam& operator = (const MultiFlyLineParam& src)
		{
			NodeParamBase::operator=(src);

			strMaterialFirst = src.strMaterialFirst;
			strMaterialSecond = src.strMaterialSecond;
			vecVertex = src.vecVertex;
			nTubularSegment = src.nTubularSegment;
			nRadialSegment = src.nRadialSegment;
			fRadius = src.fRadius;
			bAnimation = src.bAnimation;
			fAnimationSpeed = src.fAnimationSpeed;
			bSwitchAnimation = src.bSwitchAnimation;
			fSwitchAnimationSpeed = src.fSwitchAnimationSpeed;
			bGlowEnable = src.bGlowEnable;
			fGlowIntensity = src.fGlowIntensity;
			return *this;
		}
	};


	class IFlyLine : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_FLY_LINE; }

		virtual void		iSetMaterial(const std::string& strImage) = 0;
	};

	class IMultiFlyLine : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_MULTI_FLY_LINE; }

		virtual void		iSetFirstMaterial(const std::string& strImage) = 0;
		virtual void		iSetSecondMaterial(const std::string& strImage) = 0;
	};
} // namespace bc


