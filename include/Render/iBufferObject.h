#pragma once

namespace bc
{
	struct BufferObjectParam
	{
		const void* pData;
		size_t		nSize;
		size_t		nOffset;
		uint		nFlag;
		
		BufferObjectParam() :
			nSize(0), pData(nullptr), nFlag(0), nOffset(0)
		{

		}
		
	};

	typedef enum BufferObjectMapType_e
	{
		BUFFER_OBJECT_MAP_TYPE_READ = 1,
		BUFFER_OBJECT_MAP_TYPE_WRITE,
		BUFFER_OBJECT_MAP_TYPE_READ_WRITE
	}BufferObjectMapType;


	class IBufferObject
	{
	public:
		virtual void	iCreate(BufferObjectParam* pParam) = 0;
		virtual bool	iUpdate(BufferObjectParam* pParam) = 0;
		virtual void*	iMap(const size_t& nSize = 0, const size_t& nOffset = 0, const BufferObjectMapType& eRW = BufferObjectMapType::BUFFER_OBJECT_MAP_TYPE_READ) = 0;
		virtual void	iUnMap() = 0;
		virtual size_t	iGetSize() = 0;
	};
}

