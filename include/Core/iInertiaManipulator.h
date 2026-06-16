#pragma once

namespace bc
{
	class IInertiaManipulator;
	typedef float(*InertiaSpeedFunc)(IInertiaManipulator* pCamera);

	class IInertiaManipulator : public IComponent
	{
	public:
		/*
		 *	获取类型
		 *	return	[out]	类型
		 */
		virtual int		iGetComponentType() { return ComponentType_e::COMPONENT_TYPE_INERTIA_MANIPULATOR; }

		virtual void	iSetRotateSpeedFunc(InertiaSpeedFunc pFun) = 0;

		virtual void	iSetLookAtPlane(const Planed& tPlane) = 0;

		virtual const Planed& iGetLookAtPlane() const = 0;

		virtual void	iSetLookAtPoint(const Vector3d& vPoint, const bool& bNormal = true, const Vector3d& vNormal = Vector3d(0, 0, 1)) = 0;

		virtual const Vector3d& iGetLookAtPoint() const = 0;

		virtual void	iSetLimitAngle(const float& fMax, const float& fMin) = 0;

		virtual float	iGetMaxLimitAngle() = 0;

		virtual float	iGetMinLimitAngle() = 0;

		virtual void	iStopInertiaMove() = 0;
	};
}

