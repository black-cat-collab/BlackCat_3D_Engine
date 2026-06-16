#pragma once

namespace bc
{
	class IVideoRoute;
	typedef void(*VideoRouteMoveFunc)(IVideoRoute* pRoute, void* pUser);
	typedef void(*VideoRouteCompleteFunc)(IVideoRoute* pRoute, void* pUser, const int& nCurrentLoop, const int& nTotalLoop);
	//返回true 继续执行下面默认操作，false则直接全部接管
	typedef bool(*VideoRouteChangeVideoPointFunc)(INode* pVideoBoard, void* pUser, bool bOpen);

	class IVideoRoute : public IComponent
	{
	public:
		virtual int		iGetComponentType() { return COMPONENT_TYPE_VIDEO_ROUTE; }

		/*
		*	添加播放点
		*	vPos     [in]  点位置
		*	vAngles  [in]  角度
		*	pParam   [in]  视频面板参数
		*	return  [out]  创建的面板
		*/
		virtual INode*	iAddPoint(const Vector3d& vPos, const Vector3d& vAngles, 
			VideoBoardWithButtonParam* pParam = nullptr, const bool& bResetBoardPosition = true) = 0;
		
		virtual bool	iGetPoint(const int& nIndex, Vector3d& vPos, Vector3d& vAngles, INode*& pBoard) = 0;

		/*
		*	删除播放点
		*	nIndex  [in]  播放点索引
		*/
		virtual void	iRemovePoint(const int& nIndex) = 0;
		
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
		*	设置全屏偏移
		*	fOffset  [in]  偏移量
		*/
		virtual void	iSetFullScreenOffset(const float& fOffset) = 0;

		/*
		*	设置恒定速度
		*	bConstantSpeed  [int]  是否使用恒定速度
		*/
		virtual void	iSetConstantSpeed(const bool& bConstantSpeed) = 0;
		
		/*
		*	设置路径完成函数
		*	pVideoRouteCompleteFunc  [in]  视频路径完成函数
		*/
		virtual void	iSetRouteCompleteFunc(VideoRouteCompleteFunc pVideoRouteCompleteFunc, void* pUser) = 0;

		/*
		*	设置开始等待回调函数
		*	pBoardRouteWaitFunc  [in]  移动到每一个点等待函数
		* 	pUser  [in]  回调对象
		*/
		virtual void	iSetWaitBeginFunc(VideoRouteMoveFunc pVideoRouteWaitFunc, void* pUser) = 0;

		/*
		*	设置开始移动回调函数
		*	pBoardRouteMoveFunc  [in]  开始移动函数
		* 	pUser  [in]  回调对象
		*/
		virtual void	iSetMoveBeginFunc(VideoRouteMoveFunc pVideoRouteMoveFunc, void* pUser) = 0;

		/*
		*	设置切换视频回调函数
		*	pBoardRouteMoveFunc  [in]  切换视频函数
		* 	pUser  [in]  回调对象
		*/
		virtual void		iSetVideoChangeFunc(VideoRouteChangeVideoPointFunc pVideoChangeFunc, void* pUser) = 0;
	};
}

