#pragma once

#define _OPENGL_330               1
#define _OPENGL_400               2
#define _OPENGL_430               3
#define _OPENGL_450               4
#define _OPENGL_460               5

#define OPENGL_VERSION          _OPENGL_450  
namespace bc
{


	enum RenderCommandType_e
	{
		RENDERCMD_CLEAR,
		RENDERCMD_SWAP,
		RENDERCMD_GL_FINISH,
		RENDERCMD_BEGIN_FRAME_BUFFER,
		RENDERCMD_END_FRAME_BUFFER,
		RENDERCMD_BLIT_FRAME_BUFFER,
		RENDERCMD_SCENE_PROJECTION,
		RENDERCMD_CAMERA_PROJECTION,
		RENDERCMD_CAMERA_EARTH_PROJECTION,
		RENDERCMD_PROJECTION_ORTHO,
		RENDERCMD_PROJECTION_OVERLAY,
		RENDERCMD_MODEL_MATRIX,
		RENDERCMD_MODEL_MATRIX_IDENTITY,
		RENDERCMD_ORTHO_MESH,
		RENDERCMD_BBOX,
		RENDERCMD_LINE,
		RENDERCMD_POINT,
		RENDERCMD_QUAD,
		RENDERCMD_FONT,
		RENDERCMD_FRUSTUM,
		RENDERCMD_BASE_MAP,
		RENDERCMD_LIGHT_MAP,
		RENDERCMD_COLOR_MESH,
		RENDERCMD_INSTANCE,
		RENDERCMD_FRONT_RENDER,
		RENDERCMD_G_BUFFER,
		RENDERCMD_G_BUFFER_DEPTH,
		RENDERCMD_CAST_SHADOW,
		RENDERCMD_SSSHADOW,
		RENDERCMD_DEFFERED_SHADING,
		RENDERCMD_FRONT_RENDER_MSAA,
		RENDERCMD_FRONT_RENDER_TRANSPARENT,
		RENDERCMD_FRONT_RENDER_LIGHTING,
		RENDERCMD_FRONT_RENDER_NO_LIGHTING,
		RENDERCMD_FRONT_RENDER_NO_LIGHTING_TRANSPARENT,
		RENDERCMD_FRONT_RENDER_OFFSET_NO_LIGHTING_TRANSPARENT,
		RENDERCMD_DEFFERED_LIGHTING,
		RENDERCMD_AVERAGE_TRANSPARENT,
		RENDERCMD_AVERAGE_TRANSPARENT_BLEND,
		RENDERCMD_TXAA,
		RENDERCMD_DYNAMIC_SKY,
		RENDERCMD_GAUSS_BLUR_H,
		RENDERCMD_GAUSS_BLUR_V,
		RENDERCMD_CROSS_BLUR_H,
		RENDERCMD_CROSS_BLUR_V,
		RENDERCMD_RADIAL_BLUR,
		RENDERCMD_BLOOM_HDR,
		RENDERCMD_BLOOM_BLEND,
		RENDERCMD_CROSS_FLARE_HDR,
		RENDERCMD_CROSS_FLARE_BLEND,
		RENDERCMD_GLOW_DEPTH,
		RENDERCMD_GLOW,
		RENDERCMD_REFLECT_BASE,
		RENDERCMD_REFLECT_LIGHTING,
		RENDERCMD_SUN_GOD_RAY_SKY,
		RENDERCMD_SUN_GOD_RAY_RADIAL_BLUR,
		RENDERCMD_SUN_GOD_RAY_BLEND,
		RENDERCMD_DEPTH_OF_FILED,
		RENDERCMD_SSAO,
		RENDERCMD_SSAO_BLEND,
		RENDERCMD_GTAO,
		RENDERCMD_IBL_IRRADIANCE,
		RENDERCMD_IBL_PREFILTER,
		RENDERCMD_IBL_BRDF,
		RENDERCMD_AVERAGE_LUMINANCE,
		RENDERCMD_TONE_MAPPING,
		RENDERCMD_COLOR_FILTER,
		RENDERCMD_HEATMAP_POINT,
		RENDERCMD_POLYGON_MESH,
		RENDERCMD_VIEWSHED_ANALYSIS,
		RENDERCMD_RGB_SPLIT_GLITCH,
		RENDERCMD_SET_SHADOW_PARAM,
		RENDERCMD_PARTICLE_FEEDBACK,
		RENDERCMD_PARTICLE,
		RENDERCMD_PARTICLE_SWAP,
		RENDERCMD_DEPTH,
		RENDERCMD_CUSTOM_DEPTH,
		RENDERCMD_DOWN_SAMPLE_BLUR,
		RENDERCMD_DOWN_SAMPLE_HIZ_MAX,
		RENDERCMD_DOWN_SAMPLE_HIZ_MIN,
		RENDERCMD_SSR,
		RENDERCMD_YUV12,
		RENDERCMD_YUV12_GAMMA_CORRECT,
		RENDERCMD_BLUR_MASK,
		RENDERCMD_DRAW_BORDER,
		RENDERCMD_DRAW_SOBEL_EDGE,
		RENDERCMD_DRAW_GRID,
		RENDERCMD_BORDER_BLEND,
		RENDERCMD_DEFFERED_COLOR_DEPTH,
		RENDERCMD_SDF_FONT,
		RENDERCMD_UI_WIDGET,
		RENDERCMD_HDR_CUBE,
		RENDERCMD_UPDATE_INDIRECT_COMMAND,
		RENDERCMD_UPDATE_MULTI_INDIRECT_COMMAND,
		RENDERCMD_DRAW_VERTEX,
		RENDERCMD_OFFSET_MESH,
		RENDERCMD_LINE_MESH,
	};

