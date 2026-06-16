#pragma once

#include <unordered_map>

/***************/
//视频融合工具
/***************/

namespace bc
{
	class PROJECT_BASED_API VideoFusionTool
	{
	public:
		VideoFusionTool(INode *pParentGroup);
		virtual ~VideoFusionTool();

		INode* GetParentGroup() { return m_pParentGroup; }
		INode* GetFusionCameraNode() { return m_pFusionCameraNode; }

		void SetPosition(const Vector3d& vOrg);
		void SetAngle(const Vector3d& vAngle);
		void SetFOV(float fFOV);
		void SetAspect(float fRatio);
		void SetDistance(float fDistane);
		void SetFuseImage(const std::string& strImage);
		void SetFuseParam(const FusionParam& tFusionParam);

		//开始渲染，方法前需要设置好参数
		void Render();
		void Stop();
		void SetCutEnable(bool bEnable);
		void MoveLeft(float fDis);
		void MoveForward(float fDis);
		void MoveZ(float fDis);
		void RotateX(float fAngle);
		void RotateY(float fAngle);
		void Save();
		void Reset();		//重置当前

		void SetModelSavePath(std::string strSavePath);

	private:
		static void static_fusion_create_callback(IFusion* pFusionNode);
		void createFusionCameraNode();

	protected:
		INode* m_pParentGroup;
		INode* m_pFusionCameraNode;
		std::string m_strModelSavePath;

	private:
		static VideoFusionTool* s_pThis;
		INode* m_pCurFusionNode;		//当前操作的FusionNode
		bool m_bStartRender;

	};
}

