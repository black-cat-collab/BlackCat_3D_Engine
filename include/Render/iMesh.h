#pragma once

#include <string>
#include <vector>

#ifdef __GNUC__

typedef unsigned long long LARGE_INTEGER;

#endif

namespace bc
{
	typedef struct MeshIndexData_s
	{	
		uint	nFirstVertex;
		uint	nVertexCount;
		uint	nFirstIndex;
		uint	nIndexCount;

		MeshIndexData_s() :
			nFirstVertex(0),
			nVertexCount(0),
			nFirstIndex(0),
			nIndexCount(0)
		{

		}
	}MeshIndexData;

	class INode;
	class IMaterial;
	class IVertexBuffer;
	class IParticleObject;
	class BoundingBox;
	class IntersectSet;
	class WatchTimer;

	struct SegmentInstrect;
	struct DrawElementsIndirectCmd_s;
	class IMesh
	{
	public:
		virtual std::string			iGetName() = 0;
		virtual void				iSetName(std::string strName) = 0;
		virtual IVertexBuffer*		iGetVertexBuffer(const int& nLOD = 0) = 0;
		virtual IParticleObject*	iGetParticleObject(const int& nIndex) = 0;
		virtual IMaterial*			iGetMaterial() = 0;
		virtual bool				iCompile() = 0;
		virtual bool				iCompile(WatchTimer& watchTimer, uint& nRetState) = 0;
		virtual bool				iIsCompiled() = 0;
		virtual int					iGetTriangleCount(const int& nLOD = 0) = 0;
		virtual void				iUpdateBBox() = 0;
		virtual const BoundingBox&	iGetBBox() = 0;
		virtual	bool				iGetPlane(Plane& tPlane) = 0;
		virtual bool				iGetNormal(Vector3& vNormal, float& fDistance) = 0;
		virtual void				iAttachMaterial(IMaterial* pMaterial) = 0;
		virtual bool				iIntersect(SegmentInstrect& si, std::vector<IntersectSet>& vecRadio, INode* pNode = nullptr, void* pObject = nullptr) = 0;
		virtual bool				iIntersect(SegmentInstrect& si, std::vector<IntersectSet>& vecRadio, int32_t offset, int32_t size, INode* pNode = nullptr, void* pObject = nullptr) = 0;
		virtual bool				iGetHighestPoint(SegmentInstrect& si, std::vector<IntersectSet>& vecRadio, INode* pNode = nullptr) = 0;
		virtual bool				iCreateLOD(const std::vector<float>& vecScale) = 0;
		virtual void				iCreateIndirectDrawObject() = 0;
		virtual int					iCreateParticleObject(const int& nTotalSize, ParticleInitialize pFunc, void* pInitializeUser) = 0;
		virtual void				iUpdateParticleObject(const int& nIndex, const int& nSize) = 0;
		virtual void				iGetTriangle(std::vector<Triangle>& vecTriangle, bool hasUV1 = false) = 0;
		// UV缩放 旋转 镜像
		virtual void				iGetUV(std::vector<Vector2>& vUV, const int& nChannel) = 0;
		virtual void				iSetUVScale(const Vector2& vScale, const int& index = 0) = 0;
		virtual Vector2				iGetUVScale(const int& index = 0) = 0;
		virtual void				iSetUVRotation(const float& fAngle, const Vector2& vCenter = Vector2(0.5f, 0.5f), const int& index = 0) = 0;
		virtual float				iGetUVRotation(const int& index = 0) = 0;
		virtual void				iSetUVMirror(const int& nDirection, const bool& bMirror, const int& index = 0) = 0; // nDirection 0 ：X; 1 ：Y; 2 ：XY; other : X;
		virtual bool				iGetUVMirror(const int& nDirection, const int& index = 0) = 0; // nDirection 0 ：X; 1 ：Y; other : false;
		virtual void				iAddMergeIndex(IMesh* pMergeMesh, const MeshIndexData& tData) = 0;
		virtual void				iDeleteMergeIndex(IMesh* pMergeMesh) = 0;
		virtual bool				iGetMergeIndex(IMesh* pMergeMesh, MeshIndexData& tData) = 0;
		virtual const std::vector< MeshIndexData>&iGetMergeIndex() = 0;
		virtual void				iUpdateIndirectDrawCommand(const std::vector<DrawElementsIndirectCmd_s>& tCommand) = 0;
	};
}

