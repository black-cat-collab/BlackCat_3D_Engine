#pragma once

namespace bc
{
	class IManipulator;
	// 定制键盘事件
	typedef void(*CustomKeyActionFunc)(INode* pCamera, const bool& bDown, void* pUser);
	//Node节点点击立即回调
	typedef void(*NodeClickImmeFunc)(INode* pCamera, INode* pClickNode, void* pUser);

	class IManipulator : public IComponent
	{
	public:
		/*
		 *	获取类型
		 *	return	[out]	类型
		 */
		virtual int			iGetComponentType() { return ComponentType_e::COMPONENT_TYPE_MANIPULATOR; }

		virtual Vector3d		iGetMousePoint() = 0;

		virtual Vector2		iGetMouseDownDCPoint(const MouseButton_e& eButton) = 0;

		virtual Vector2		iGetMouseMoveDCPoint() = 0;

		/*
		 *	传入屏幕坐标获取三维世界坐标
		 *	
		 */
		virtual bool iPickedPoint(const int& x, const int& y, Vector3d& vOut) = 0;

		virtual void		iSetMouseButtonEnable(const MouseButton_e& eButton, const bool& bEnable) = 0;

		virtual bool		iGetMouseButtonEnable(const MouseButton_e& eButton) = 0;

		virtual void		iSetMouseClickEnable(const MouseButton_e& eButton, const bool& bEnable) = 0;

		virtual bool		iGetMouseClickEnable(const MouseButton_e& eButton) = 0;

		virtual void		iSetKeyActionEnable(const int& nKeyAction, const bool& bEnable) = 0;

		virtual bool		iGetKeyActionEnable(const int& nKeyAction) = 0;

		virtual bool		iBindCustomKeyAction(const uint& nKey, const int& nKeyAction, CustomKeyActionFunc pFunc, void* pUser) = 0;

		/*
		 *	获取视角限制球体
		 *	return				[out]	BoundingSphere
		 */
		virtual const BoundingSphered& iGetViewSphere() const = 0;

		/*
		 *	设置视角限制球体
		 *	return				[out]	void
		 */
		virtual void		iSetViewSphere(const BoundingSphered& tSphere) = 0;

		virtual void		iSetSceneLimitHeight(const float& fHeight) = 0;

		virtual float		iGetSceneLimitHeight() = 0;

		virtual void		iSetSceneBaseHeight(const float& fHeight) = 0;

		virtual float		iGetSceneBaseHeight() = 0;

		virtual float*		iGetSceneBaseHeightPtr() = 0;

		virtual void		iSetMultipleSelectEnable(bool bEnable) = 0;

		virtual Vector3d		iGetViewCenterPoint() = 0;

		/*
		 *	设置相机是否惯性
		 *	bEnable		是否惯性相机
		 */
		virtual void		iSetInertialEnable(bool bEnable) = 0;
		/*
		 *	设置相机惯性速度比例
		 *	fScale		速度的比例
		 */
		virtual void		iSetInertialSpeedScale(const float& fScale) = 0;
		/*
		 *	是否切换右键和中间的操作
		 *	bSwitch		是否切换
		 */
		virtual void		iSwitchRBMB(bool bSwitch) = 0;
		virtual bool		iIsSwitchRBMB() = 0;
		// 滚轮缩放因子 (越大则幅度越大)
		virtual void		iSetWheelFactor(const float& fFactor) = 0;
		// 添加Node节点点击立即回调
		virtual void      iAddNodeClickImmeFunc(NodeClickImmeFunc pFunc, void* pUser) = 0;
		virtual void      iRemoveNodeClickImmeFunc(void* pUser) = 0;
		virtual void      iRemoveAllNodeClickImmeFunc() = 0;
	};
}

