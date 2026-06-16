#pragma once

namespace bc
{	
	enum class SelectorType
	{
		Selector_Line,
		Selector_Polygon,
		Selector_Face,
		Selector_Point,
	};

	enum class ControlMode
	{
		Single, Continuous
	};

	class PolygonSelectorParam : public NodeParamBase
	{
	public:

		Vector4			vLineColor;			// 颜色
		float					fLineWidth;			// 线宽
		int					nPointMaxLimit;		// 最多能绘制几个点
		bool					bDepthTest;			// 是否开启深度测试
		float                 fHeight;               // 高度
		std::vector<Vector3> vecPoint; // 控制点
		SelectorType   eType;
		std::function<void(INode*)> callback;
		PolygonSelectorParam() : NodeParamBase()
		{
			nNodeType = NODE_POLYGON_SELECTOR;
			vLineColor = Vector4(1.0);
			fLineWidth = 1.0f;
			nPointMaxLimit = -1;
			fHeight = 0.0f;
			bDepthTest = true;
			eType = SelectorType::Selector_Polygon;
			callback = nullptr;
		}

		PolygonSelectorParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);
			return *this;
		}

		PolygonSelectorParam& operator=(const PolygonSelectorParam& rhs)
		{
			NodeParamBase::operator=(rhs);

			vLineColor = rhs.vLineColor;
			eType = rhs.eType;
			fLineWidth = rhs.fLineWidth;
			fHeight = rhs.fHeight;
			nPointMaxLimit = rhs.nPointMaxLimit;
			bDepthTest = rhs.bDepthTest;
			vecPoint = rhs.vecPoint;
			return *this;
		}
	};
	class IPolygonSelector : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_POLYGON_SELECTOR; }
		virtual void      iSetEditor(bool b) = 0;
		virtual void      iSetParam(const PolygonSelectorParam& param) = 0;
		virtual void      iGetParam(PolygonSelectorParam& param) = 0;
		virtual void      iSetSelectType(SelectorType type) = 0;
		virtual void      iSetControlType(ControlMode mode) = 0;
		virtual void      iSetCallback(const std::function<void(INode*)>& callback) = 0;
		virtual void      iGetControlPoints(std::vector<Vector3>& output) = 0;
		virtual void      iFinish() = 0;
		virtual void      iReset() = 0;
	};
} // namespace bc


