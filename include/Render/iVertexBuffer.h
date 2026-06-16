#pragma once

namespace bc
{
	enum AttributeType_e
	{
		ATTRIB_VERTEX = 0,
		ATTRIB_TEXCOORD0,
		ATTRIB_TEXCOORD1,
		ATTRIB_TEXCOORD2,
		ATTRIB_TEXCOORD3,
		ATTRIB_TEXCOORD4,
		ATTRIB_COLOR,
		ATTRIB_NORMAL,
		ATTRIB_TANGENT,
		ATTRIB_BINORMAL,
		ATTRIB_BONE_INDEX_START,
		ATTRIB_BONE_INDEX_END = ATTRIB_BONE_INDEX_START + MAX_EFFECT_BONE - 1,
		ATTRIB_BONE_WEIGHT_START,
		ATTRIB_BONE_WEIGHT_END = ATTRIB_BONE_WEIGHT_START + MAX_EFFECT_BONE - 1,
		ATTRIB_MATRIX,
		ATTRIB_PARTICLE,
		ATTRIB_COUNT
	};

	enum AtrributeMapType_e
	{
		MAP_TYPE_READ = 1,
		MAP_TYPE_WRITE,
		MAP_TYPE_READ_WRITE
	};

	struct VBOParam
	{
		VBOParam(AttributeType_e eType, void* pBuffer, int nBufferSize, int nStrideSize) :
			_eType(eType), _pBufferPtr(pBuffer), _nBufferSize(nBufferSize),
			_nStrideSize(nStrideSize), _bStaticData(true),
			_bNormalize(false), _bDivisor(false)
		{

		}

		VBOParam() :_eType(AttributeType_e::ATTRIB_VERTEX), _pBufferPtr(NULL), _nBufferSize(0), _nStrideSize(3), _bStaticData(true),
			_bNormalize(false), _bDivisor(false)
		{

		}
		AttributeType_e _eType;
		const void* _pBufferPtr;
		bool  _bStaticData;
		int _nBufferSize;
		int _nStrideSize;
		bool _bDivisor;
		bool _bNormalize;

	};

	class IVertexBuffer
	{
	public:
		virtual void	iCreateVertexBuffer(const VBOParam& tVBOParam) = 0;
		virtual void	iCreateElementArrayBuffer(const void* pBufferPtr, const int& nBufferSize) = 0;
		virtual bool	iUpdateVertexBuffer(const AttributeType_e& eType, const void* pBuffer, const int& nBufferSize) = 0;
		virtual bool	iUpdateElementBuffer(const void* pBufferPtr, const int& nBufferSize) = 0;
		virtual void*	iMapVertexBuffer(const AttributeType_e& eType, const AtrributeMapType_e& eRW = AtrributeMapType_e::MAP_TYPE_READ) = 0;
		virtual void	iUnMapVertexBuffer(const AttributeType_e& eType) = 0;
		virtual int*	iMapElementBuffer(const AtrributeMapType_e& eRW = AtrributeMapType_e::MAP_TYPE_READ) = 0;
		virtual void	iUnMapElementBuffer() = 0;
		virtual int		iGetVertexCount() const = 0;
		virtual int		iGetElemCount() const = 0;
		virtual int		iGetChannelCount() const = 0;
	};
}

