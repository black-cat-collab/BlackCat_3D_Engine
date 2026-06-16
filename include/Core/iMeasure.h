#pragma once

namespace bc
{
	enum MeasureStatus
	{
		MEASURE_NONE = 0,
		MEASURE_BEGIN,
		MEASURE_DURING,
		MEASURE_END
	};

	class MeasureDistanceParam : public NodeParamBase
	{
	public:
		Vector4			vMeasureLineColor;
		float			fMeasureLineWidth;

		FontBoardParam	cFontBoardParam;		//文字面板参数
		Vector3			vTextColor;
		Vector2			vTextSize;

		BoardParam		cPointParam;			//起始 终止点位面板参数

		MeasureDistanceParam()
		{
			nNodeType = NODE_MEASURE_DISTANCE;

			vMeasureLineColor = Vector4(1.0f);
			fMeasureLineWidth = 1.0f;

			cFontBoardParam = FontBoardParam();
			vTextColor = Vector3(1.0f);
			vTextSize = Vector2(5.0f);

			cPointParam = BoardParam();
			cPointParam.direction = FaceToCamera;
		}

		MeasureDistanceParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		MeasureDistanceParam& operator = (const MeasureDistanceParam& src)
		{
			NodeParamBase::operator=(src);

			vMeasureLineColor = src.vMeasureLineColor;
			fMeasureLineWidth = src.fMeasureLineWidth;

			cFontBoardParam = src.cFontBoardParam;
			vTextColor = src.vTextColor;
			vTextSize = src.vTextSize;

			cPointParam = src.cPointParam;

			return *this;
		}
	};

	class MeasureAreaParam : public NodeParamBase
	{
	public:
		Vector4			vMeasureLineColor;
		float			fMeasureLineWidth;

		bool			bUnifyHeight;			//自动统一高度
		float			fEpsilonDistance;		//判断结束测量的最小距离

		Vector4			vAreaColor;
		Vector4			vAreaSelectColor;
		Vector4			vAreaHoverColor;

		FontBoardParam	cFontBoardParam;		//文字面板参数
		Vector3			vTextColor;
		Vector2			vTextSize;

		MeasureAreaParam()
		{
			nNodeType = NODE_MEASURE_AREA;

			vMeasureLineColor = Vector4(1.0f);
			fMeasureLineWidth = 1.0f;

			bUnifyHeight = true;
			fEpsilonDistance = 1.0f;

			vAreaColor = Vector4(1.0f, 1.0f, 1.0f, 0.3f);
			vAreaSelectColor = Vector4(1.0f, 1.0f, 1.0f, 0.6f);
			vAreaHoverColor = Vector4(1.0f, 1.0f, 1.0f, 0.3f);

			cFontBoardParam = FontBoardParam();
			vTextColor = Vector3(1.0f);
			vTextSize = Vector2(1.0f);
		}

		MeasureAreaParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		MeasureAreaParam& operator = (const MeasureAreaParam& src)
		{
			NodeParamBase::operator=(src);

			vMeasureLineColor = src.vMeasureLineColor;
			fMeasureLineWidth = src.fMeasureLineWidth;

			bUnifyHeight = src.bUnifyHeight;
			fEpsilonDistance = src.fEpsilonDistance;

			vAreaColor = src.vAreaColor;
			vAreaSelectColor = src.vAreaSelectColor;
			vAreaHoverColor = src.vAreaHoverColor;

			cFontBoardParam = src.cFontBoardParam;
			vTextColor = src.vTextColor;
			vTextSize = src.vTextSize;

			return *this;
		}
	};

	class IMeasureDistance : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_MEASURE_DISTANCE; }
		virtual void        iBegin() = 0;
		virtual void        iEnd() = 0;
	};

	class IMeasureArea : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_MEASURE_AREA; }
		virtual void		iMeasureAreaByPoints(const std::vector<Vector3>& vecPoint) = 0;

		virtual void        iBegin() = 0;
		virtual void        iEnd() = 0;
	};
} // namespace bc


