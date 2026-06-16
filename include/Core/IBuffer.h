#pragma once

namespace bc
{
	enum class BufferType
	{
		Buffer_Point,
		Buffer_Line,
		Buffer_Face
	};
	enum class ClipType
	{
		Union, Diffrent, Intersect
	};

	enum class RuleType
	{
		EvenOdd, NonZero, Positive, Negative
	};
	class BufferParam : public DynamicLoadNodeParam
	{
	public:
		std::vector<Vector3> vecPoint;
		Vector4                       vFaceColor;
		Vector4                       vPointColor;
		float                            fRadius;
		float                            fPointSize;
		RuleType                     eRuleType;
		BufferType                  eType;
		ClipType                     eClipType;

		BufferParam() : DynamicLoadNodeParam()
		{
			nNodeType = NodeType_e::NODE_BUFFER;
			vFaceColor = Vector4(1.0);
			vPointColor = Vector4(1.0);
			fRadius = 0.0f;
			fPointSize = 1.0f;
			eType = BufferType::Buffer_Point;
			eClipType = ClipType::Union;
			eRuleType = RuleType::NonZero;
		}
		BufferParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);
			return *this;
		}

		BufferParam& operator=(const DynamicLoadNodeParam& rhs)
		{
			DynamicLoadNodeParam::operator=(rhs);
			return *this;
		}

		BufferParam& operator=(const BufferParam& rhs)
		{
			DynamicLoadNodeParam::operator=(rhs);
			vFaceColor = rhs.vFaceColor;
			vPointColor = rhs.vPointColor;
			fRadius = rhs.fRadius;
			fPointSize = rhs.fPointSize;
			eRuleType = rhs.eRuleType;
			eType = rhs.eType;
			eClipType = rhs.eClipType;
			vecPoint.assign(rhs.vecPoint.begin(), rhs.vecPoint.end());
			return *this;
		}
	};

	class IBuffer : public IComponent
	{
	public:
		virtual ~IBuffer() = default;
		virtual int	 iGetComponentType() { return COMPONENT_TYPE_BUFFER; }
		virtual void	iGetShellParam(BufferParam& tParam) = 0;
		virtual void	iSetShellParam(const BufferParam& tParam) = 0;
	};
}

