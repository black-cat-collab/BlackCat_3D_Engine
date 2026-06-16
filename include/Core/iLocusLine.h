#pragma once

namespace bc
{
	class LocusLineSegment
	{
	public:
		std::vector<Vector3>	vecVertex;
		Vector4					vColor;
		float					fWidth;
		int						nLineStyle;
		int						nDottedFactor;
		ushort					usDottedStyle;
		std::string				strID;			//片段唯一标识（不重复，外部填入，用来做匹配）
		LocusLineSegment()
		{
			vecVertex.clear();
			vColor = Vector4(1.0f);
			fWidth = 1.0f;
			nLineStyle = LineStyle_e::LINE_STYLE_STRIP;
			nDottedFactor = 1;
			usDottedStyle = 0xFF00;
		}

		LocusLineSegment& operator = (const LocusLineSegment& src)
		{
			vecVertex = src.vecVertex;
			vColor = src.vColor;
			fWidth = src.fWidth;
			nLineStyle = src.nLineStyle;
			nDottedFactor = src.nDottedFactor;
			usDottedStyle = src.usDottedStyle;
			strID = src.strID;
			return *this;
		}
	};

	class LocusLineParam : public NodeParamBase
	{
	public:
		LocusLineSegment				stDefaultSegment;		//主要用来存储线宽，颜色，虚线样式等
		std::vector<LocusLineSegment>	vecSegment;
		bool							bDepthTest;				// 深度测试
		bool							bGlowEnable;			// 是否辉光
		LocusLineParam()
		{
			nNodeType = NODE_LOCUS_LINE;
			vecSegment.clear();
			bDepthTest = true;
			bGlowEnable = false;
		}

		LocusLineParam& operator=(const NodeParamBase& src)
		{
			NodeParamBase::operator=(src);

			return *this;
		}

		LocusLineParam& operator = (const LocusLineParam& src)
		{
			NodeParamBase::operator=(src);

			stDefaultSegment = src.stDefaultSegment;
			vecSegment = src.vecSegment;
			bDepthTest = src.bDepthTest;
			bGlowEnable = src.bGlowEnable;
			return *this;
		}
	};

	class ILocusLine : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_LOCUS_LINE; }
		/*
		*	添加轨迹线段
		*	tParam  [in]  轨迹线段参数
		*/
		virtual void		iAddLocusLineSegment(const LocusLineSegment& tParam) = 0;

		/*
		*	使用新的轨迹线段代替指定轨迹线段
		*	nIndex  [in]  要替代的轨迹线段索引
		*	tParam  [in]  新的轨迹线段参数
		*/
		virtual void		iUpdateLocusLineSegment(const size_t& nIndex, const LocusLineSegment& tParam) = 0;
		
		/*
		*	替换所有的轨迹线段
		*	vecParam  [in]  新的轨迹线段的容器
		*/
		virtual void		iUpdateLocusLineSegment(const std::vector<LocusLineSegment>& vecParam) = 0;
	};
} // namespace bc


