#pragma once

namespace bc
{
	// 聚合等级参数
	class ClustererLevelParam
	{
	public:
		typedef void(*ClustererUpdateFunc)(ClustererLevelParam* pParam, INode *pNode, const int& nCount);
		typedef INode*(*ClustererCreateNodeFunc)(ClustererLevelParam* pParam, ISystemAPI *pSystemAPI, Vector3 vOrigin);
		typedef void(*ClustererBindVisibleChangedFunc)(ClustererLevelParam* pParam, std::vector<INode*> vecNode, const bool& bVisible);

		ClustererLevelParam() : vTopLeft(Vector3(0, 0, 0)), vBottomRight(Vector3(0, 0, 0)), nRows(0), nColumns(0), bCentroid(false),
			fMinRange(0.0f), fMaxRange(0.0f), bDrawLine(true), fLineWidth(1.0f), vLineColor(Vector4(1.0f, 1.0f, 1.0f, 0.8f)),
			pCreateNodeFunc(nullptr), pUpdateFunc(nullptr) , pBindVisibleChangedFunc(nullptr)
		{
			
		}

		~ClustererLevelParam() {}

	public:
		// 左上顶点 坐标轴为：x朝右，y朝上
		Vector3 vTopLeft;
		// 右下顶点
		Vector3 vBottomRight; 
		// 需要划分的行数
		int	nRows;
		// 需要划分列数
		int nColumns;
		// 是否更新位置到质心
		bool bCentroid;
		// 最小值
		float fMinRange;
		// 最大值
		float fMaxRange;
		// 是否绘制线框
		bool bDrawLine;
		// 线宽
		float fLineWidth;
		// 线颜色
		Vector4 vLineColor;
		// 创建聚合后的节点
		ClustererCreateNodeFunc pCreateNodeFunc;
		// 更新区域的聚合数量
		ClustererUpdateFunc pUpdateFunc;
		//变量暂存
		std::map<std::string, INT_PTR> mapINTPTR;
		//显示变化回调
		ClustererBindVisibleChangedFunc pBindVisibleChangedFunc;
	};

	class INodeClusterer
	{
	public:
		// 增加等级
		virtual void iAddLevel(const ClustererLevelParam& tParam) = 0;

		// 绑定对象
		virtual void iBindNode(INode* pNode) = 0;

		// 解绑所有对象
		virtual void iUnbindAllNode() = 0;
	};
}

