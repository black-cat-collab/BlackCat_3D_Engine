#pragma once

namespace bc
{
	
	class EnergyParam : public DynamicLoadNodeParam
	{
	public:
		int32_t nSize;
		float     fMax;
		float     fMin;
		float     fStep;
		Vector2 vPosition;
		Vector2 vRectSize;
		std::string strEnergyFilePath;
		EnergyParam() : DynamicLoadNodeParam()
		{
			nNodeType = NodeType_e::NODE_ENERGY;
			nSize = 10;
			fMax = 100.0f;
			fMin = 0.0f;
			fStep = 10.0f;
			vPosition = { 0.8f, 0.5f };
			vRectSize = { 0.4f, 0.4f };
			strEnergyFilePath = "";
		}
		EnergyParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);
			return *this;
		}

		EnergyParam& operator=(const DynamicLoadNodeParam& rhs)
		{
			DynamicLoadNodeParam::operator=(rhs);
			return *this;
		}

		EnergyParam& operator=(const EnergyParam& rhs)
		{
			DynamicLoadNodeParam::operator=(rhs);
			nSize = rhs.nSize;
			fMax = rhs.fMax;
			fMin = rhs.fMin;
			fStep = rhs.fStep;
			vPosition = rhs.vPosition;
			vRectSize = rhs.vRectSize;
			strEnergyFilePath = rhs.strEnergyFilePath;
			return *this;
		}
	};

	class IEnergy : public IComponent
	{
	public:
		virtual ~IEnergy() = default;
		virtual int	 iGetComponentType() { return COMPONENT_TYPE_ENERGY; }
		virtual void	iGetEnergyParam(EnergyParam& tParam) = 0;
		virtual void	iSetEnergyParam(const EnergyParam& tParam) = 0;
		virtual void  iBuild() = 0;
	};
}

