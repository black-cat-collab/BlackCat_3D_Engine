#pragma once

namespace bc
{
	typedef struct BCDataBuffer_s
	{
		char* pBuffer;
		int   nSize;
		int   nLength;

		BCDataBuffer_s(const bool& _bAllocate = false, const int& _nSize = LENGTH_1K)
		{
			if (_bAllocate)
			{
				pBuffer = new char[_nSize];
				memset(pBuffer, 0, _nSize);
				nSize = _nSize;
			}
			else
			{
				pBuffer = nullptr;
				nSize = 0;
			}
			nLength = 0;
		}

		~BCDataBuffer_s()
		{
			DELETE_ARRAY(pBuffer);
		}

		int SetLength(const int& nLen)
		{
			if (nLen < 1)
			{
				return STATUS_BAD_PARAMS;
			}

			DELETE_ARRAY(pBuffer);
			pBuffer = new char[nLen];
			if (!pBuffer)
			{
				return STATUS_NEW;
			}
			memset(pBuffer, 0, nLen);

			nSize = nLen;
			nLength = nLen;

			return STATUS_SUCCESS;
		}

		int GetBuffer(const int& nDataLength)
		{
			if (nDataLength + nDataLength % 4 > nSize)
			{
				DELETE_ARRAY(pBuffer);
				nSize = nDataLength + nDataLength % 4;
				pBuffer = new char[nSize];
				if (!pBuffer)
				{
					return STATUS_NEW;
				}
			}

			return STATUS_SUCCESS;
		}

		int SetData(const char* pData, const int& nDataLength)
		{
			int nRet = STATUS_SUCCESS;

			if (!pData)
			{
				return STATUS_BAD_PARAMS;
			}

			nRet = GetBuffer(nDataLength);
			if (nRet != STATUS_SUCCESS)
			{
				return nRet;
			}

			memset(pBuffer, 0, nSize);
			memcpy(pBuffer, pData, nDataLength);
			nLength = nDataLength;

			return STATUS_SUCCESS;
		}

		struct BCDataBuffer_s& operator = (const struct BCDataBuffer_s& rhs)
		{
			int nRet = STATUS_SUCCESS;

			nRet = GetBuffer(rhs.nLength);
			if (nRet != STATUS_SUCCESS)
			{
				return *this;
			}

			if (pBuffer)
			{
				memset(pBuffer, 0, nSize);
				memcpy(pBuffer, rhs.pBuffer, rhs.nLength);
				nLength = rhs.nLength;
			}

			return *this;
		}

	}BCDataBuffer;

	enum EServerServion
	{
		SERVER_VERSION_LESS_V10 = 0,		//小于v10版本
		SERVER_VERSION_V10,					//后台服务v10版本
	};

	class IProtocolAPI
	{
	public:
		/*
		 *	初始化
		 *  pSystemAPI	[in] 系统接口指针
		 */
		virtual void				iInitialize(ISystemAPI* pSystemAPI) = 0;

		virtual ILocationManager*	iGetLocationManager() = 0;

		virtual INetworkManager*	iGetNetworkManager() = 0;

		virtual IMonitorManager*	iGetMonitorManager() = 0;

		virtual ICloudRenderManager*	iGetCloudRenderManager() = 0;

		virtual ISoftwareLockManager* iGetSoftwareLockManager() = 0;
		//设置后台版本
		virtual void				  iSetServerVersion(EServerServion eVersion) = 0;
		//获取后台版本
		virtual EServerServion		  iGetServerVersion() = 0;
		
	};
}
