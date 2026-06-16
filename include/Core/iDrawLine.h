#pragma once

namespace bc
{
	class IShell;

	typedef void (*ShellCreatedCallback)(std::vector<IShell*> shells);

	class DrawLineParam : public NodeParamBase
	{
	public:
		enum DrawLineType
		{
			OPEN_TYPE = 0,	// 打开
			CLOSE_TYPE		// 闭合
		};

		Vector4					vLineColor;			// 颜色
		float					fLineWidth;			// 线宽
		float					fPointSize;			// 点大小
		float					fEpsilonDistance;	// 判断重合距离
		int						nPointMaxLimit;		// 最多能绘制几个点
		bool					bDepthTest;			// 是否开启深度测试
		DrawLineType			eDrawType;
		DrawLineParam() : NodeParamBase()
		{
			nNodeType = NODE_DRAW_LINE;
			vLineColor = Vector4(1.0);
			fLineWidth = 1.0;
			fPointSize = 10.0;
			fEpsilonDistance = 1.0;
			nPointMaxLimit = -1;
			bDepthTest = false;
			eDrawType = OPEN_TYPE;
		}

		DrawLineParam& operator=(const NodeParamBase& rhs)
		{
			NodeParamBase::operator=(rhs);

			return *this;
		}

		DrawLineParam& operator=(const DrawLineParam& rhs)
		{
			NodeParamBase::operator=(rhs);

			vLineColor = rhs.vLineColor;
			eDrawType = rhs.eDrawType;
			fLineWidth = rhs.fLineWidth;
			fPointSize = rhs.fPointSize;
			fEpsilonDistance = rhs.fEpsilonDistance;
			nPointMaxLimit = rhs.nPointMaxLimit;
			bDepthTest = rhs.bDepthTest;
			return *this;
		}
	};

	class IDrawLine : public IComponent
	{
	public:
		virtual int			iGetComponentType() { return COMPONENT_TYPE_DRAW_LINE; }
		virtual void		iGetPoint(std::vector<Vector3>& vecPoint) = 0;
		virtual void		iGetIntersect(std::vector<IntersectSet>& vecIntersect) = 0;
		virtual void		iStartDraw() = 0;
		virtual void		iStopDraw() = 0;
		virtual void		iClearPoint() = 0;
		virtual void		iRemoveLastPoint() = 0;
		virtual void		iGetDrawParam(DrawLineParam& tParam) = 0;
		virtual void		iSetDrawParam(const DrawLineParam& tParam) = 0;
		virtual void        iSetDrawMode(IShell* pNode, MouseToolType_e type) = 0;
		virtual void        iUndo() = 0;
		virtual void        iRedo() = 0;
		virtual void        iSetAutoGenerate(bool b) = 0;
		virtual void        iSetClosePointsCount(int32_t count) = 0;
		virtual void        iSaveSplit() = 0;
		virtual void        iCancelSplit() = 0;
		virtual void        iSetTopGroupNode(INode* pTopGroupNode) = 0;
		virtual INode*      iGetTopGroupNode() = 0;
		virtual void        iSetCreatedCallback(ShellCreatedCallback callback) = 0;
		virtual void		iSetTextureSaveDir(const std::string& strPath) = 0;
	};
} // namespace bc