	typedef enum SplitDiscardType_e
	{
		SPLIT_DISCARD_NONE = 0,
		SPLIT_DISCARD_NEGATIVE,
		SPLIT_DISCARD_POSITIVE
	}SplitDiscardType;

	typedef struct DrawElementsIndirectCmd_s {
		GLuint  nCount;
		GLuint  nInstanceCount;
		GLuint  nFirstIndex;
		GLuint  nBaseVertex;
		GLuint  nBaseInstance;
	}DrawElementsIndirectCmd;

	typedef struct RenderBaseCmd_s
	{
		RenderCommandType_e	eType;
	}RenderBaseCmd;

	typedef struct RenderClearCmd_s
	{
		RenderCommandType_e	eType;
		uint				nState;
		Vector4				vColor;

		RenderClearCmd_s() :
			vColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f)),
			eType(RENDERCMD_CLEAR),
			nState(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		{

		}
	}RenderClearCmd;

	typedef struct RenderSwapCmd_s
	{
		RenderCommandType_e	eType;

		RenderSwapCmd_s() :
			eType(RENDERCMD_SWAP)
		{

		}
	}RenderSwapCmd;

	typedef struct RenderGLFinishCmd_s
	{
		RenderCommandType_e	eType;

		RenderGLFinishCmd_s() :
			eType(RENDERCMD_GL_FINISH)
		{

		}
	}RenderGLFinishCmd;

	typedef struct RenderFrameBufferCmd_s
	{
		RenderCommandType_e	eType;
		IFrameBuffer*		pFrameBuffer;
		RenderFrameBufferCmd_s() :
			pFrameBuffer(nullptr),
			eType(RENDERCMD_BEGIN_FRAME_BUFFER)
		{

		}
	}RenderFrameBufferCmd;

	typedef struct RenderBlitFrameBufferCmd_s
	{
		RenderCommandType_e	eType;
		uint				uState;
		IFrameBuffer*		pReadFBO;
		IFrameBuffer*		pDrawFBO;
		int					nSrcX0;
		int					nSrcY0;
		int					nSrcX1;
		int					nSrcY1;
		int					nDstX0;
		int					nDstY0;
		int					nDstX1;
		int					nDstY1;
		int					nIndex;

		RenderBlitFrameBufferCmd_s() :
			nSrcX0(0),
			nSrcY0(0),
			nSrcX1(0),
			nSrcY1(0),
			nDstX0(0),
			nDstY0(0),
			nDstX1(0),
			nDstY1(0),
			nIndex(0),
			pReadFBO(nullptr),
			pDrawFBO(nullptr),
			uState(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT),
			eType(RENDERCMD_BLIT_FRAME_BUFFER)
		{

		}
	}RenderBlitFrameBufferCmd;

	typedef struct DottedLineStyle_s
	{
		int			nFactor;
		ushort		usPattern;
		DottedLineStyle_s() :
			nFactor(1),
			usPattern(0xFF00)
		{

		}
	}DottedLineStyle;

	typedef struct RenderLineCmd_s
	{
		RenderCommandType_e	eType;
		Vector3*			pPoint;
		Vector4				vColor;
		LineStyle_e			eStyle;
		float				fLineWidth;
		float				fFadeDistance;
		int					nNumPoint;
		bool				bDepthTest;
		DottedLineStyle		sDottedStyle;
		RenderLineCmd_s() :
			pPoint(nullptr),
			nNumPoint(0),
			bDepthTest(true),
			fLineWidth(2.0f),
			fFadeDistance(10.0f),
			eType(RENDERCMD_LINE),
			eStyle(LINE_STYLE_LINES),
			vColor(1.0f, 1.0f, 1.0f, 1.0f),
			sDottedStyle(DottedLineStyle())
		{

		}

	}RenderLineCmd;

	typedef struct RenderBBoxCmd_s
	{
		RenderCommandType_e	eType;
		Vector3				vMin;
		Vector3				vMax;
		Vector4				vColor;
		float				fLineWidth;
		bool				bDepthTest;
		RenderBBoxCmd_s() :
			fLineWidth(1.0f),
			bDepthTest(true),
			eType(RENDERCMD_BBOX),
			vMin(0.0f, 0.0f, 0.0f),
			vMax(0.0f, 0.0f, 0.0f),
			vColor(1.0f, 1.0f, 1.0f, 1.0f)	
		{

		}
	}RenderBBoxCmd;

	typedef struct RenderQuadCmd_s
	{
		RenderCommandType_e	eType;
		Vector3				vPoint[5];
		Vector4				vColor;
		float				fLineWidth;
		bool				bDepthTest;
		RenderQuadCmd_s() :
			fLineWidth(2.0f),
			bDepthTest(true),
			vColor(1.0f, 1.0f, 1.0f, 1.0f),
			eType(RENDERCMD_QUAD)
		{

		}
	}RenderQuadCmd;

	typedef struct RenderFontCmd_s
	{
		RenderCommandType_e	eType;
		int					nWidth;
		int					nHeight;
		bool				bUpsideDown;
		std::vector<TextShow>* pVecText;

		RenderFontCmd_s() :
			nWidth(0),
			nHeight(0),
			pVecText(nullptr),
			bUpsideDown(false),
			eType(RENDERCMD_FONT)
		{

		}
	}RenderFontCmd;

	typedef struct RenderSDFFontCmd_s
	{
		RenderCommandType_e	eType;
		int					nWidth;
		int					nHeight;
		bool				bUpsideDown;
		std::vector<SDFTextShow>* pVecText;

		RenderSDFFontCmd_s() :
			nWidth(0),
			nHeight(0),
			pVecText(nullptr),
			bUpsideDown(false),
			eType(RENDERCMD_SDF_FONT)
		{

		}
	}RenderSDFFontCmd;

	typedef struct RenderFrustumCmd_s
	{
		RenderCommandType_e	eType;
		Vector3				Left[4];
		Vector3				Right[4];
		Vector3				Far[5];
		Vector3				Near[5];
		Vector4				vColor;
		float				fLineWidth;

		RenderFrustumCmd_s() :
			fLineWidth(2.f),
			vColor(1.0f, 1.0f, 1.0f, 1.0f),
			eType(RENDERCMD_FRUSTUM)
		{

		}
	}RenderFrustumCmd;

	typedef struct RenderProjectionCmd_s
	{
		RenderCommandType_e	eType;
		INode*				pCamera;

		RenderProjectionCmd_s() :
			pCamera(nullptr),
			eType(RENDERCMD_CAMERA_PROJECTION)
		{

		}
	}RenderProjectionCmd;

	typedef struct RenderEarthProjectionCmd_s
	{
		RenderCommandType_e	eType;
		INode* pCamera;

		RenderEarthProjectionCmd_s() :
			pCamera(nullptr),
			eType(RENDERCMD_CAMERA_EARTH_PROJECTION)
		{

		}
	}RenderEarthProjectionCmd;

	typedef struct RenderOrthoProjectionCmd_s
	{
		RenderCommandType_e	eType;
		Vector4				vViewPort;
		Vector4				vOrtho;
		Vector2				vPlaneClip;

		RenderOrthoProjectionCmd_s() :
			vPlaneClip(Vector2(0.0f, 1.0f)),
			vOrtho(Vector4(-1.0f, 1.0f, -1.0f, 1.0f)),
			vViewPort(Vector4(0.0f, 0.0f, 1920.0f, 1080.0f)),
			eType(RENDERCMD_PROJECTION_ORTHO)
		{

		}
	}RenderOrthoProjectionCmd;

	typedef struct RenderOverlayCmd_s
	{
		RenderCommandType_e	eType;
		Vector4				vViewPort;
		float				fNear;
		float				fFar;

		RenderOverlayCmd_s() :
			fFar(1.0f),
			fNear(-1.0f),
			vViewPort(Vector4(0.0f, 0.0f, 1920.0f, 1080.0f)),
			eType(RENDERCMD_PROJECTION_OVERLAY)
		{

		}
	}RenderOverlayCmd;

	typedef struct RenderModelMatrixCmd_s
	{
		RenderCommandType_e	eType;
		Matrix4d				tMatrix;
		Matrix4d				tPreviousMatrix;
		RenderModelMatrixCmd_s() :
			tMatrix(Matrix4d::IDENTITY),
			tPreviousMatrix(Matrix4d::IDENTITY),
			eType(RENDERCMD_MODEL_MATRIX)
		{

		}
	}RenderModelMatrixCmd;

	//typedef struct RenderModelMatrixDoubleCmd_s
	//{
	//	RenderCommandType_e	eType;
	//	Matrix4d				tMatrix;
	//	Matrix4d				tPreviousMatrix;
	//	RenderModelMatrixDoubleCmd_s() :
	//		tMatrix(Matrix4d::IDENTITY),
	//		tPreviousMatrix(Matrix4d::IDENTITY),
	//		eType(RENDERCMD_MODEL_MATRIX_DOUBLE)
	//	{

	//	}
	//}RenderModelMatrixDoubleCmd;

	typedef struct RenderIdentityMatrixCmd_s
	{
		RenderCommandType_e	eType;

		RenderIdentityMatrixCmd_s() :
			eType(RENDERCMD_MODEL_MATRIX_IDENTITY)
		{

		}
	}RenderIdentityMatrixCmd;

	typedef struct RenderOrthoMeshCmd_s
	{
		RenderCommandType_e	eType;
		Vector4				vViewPort;
		IMesh*				pDrawMesh;
		Matrix4				tMatrix;

		RenderOrthoMeshCmd_s() :
			pDrawMesh(nullptr),
			tMatrix(Matrix4::IDENTITY),
			vViewPort(Vector4(0.0f, 0.0f, 1920.0f, 1080.0f)),
			eType(RENDERCMD_ORTHO_MESH)
		{

		}
	}RenderOrthoMeshCmd;

	typedef struct RenderColorMeshCmd_s
	{
		RenderCommandType_e	eType;
		Vector4				vColor;
		int					nLODIndex;
		int					nMeshCount;
		IMesh**				ppDrawMesh;
		RenderColorMeshCmd_s() :
			nLODIndex(0),
			nMeshCount(0),
			ppDrawMesh(nullptr),
			eType(RENDERCMD_COLOR_MESH),
			vColor(0.949f, 0.988f, 1.0f, 0.3f)
		{

		}
	}RenderColorMeshCmd;

	typedef struct RenderLineMeshCmd_s
	{
		RenderCommandType_e	eType;
		Vector4				vColor;
		int					nLODIndex;
		int					nMeshCount;
		LineStyle_e			eStyle;
		float				fLineWidth;
		float				fFadeDistance;
		bool                bDepthTest;
		DottedLineStyle		sDottedStyle;
		IMesh** ppDrawMesh;
		RenderLineMeshCmd_s() :
			nLODIndex(0),
			nMeshCount(0),
			ppDrawMesh(nullptr),
			eType(RENDERCMD_LINE_MESH),
			bDepthTest(true),
			eStyle(LINE_STYLE_LINES),
			vColor(1.0f, 1.0f, 1.0f, 1.0f),
			sDottedStyle(DottedLineStyle()),
			fLineWidth(2.0f),
			fFadeDistance(10.0f)
		{

		}
	}RenderLineMeshCmd;

	typedef struct RenderUpdateMultiIndirectCmd_t
	{
		RenderCommandType_e	eType;
		int					nMeshCount;
		IMesh**				ppDrawMesh;
		std::vector<DrawElementsIndirectCmd>* pIndirectCommand;
		RenderUpdateMultiIndirectCmd_t() :
			nMeshCount(0),
			ppDrawMesh(nullptr),
			pIndirectCommand(nullptr),
			eType(RENDERCMD_UPDATE_MULTI_INDIRECT_COMMAND)
		{

		}
	}RenderUpdateMultiIndirectCmd;

	typedef struct RenderUpdateIndirectCmd_t
	{
		RenderCommandType_e	eType;
		IMesh*				pMesh;
		std::vector<DrawElementsIndirectCmd>* pIndirectCommand;
		RenderUpdateIndirectCmd_t() :
			pMesh(nullptr),
			pIndirectCommand(nullptr),
			eType(RENDERCMD_UPDATE_INDIRECT_COMMAND)
		{

		}
	}RenderUpdateIndirectCmd;

	typedef struct RenderMeshCmd_t
	{
		RenderCommandType_e	eType;
		int					nLODIndex;
		int					nMeshCount;
		IMesh**				ppDrawMesh;
		bool				bOcclusion;
		bool				bIndirect;
		RenderMeshCmd_t() :
			nLODIndex(0),
			nMeshCount(0),
			bOcclusion(false),
			bIndirect(false),
			ppDrawMesh(nullptr),
			eType(RENDERCMD_BASE_MAP)
		{

		}
	}RenderMeshCmd;

	typedef struct RenderOffsetMeshCmd_t
	{
		RenderCommandType_e	eType;
		int					nLODIndex;
		int					nMeshCount;
		IMesh** ppDrawMesh;
		bool				bOcclusion;
		bool				bIndirect;
		int*               ppOffset;
		int*                ppSize;
		RenderOffsetMeshCmd_t() :
			nLODIndex(0),
			nMeshCount(0),
			ppOffset(nullptr),
			ppSize(nullptr),
			bOcclusion(false),
			bIndirect(false),
			ppDrawMesh(nullptr),
			eType(RENDERCMD_OFFSET_MESH)
		{

		}
	}RenderOffsetMeshCmd;

	typedef struct RenderInstanceCmd_s
	{
		RenderCommandType_e	eType;
		RenderCommandType_e	eCurrentRenderType;
		int					nLODIndex;
		int					nMeshCount;
		int					nInstanceCount;
		IMesh**				ppDrawMesh;

		RenderInstanceCmd_s() :
			nLODIndex(0),
			nMeshCount(0),
			nInstanceCount(0),
			ppDrawMesh(nullptr),
			eType(RENDERCMD_INSTANCE),
			eCurrentRenderType(RENDERCMD_BASE_MAP)
		{

		}
	}RenderInstanceCmd;

	typedef struct RenderSetShadowParamCmd_t
	{
		RenderCommandType_e	eType;
		ShadowParam			tShadowParam;

		RenderSetShadowParamCmd_t() :
			eType(RENDERCMD_SET_SHADOW_PARAM)
		{

		}
	}RenderSetShadowParamCmd;

	class IRenderAPI
	{
	public:
		virtual void			iInitialize(ISystemAPI* pSystemAPI) = 0;
		virtual void          iRelease() = 0;
		virtual void			iExecuteRenderCommand() = 0;
		virtual bool			iPushRenderCommand(void* pCommandPtr, const int& nCommandSize) = 0;
		virtual void			iBeginBuffer(IFrameBuffer* pFrameBuffer) = 0;
		virtual void			iEndBuffer() = 0;
		virtual void			iSetSunPosition(const Vector3& vPos) = 0;
		virtual const Vector3&	iGetSunPosition() = 0;
		virtual void			iSetMoonPosition(const Vector3& vPos) = 0;
		virtual const Vector3&	iGetMoonPosition() = 0;
		virtual void			iSetSkyLightColor(const Vector3& vColor) = 0;
		virtual const Vector3&	iGetSkyLightColor() = 0;
		virtual void			iSetSkyLightIntensity(const float& fIntensity) = 0;
		virtual const float&	iGetSkyLightIntensity() = 0;
		virtual void			iSetSkyLightDirection(const Vector3& vDirection) = 0;
		virtual const Vector3&	iGetSkyLightDirection() = 0;
		virtual const bool      iGetSurround() = 0;
		virtual const void      iSetSurround(bool bSurround) = 0;
		virtual void			iAddDynamicLight(const MaterialLight& tMaterialLight) = 0;
		virtual int				iGetDynamicLightCount() = 0;
		virtual void			iClearDynamicLight() = 0;
		virtual void			iAddStationaryLight(const MaterialLight& tMaterialLight) = 0;
		virtual int				iGetStationaryLightCount() = 0;
		virtual void			iClearStationaryLight() = 0;
		virtual void			iClearGBufferShadowLight() = 0;
		virtual void			iAddGBufferShadowLight(GBufferShadowLight& tLight) = 0;
		virtual void			iAddCubeReflect(const MaterialCubeReflect& tMaterialReflect) = 0;
		virtual int				iGetCubeReflectCount() = 0;
		virtual void			iClearCubeReflect() = 0;
		virtual void			iUpdateSceneFog() = 0;
		virtual void			iUpdateStationaryLight() = 0;
		virtual void			iSetShadowParam(const ShadowParam& tParam) = 0;
		virtual void			iSetViewshedAnalysisParam(const ViewshedAnalysisParam& tParam) = 0;
		virtual void			iSetVideoFusionParam(const VideoFusionParam& tParam) = 0;
		virtual IMesh*			iCreateMesh() = 0;
		virtual IMesh*          iCreateBIMMesh() = 0 ;
		virtual IMesh*			iCreateMergeMesh(const std::vector<IMesh*> vecMesh) = 0;
		virtual IMesh*			iCreateMergeBIMMesh(const std::vector<IMesh*>& vecMesh, bool hasUV1 = false) = 0;
		virtual IMesh*		iCreateMergeCubeMesh(const std::vector<IMesh*>& vecMesh) = 0;
		virtual IMesh*			iCreateBIMMesh(const std::vector<Triangle>& vecTris, bool hasUV1 = false) = 0;
		virtual IMesh*			iCreateMergeMesh(const std::vector<std::pair<IMesh*, Matrix4>> vecMesh) = 0;
		virtual IMaterial*		iRegistMaterial(const MaterialParam& tParam) = 0;
		virtual void			iFreeMaterial(IMaterial** ppMaterial) = 0;
		virtual ITexture*		iRegistTexture(const TextureParam& tParam) = 0;
		virtual ITexture*		iFindTexture(const std::string& strID) = 0;
		virtual void			iFreeTexture(ITexture** ppTexture, const bool& bImmediately = false) = 0;
		virtual void			iFreeTexture(const std::string& strImage, const bool& bImmediately = false) = 0;
		virtual IFrameBuffer*	iRegistFrameBuffer(const FrameBufferParam& tParam) = 0;
		virtual void			iFreeFrameBuffer(IFrameBuffer** ppFrameBuffer) = 0;
		virtual void			iFreeMesh(IMesh** ppMesh) = 0;
		virtual void			iSetCurrentModelMatrix(const Matrix4d& m) = 0;
		virtual void			iSetCurrentViewMatrix(const Matrix4d& m) = 0;
		virtual void			iSetCurrentModelViewMatrix(const Matrix4d& m) = 0;
		virtual void			iSetCurrentProjectionMatrix(const Matrix4d& m) = 0;
		virtual void			iSetCurrentModelViewProjectionMatrix(const Matrix4d& m) = 0;
		virtual void			iReprojectOcclusionCull(const int& nWidth, const int& nHeight, INode* pCameraNode) = 0;
		virtual bool			iOcclusionCullBox(const BoundingBox& bBox, const Matrix4& mMVP) = 0;
		virtual bool			iOcclusionCullBox(const BoundingBox& bBox) = 0;
		virtual bool			iReadDepthValue(const int& x, const int& y, double& fOut, bool bImmediately = true) = 0;
		virtual bool			iReadDepthValue(const int& x, const int& y, const int& nHeight, IFrameBuffer* pFrameBuffer, double& fOut) = 0;
		virtual	bool			iProject(const Vector3d& vIn, const  ViewPort& vp, const Matrix4d& mPrj, 
			const Matrix4d& mViewModel, Vector3d& vOut) = 0;
		virtual bool			iUnProject(const int& x, const int& y, const ViewPort& vp, const Matrix4d& mPrj, 
			const Matrix4d& mViewModel, Vector3d& vOut, bool bImmediately = true) = 0;
		virtual bool			iUnProject(const int& x, const int& y, const double& z, const ViewPort& vp, 
			const Matrix4d& mPrj, const Matrix4d& mViewModel, Vector3d& vOut) = 0;

		virtual bool			iAddText(const Vector2& vPos, const Vector2& vSize,
			const Vector4& vColor, const char* str) = 0;
		// 创建一个矩形面
		// OpenGL要求y轴0.0坐标是在图片的底部的，但是图片的y轴0.0坐标通常在顶部
		// bUpsideDown用于上下颠倒
		virtual IMesh*			iCreateQuadMesh(const float& fResolutionScale, const CoordPlane_e& ePlane = COORD_PLANE_YOZ,
			const Vector2& vQuadCenter = Vector2(0, 0), const bool& bUpsideDown = true, const Matrix4& mMat = Matrix4::IDENTITY) = 0;
		// 创建一个立方体面
		virtual IMesh*			iCreateCubeMesh(const float& fResolutionScale = 1.0f, const Matrix4& mMat = Matrix4::IDENTITY) = 0;
		// 创建球面
		virtual IMesh*			iCreateSphereMesh(const float& fResolutionScale = 1.0f,
			const int& nSegmentX = 64, const int& nSegmentY = 64, const Matrix4& mMat = Matrix4::IDENTITY) = 0;
		virtual void			iTriangleBuildIndex(const std::vector<Triangle>& vecTri, std::vector<uint>& vecElem,
			std::vector<TriangleVertex>& vecVertices, PFNNMCOMPARED compare = nullptr) = 0;
		virtual IMesh*			iTriangleBuildMesh(const std::string& strName, const std::vector<Triangle>& vecTri,
			PFNNMCOMPARED compare = nullptr) = 0;
		virtual IMesh*			iTriangleIndexToMesh(const std::string& strName, std::vector<uint>& vecElem,
			std::vector<TriangleVertex>& vecVertices) = 0;
		virtual bool			iTriangleUpdateMesh(const std::vector<Triangle>& vecTri, IMesh* pMesh, 
			PFNNMCOMPARED compare = nullptr) = 0;
		virtual bool			iSplitTriangle(std::vector<Triangle>& vecTriangle, Plane& pln,
			std::vector<IMesh*>& vecMesh, const SplitDiscardType& eType = SPLIT_DISCARD_NONE) = 0;
		virtual bool			iVerticalSplitTriangle(std::vector<Triangle>& vecTriangle, Vector3& vStartPoint, Vector3& vCrossPoint,
			std::vector<IMesh*>& vecMesh) = 0;
		//外部加载script
		virtual void	iLoadScriptFile(const std::string& strScriptPath) = 0;
	};
}
