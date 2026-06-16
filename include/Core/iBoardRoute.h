#pragma once

namespace bc
{
	class IBoardRoute;
	typedef void(*BoardRouteMoveFunc)(IBoardRoute* pRoute, void* pUser);
	typedef void(*BoardRouteCompleteFunc)(IBoardRoute* pRoute, void* pUser, const int& nCurrentLoop, const int& nTotalLoop);

	class IBoardRoute : public IComponent
	{
	public:
		virtual int		iGetComponentType() { return COMPONENT_TYPE_BOARD_ROUTE; }

		/*
		*	添加播放点
		*	vPos     [in]  点位置
		*	vAngles  [in]  角度
		*	pParam   [in]  面板参数
		*/
		virtual INode*	iAddPoint(const Vector3d& vPos, const Vector3d& vAngles, 
			BoardParam* pParam = nullptr, const double& fBoardDistance = 3) = 0;

		virtual bool	iGetPoint(const int& nIndex, Vector3& vPos, Vector3& vAngles, INode*& pBoard) = 0;

		/*
		*	删除播放点
		*	nIndex  [in]  播放点索引
		*/
		virtual void	iRemovePoint(const int& nIndex) = 0;

		/*
		 *	清空播放点
		 */
		//virtual void	iClearPoint() = 0;
		
		/*
		*	开始播放
		*	nIndex  [in]  播放点索引
		*/
		virtual void	iBeginPlay(const int& nIndex = 0) = 0;
		
		/*
		*	停止播放
		*/
		virtual void	iStopPlay() = 0;

		/*
		*	是否正在播放视频
		*	return  [out]  
		*/
		virtual bool	iIsPlaying() = 0;

		/*
		*	暂停
		*	bPause  [in]  
		*/
		virtual void	iPause(const bool& bPause = true) = 0;

		/*
		*	是否暂停
		*	return  [out]  
		*/
		virtual bool	iIsPause() = 0;

		/*
		*	获取当前Index
		*	return  [out]
		*/
		virtual int		iGetCurrentIndex() = 0;

		/*
		*	获取总数
		*	return  [out]
		*/
		virtual int		iGetSize() = 0;

		/*
		*	设置循环
		*	bLoop  [in]  是否循环
		*	nTotalLoopCount  [in]  循环次数
		*/
		virtual void	iSetLoop(const bool& bLoop, const int& nTotalLoopCount) = 0;

		/*
		*	设置相机移动速度
		*	fSpeed  [in]  速度
		*/
		virtual void	iSetMoveSpeed(const float& fSpeed) = 0;

		/*
		*	设置等待时间
		*	fWaitTime  [in]  等待时间
		*/
		virtual void	iSetWaitTime(const float& fWaitTime) = 0;

		/*
		*	设置在没有视频点时等待
		*	bWait  [in]  是否等待
		*/
		virtual void	iSetWaitOnNoVideoPoint(const bool& bWait) = 0;

		/*
		*	设置恒定速度
		*	bConstantSpeed  [int]  是否使用恒定速度
		*/
		virtual void	iSetConstantSpeed(const bool& bConstantSpeed) = 0;

		/*
		*	设置路径完成函数
		*	pBoardRouteCompleteFunc  [in]  视频路径完成函数
		*	pUser  [in]  回调对象
		*/
		virtual void	iSetRouteCompleteFunc(BoardRouteCompleteFunc pBoardRouteCompleteFunc, void* pUser) = 0;

		/*
		*	设置开始等待回调函数
		*	pBoardRouteWaitFunc  [in]  移动到每一个点等待函数
		* 	pUser  [in]  回调对象
		*/
		virtual void	iSetWaitBeginFunc(BoardRouteMoveFunc pBoardRouteWaitFunc, void* pUser) = 0;

		/*
		*	设置开始移动回调函数
		*	pBoardRouteMoveFunc  [in]  开始移动函数
		* 	pUser  [in]  回调对象
		*/
		virtual void	iSetMoveBeginFunc(BoardRouteMoveFunc pBoardRouteMoveFunc, void* pUser) = 0;
	};
}

