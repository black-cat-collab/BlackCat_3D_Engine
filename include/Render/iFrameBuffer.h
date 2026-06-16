#pragma once

#define RBO_COLOR_BUFFER_MAX	16

namespace bc
{
	enum class FrameBufferType_e
	{
		FRAME_BUFFER_TEXTURE = 0,
		FRAME_BUFFER_TEXTURE_SHADOW
	};

	typedef struct FrameBufferMipmap_s
	{
		int nHeight;
		int nWidth;

		FrameBufferMipmap_s() : 
			nHeight(128), 
			nWidth(128)
		{

		}
	}FrameBufferMipmap;

	typedef struct FrameBufferParam_s
	{
		FrameBufferType_e	eType;
		std::string			strID;
		int					nSamples;
		int					nRBOWidth;
		int					nRBOHeight;
		bool				bDepthRBO;
		int					nColorRBO;
		bool				bColorRBO;
		uint				nColorRBOFormat[RBO_COLOR_BUFFER_MAX];
		uint				nFormat;
		uint				nAttachment;
		std::vector<FrameBufferMipmap*> vecMipmap;
		FrameBufferParam_s() :
			eType(FrameBufferType_e::FRAME_BUFFER_TEXTURE),
			nRBOWidth(1), nRBOHeight(1), nSamples(0), 
			bDepthRBO(false), nColorRBO(1), bColorRBO(false),
			nFormat(GL_DEPTH_COMPONENT32F),
			nAttachment(GL_DEPTH_ATTACHMENT)
		{
			for (int i = 0; i < RBO_COLOR_BUFFER_MAX; ++i)
			{
				nColorRBOFormat[i] = GL_RGBA16F_ARB;
			}
		}

		~FrameBufferParam_s()
		{
			ClearMipmap();
		}

		void AddMipmap(const int& nWidth, const int& nHeight)
		{
			FrameBufferMipmap* pMipmap = new FrameBufferMipmap;
			pMipmap->nWidth = nWidth;
			pMipmap->nHeight = nHeight;
			vecMipmap.push_back(pMipmap);
		}

		void ClearMipmap()
		{
			for (size_t i = 0; i < vecMipmap.size(); ++i)
			{
				delete vecMipmap[i];
			}
			vecMipmap.clear();
		}
	}FrameBufferParam;

	class IFrameBuffer
	{
	public:
		// ID
		virtual std::string	iGetID() = 0;
		virtual bool	iBindTexture(ITexture* pTexture, const uint& nAttachment = 0, const int& nMipmap = 0) = 0;
		virtual void	iSetCurrentLayer(const int& nIndex) = 0;
		virtual int		iGetCurrentLayer() = 0;
	};
}

