#pragma once

#define DEFAULT_TEXTURE_NAME		"defaultTexture"
#define SPHERICAL_HARMONIC_COUNT	9
#define TEXTURE_LOAD_PRIORITY		0

namespace bc
{
	enum class TextureType_e
	{
		TEXTYPE_RGBA_2D = 0,
		TEXTYPE_RGBA_2D_ARRAY,
		TEXTYPE_RGBA_2D_ANIMMAP,
		TEXTYPE_RGBA_3D,
		TEXTYPE_CUBEMAP,
		TEXTYPE_CUBE_ARRAY,
		TEXTYPE_BUFFER,
		TEXTYPE_HDR
	};

	enum class TextureLoadState_e
	{
		WaitForRead,
		Reading,
		WaitForCompile,
		Loaded
	};

	enum class TextureDownloadState_e
	{
		Unkown = 0,
		WaitForDownload,
		Downloading,
		Success,
		Failed,
	};

	typedef struct SphericalHarmonicChannel_s
	{
		float	coeffs[SPHERICAL_HARMONIC_COUNT] = { 0 };

		SphericalHarmonicChannel_s& operator=(const SphericalHarmonicChannel_s& rhs)
		{
			for (int i = 0; i < SPHERICAL_HARMONIC_COUNT; ++i)
			{
				coeffs[i] = rhs.coeffs[i];
			}

			return *this;
		}
	}SphericalHarmonicChannel;

	typedef struct TextureCubePosition_s
	{
		std::string		strPos;
		uint			nIndex;
	}TextureCubePosition;

	typedef struct TextureParam_s
	{
		TextureType_e	eTexType;
		std::string		strName;
		std::vector<std::string>	vecStrFile;
		uint			nMinFilter;
		uint			nMagFilter;
		int				nWidth;
		int				nHeight;
		int				nDepth;
		bool			bMipMap;
		// 各项异性过滤
		bool			bAnisotropy;
		bool			bLoadFile;
		std::string		strUrl;
		uint			nWrapMode;
		uint			nImageDataType;
		int				nBits;
		byte*			pPixels;
		uint			nFormat;
		uint			nInternalFormat;
		bool			bDepth;
		bool			bPBO;
		bool			bMultisample;
		Vector4			vBorderColor;
		bool			bCompileIM;
		int				nTexArraySize;
		float			fFPS;
		byte*			pData;
		long			lDataSize;
		int             nDownLoadPriority;
		int             nDownLoadLevel;

		TextureParam_s() :
			TextureParam_s("")
		{

		}

		TextureParam_s(std::string _strName, TextureType_e _eType = TextureType_e::TEXTYPE_RGBA_2D,
			uint _eMode = GL_CLAMP_TO_EDGE, uint _nMinFilter = GL_LINEAR, uint _nMagFilter = GL_LINEAR,
			int _nWidth = 8, int _nHeight = 8, int _nDepth = 1, int _nBits = 1)
		{
			strName = _strName;
			eTexType = _eType;
			nWrapMode = _eMode;
			nMinFilter = _nMinFilter;
			nMagFilter = _nMagFilter;
			nWidth = _nWidth;
			nHeight = _nHeight;
			nDepth = _nDepth;
			nBits = _nBits;
			pPixels = nullptr;

			bMipMap = false;
			bAnisotropy = false;
			bLoadFile = false;
			nFormat = GL_RGBA;
			nInternalFormat = GL_RGBA;
			nImageDataType = GL_UNSIGNED_BYTE;

			bDepth = false;
			bPBO = false;
			bMultisample = false;
			vBorderColor = Vector4(1.0, 1.0, 1.0, 1.0);
			bCompileIM = false;
			nTexArraySize = 1;
			fFPS = 1;
			pData = nullptr;
			lDataSize = 0;
			nDownLoadPriority = TEXTURE_LOAD_PRIORITY;
			nDownLoadLevel = 1;
		}

		TextureParam_s& operator=(const TextureParam_s& rhs)
		{
			eTexType = rhs.eTexType;
			strName = rhs.strName;
			vecStrFile = rhs.vecStrFile;
			nMinFilter = rhs.nMinFilter;
			nMagFilter = rhs.nMagFilter;
			nWidth = rhs.nWidth;
			nHeight = rhs.nHeight;
			nDepth = rhs.nDepth;
			bMipMap = rhs.bMipMap;
			bAnisotropy = rhs.bAnisotropy;
			bLoadFile = rhs.bLoadFile;
			strUrl = rhs.strUrl;
			nWrapMode = rhs.nWrapMode;
			nImageDataType = rhs.nImageDataType;
			nBits = rhs.nBits;
			pPixels = rhs.pPixels;
			nFormat = rhs.nFormat;
			nInternalFormat = rhs.nInternalFormat;
			bDepth = rhs.bDepth;
			bPBO = rhs.bPBO;
			bMultisample = rhs.bMultisample;
			vBorderColor = rhs.vBorderColor;
			bCompileIM = rhs.bCompileIM;
			nTexArraySize = rhs.nTexArraySize;
			fFPS = rhs.fFPS;
			pData = rhs.pData;
			lDataSize = rhs.lDataSize;
			nDownLoadPriority = rhs.nDownLoadPriority;
			nDownLoadLevel = rhs.nDownLoadLevel;

			return *this;
		}
	}TextureParam;

	class ITexture
	{
	public:
		virtual std::string			iGetName() = 0;
		virtual uint				iGetLoadState() = 0;
		virtual bool				iCompile() = 0;

		virtual void				iBindTexture() = 0;
		virtual void				iUpdateTexture(byte* pData, const int& nWidth, const int& nHeight) = 0;
		virtual void				iGenerateMipmap(const int& nMax = -1, const int& nMin = -1) = 0;
	
		virtual void				iUpdateWrap(const uint& nWrap) = 0;
		virtual void				iUpdateFilter(const uint& nMin, const uint& nMag) = 0;

		virtual void				iStartAnimap() = 0;
		virtual void				iStopAnimap() = 0;

		virtual void				iSetArrayMapIndex(const int& nIndex) = 0;
		virtual int					iGetArrayMapIndex() = 0;

		virtual unsigned int		iGetTexTarget() = 0;
		virtual unsigned int		iGetTexObject() = 0;
		virtual unsigned int		iGetTexHandle() = 0;
		virtual void				iSetLoadTaskID(const size_t& nID) = 0;
		virtual size_t				iGetLoadTaskID() const = 0;

		virtual const TextureParam& iGetTextureParam() = 0;

		virtual SphericalHarmonicChannel*	iGetSphericalHarmonicChannel() = 0;
		virtual void				iGenerateSphericalHarmonic() = 0;
		virtual bool				iSave(const std::string& strFilePath, const bool& bUpsideDown = true, const Vector4& vColor = Vector4(1.0)) = 0;

		virtual int					iGetComponent(const int& nFace = 0, const int& nLevel = 0) = 0;
		virtual byte*				iGetBytePixel(const int& nFace = 0, const int& nLevel = 0, bool bUpsideDown = false) = 0;
		virtual float*				iGetFloatPixel(const int& nFace = 0, const int& nLevel = 0, bool bUpsideDown = false) = 0;
		virtual TextureDownloadState_e  iGetDownloadState() = 0;
	};
}
