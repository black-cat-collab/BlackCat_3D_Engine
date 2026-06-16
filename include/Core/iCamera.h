#pragma once

#include <vector>

#define  CUBE_FACE_NUM  6

namespace bc
{
	enum CameraProjectionType_e						// 投影模式
	{
		PROJECTION_TYPE_PERSPECTIVE = 0,			// 透视投影
		PROJECTION_TYPE_ORTHO,						// 正射投影
	};

	enum CameraOperateType_e						// 操作方式
	{
		OPERATE_TYPE_SELECT = 0,					// 选择
		OPERATE_TYPE_CAMERA_TO_POINT,				// 相机跳转
		OPERATE_TYPE_ANIMATION_PLAY,				// 漫游播放
		OPERATE_TYPE_MEASURE,						// 测量 
		OPERATE_TYPE_TRIANGLE_SELECT,				// 框选模型
		OPERATE_TYPE_NO_OPERATE,					// 禁止操作相机
		OPERATE_TYPE_SPLIT_STOREY,					// 切割层
		OPERATE_TYPE_SPLIT_APARTMENT,					// 切割户室
		OPERATE_TYPE_END							// 结束
	};


	enum AnimationPathPlayType_e					// 路径播放类型
	{
		PATH_PLAY_LOOP_ALL = 0,						// 全部循环
		PATH_PLAY_LOOP_SINGLE,						// 单个循环
		PATH_PLAY_NOLOOP_ALL,						// 全部不循环
		PATH_PLAY_NOLOOP_SINGLE						// 单个不循环
	};

	// 定制相机移动到目标点的速度
	typedef void(*CameraMoveToPointSpeedFunc)(const float& fTotalDistance, float& fOutCurrentDistance, const float& fSpeed);

	class ViewPort
	{
	public:
		ViewPort() :
			m_nX(0), m_nY(0), m_nWidth(0), m_nHeight(0)
		{

		}
		ViewPort(const int& x, const int& y, const int& w, const int& h) :
			m_nX(x), m_nY(y), m_nWidth(w), m_nHeight(h)
		{

		}

		/*
		 *	设置视口
		 *	x		[in]	x值
		 *	y		[in]	y值
		 *	w		[in]	宽
		 *	h		[in]	高
		 */
		void Set(const int& x, const int& y, const int& w, const int& h)
		{
			m_nX = x;
			m_nY = y;
			m_nWidth = w;
			m_nHeight = h;
		}

		/*
		 *	获取x值
		 *	return	[out]	x值
		 */
		int GetX() const
		{
			return m_nX;
		}

		/*
		 *	获取y值
		 *	return	[out]	y值
		 */
		int GetY() const
		{
			return m_nY;
		}

		/*
		 *	设置x值
		 *	x		[in]	x值
		 */
		void SetX(const int& x)
		{
			m_nX = x;
		}

		/*
		 *	设置y值
		 *	y		[in]	y值
		 */
		void SetY(const int& y)
		{
			m_nY = y;
		}

		/*
		 *	获取宽
		 *	return	[out]	宽
		 */
		int GetWidth() const
		{
			return m_nWidth;
		}

		/*
		 *	获取高
		 *	return	[out]	高
		 */
		int GetHeight() const
		{
			return m_nHeight;
		}

		/*
		 *	设置宽
		 *	w		[in]	宽
		 */
		void SetWidth(const int& w)
		{
			m_nWidth = w;
		}

		/*
		 *	设置高
		 *	h		[in]	高
		 */
		void SetHeight(const int& h)
		{
			m_nHeight = h;
		}

		Matrix4 Matirx() const
		{
			Matrix4 m = Matrix4::IDENTITY;
			m.MakeTranslate(Vector3(1, 1, 1));
			Matrix4 t1 = Matrix4::IDENTITY;
			t1.SetScale(Vector3(0.5*m_nWidth, 0.5*m_nHeight, 0.5f));

			Matrix4 t2 = Matrix4::IDENTITY;
			t2.MakeTranslate(Vector3(m_nX, m_nY, 0.0f));
			return t2*t1*m;
		}
	private:
		int m_nX;
		int m_nY;
		int m_nWidth;
		int m_nHeight;
	};


	// 定制相机完成漫游回调函数
	class INode;
	typedef void(*CameraMoveCompleteFunc)(INode*, void*);

	class ICamera : public IComponent
	{
	public:
		/*
		 *	获取类型
		 *	return	[out]	类型
		 */
		virtual int							iGetComponentType() { return ComponentType_e::COMPONENT_TYPE_CAMERA; }




		/*
 *	获取前一个原点
 *	return	[out]	前一个原点
 */
		virtual  const Vector3d& iGetPreviousOrigin() = 0;
		/*
 *	获取渲染原点
 *	return	[out]	渲染原点
 */
		virtual  const Vector3d& iGetRenderOrigin() = 0;


		/*
		 *	获取操作方式
		 *	return	[out]	操作方式
		 */
		virtual const CameraOperateType_e&	iGetOperateType() const = 0;

		/*
		 *	设置操作方式
		 *	eType	[in]	操作方式
		 */
		virtual void 						iSetOperateType(CameraOperateType_e eType) = 0;

		/*
		 *	获取视口
		 *	return	[out]	视口
		 */
		virtual const ViewPort&				iGetViewPort() = 0;

		/*
		 *	设置视口
		 *	vVP		[in]	视口
		 */
		virtual void						iSetViewPort(const ViewPort& vVP) = 0;

		/*
		 *	获取视场角
		 *	return	[out]	视场角
		 */
		virtual const double&				iGetFOV() = 0;

		/*
		 *	设置视场角
		 *	fFov	[in]	视场角
		 */
		virtual void						iSetFOV(const double& fFov) = 0;

		/*
		 *	获取宽高比
		 *	return	[out]	宽高比
		 */
		virtual const double&				iGetAspect() = 0;

		/*
		 *	设置宽高比
		 *	fAspect	[in]	宽高比
		 */
		virtual void						iSetAspect(const double& fAspect) = 0;

		/*
		 *	获取近平面
		 *	return	[out]	近平面
		 */
		virtual const double&				iGetNearClip() = 0;

		/*
		 *	设置近平面
		 *	fNear	[in]	近平面
		 */
		virtual void						iSetNearClip(const double& fNear) = 0;

		/*
		 *	获取远平面
		 *	return	[out]	远平面
		 */
		virtual const double&				iGetFarClip() = 0;

		/*
		 *	设置远平面
		 *	fFar	[in]	远平面
		 */
		virtual void						iSetFarClip(const double& fFar) = 0;

		
		/*
		 *	获取视锥体
		 *	l		[in]	左平面
		 *	r		[in]	右平面
		 *	t		[in]	上平面
		 *	b		[in]	下平面
		 *	fN		[in]	近平面
		 *	fF		[in]	远平面
		 */
		virtual void						iGetFrustum(double& l, double& r, double& t, double& b, double& fN, double& fF) = 0;
		
		/*
		 *	设置视锥体
		 *	l		[in]	左平面
		 *	r		[in]	右平面
		 *	t		[in]	上平面
		 *	b		[in]	下平面
		 *	fN		[in]	近平面
		 *	fF		[in]	远平面
		 */
		virtual void						iSetFrustum(const double& l, const double& r, const double& t, const double& b, const double& fN, const double& fF) = 0;

		/*
		 *	设置视图矩阵
		 *	m		[in]	视图矩阵
		 */
		virtual void						iSetViewMatrix(const Matrix4d& m) = 0;

		/*
		 *	设置投影矩阵
		 *	m		[in]	投影矩阵
		 */
		virtual void						iSetProjectionMatrix(const Matrix4d& m) = 0;

		/*
		 *	设置模型视图投影矩阵
		 *	m		[in]	模型视图投影矩阵
		 */
		virtual void						iSetModelViewProjectionMatrix(const Matrix4d& m) = 0;

		/*
		 *	获取视图矩阵
		 *	return	[out]	视图矩阵
		 */
		virtual  const Matrix4d& iGetViewMatrix()  = 0;

		/*
		 *	获取投影矩阵
		 *	return	[out]	投影矩阵
		 */
		virtual  const Matrix4d& iGetProjectionMatrix()  = 0;

		/*
		 *	获取模型视图投影矩阵
		 *	return	[out]	模型视图投影矩阵
		 */
		virtual  const Matrix4d& iGetModelViewProjectionMatrix()  = 0;

		/*
		 *	获取视图空间转换矩阵
		 *	return	[out]	视图空间转换矩阵
		 */
		virtual  const Matrix4d& iGetViewSpaceTransformMatrix()  = 0;

		/*
		 *	获取前一个视图矩阵
		 *	return	[out]	前一个视图矩阵
		 */
		virtual  const Matrix4d&				iGetPreviousViewMatrix()  = 0;

		/*
		 *	获取前一个投影矩阵
		 *	return	[out]	前一个投影矩阵
		 */
		virtual  const Matrix4d& iGetPreviousProjectionMatrix()  = 0;

		/*
		 *	获取前一个模型视图投影矩阵
		 *	return	[out]	前一个模型视图投影矩阵
		 */
		virtual  const Matrix4d& iGetPreviousModelViewProjectionMatrix()  = 0;

		/*
		*	记录当前位置
		*/
		virtual void						iRecordOrigin() = 0;

		/*
		*	获取上次记录的位置
		*	return	[out]	上次记录的位置
		*/
		virtual Vector3d						iGetRecordOrigin() = 0;

		/*
		*	记录当前角度
		*/
		virtual void						iRecordAngles() = 0;

		/*
		*	获取上次记录的角度
		*	return	[out]	上次记录的角度
		*/
		virtual Vector3d						iGetRecordAngles() = 0;

		/*
		*	添加视点
		*	strName  [in]  视点名称
		*/
		virtual void						iAddViewPoint(const std::string& strName) = 0;

		/*
		*	重新保存视点
		*	strName  [in]  被重新保存的视点名称
		*/
		virtual void						iSaveViewPoint(const std::string& strName) = 0;

		/*
		*	删除视点
		*	strName  [in]  被删除的视点名称
		*/
		virtual void						iDeleteViewPoint(const std::string& strName) = 0;

		/*
		*	获取全部视点名字
		*	return  [int]  视点名字存放容器
		*/
		virtual void						iGetViewPointName(std::vector<std::string>& vecName) = 0;

		/*
		 *	观察点位置转换
		 *	strName	[in]	名字
		 */
		virtual void						iSetViewPoint(const std::string& strName) = 0;

		/*
		 *	获取观察点位置
		 *	strName	[in]	名字
		 *  vOrigin	[out]	原点坐标
		 *	vAngle	[out]	角度
		 */
		virtual void						iGetViewPoint(const std::string& strName, Vector3d& vOrigin, Vector3d& vAngle) = 0;



		/*
		 *	获取观察点位置
		 *	strName	[in]	名字
		 *  vOrigin	[out]	原点坐标 双精度
		 *	vAngle	[out]	角度
		 */
		//virtual void						iGetViewPointd(const std::string& strName, Vector3d& vOrigin, Vector3d& vAngle) = 0;
		/*
		 *	观察点位置转换
		 *	vOrigin	[in]	原点坐标
		 *	vAngle	[in]	角度
		 *	bSky	[in]	是否转换
		 */
		virtual void						iGoViewPoint(const Vector3d& vOrigin, const Vector3d& vAngle, const bool& bSky = true) = 0;



		/*
		 *	观察点位置转换
		 *	strName	[in]	名字
		 *	bSky	[in]	是否转换
		 */
		virtual void						iGoViewPoint(const std::string& strName, const bool& bSky = true) = 0;

		/*
		 *	设置跳转速度
		 *	fSpeed	[in]	速度
		 */
		virtual void						iSetGoViewPointSpeed(const float& fSpeed) = 0;

		/*
		 *	获取跳转速度
		 *	return	[out]	速度
		 */
		virtual float						iGetGoViewPointSpeed() = 0;

		/*
		 *	设置空中跳转高度
		 *	fHeight [in]	高度
		 */
		virtual void						iSetGoViewPointSkyPositionZ(const float& fHeight) = 0;

		/*
		 *	设置跳转完成回调
		 *	pFunc	[in]	回调函数
		 */
		virtual void						iSetGoViewPointCallback(CameraMoveCompleteFunc pFunc, void* pUser) = 0;

		/*
		 *	观察指定节点
		 *	INode			[in]	节点指针
		 *	fAddDistance	[in]	增加距离
		 *	bGoViewPoint	[in]	是否跳转
		 */
		virtual void						iLookAtNode(INode* pNode, const bool& bGoViewPoint = false, 
														const double& fAddDistance = 0.0) = 0;

		/*
		 *	将屏幕坐标(x,y)转换成世界坐标的一条射线并分别记录与近远平面的两个交点
		 *	x		[in]	屏幕坐标x的值
		 *	y		[in]	屏幕坐标y的值
		 *	tObj	[in]	存储要取的相交对象的属性
		 *	return	[out]	是否成功
		 */
		virtual bool						iIntersectObject(const int& x, const int& y, IntersectSet& tObj) = 0;

		/*
		 *	得出相交对象并记录其属性
		 *	vStart	[in]	起点的世界坐标
		 *	vEnd	[in]	终点的世界坐标
		 *	tObj	[in]	存储要取的相交对象的属性
		 *	return	[out]	是否成功
		 */
		virtual bool						iIntersectObject(const Vector3d& vStart, const Vector3d& vEnd, IntersectSet& tObj) = 0;

		/*
		 *	将世界坐标转换成屏幕坐标
		 *	v		[in]	原坐标
		 *	vOut	[in]	存储转换后的坐标
		 *	return	[out]	是否成功
		 */
		virtual bool						iProject(const Vector3d& v, Vector3d& vOut) = 0;
		//virtual bool						iProjectd(const Vector3d& v, Vector3d& vOut) = 0;
		/*
		 *	将屏幕坐标(x,y)转换成世界坐标
		 *	x		[in]	屏幕坐标x的值
		 *	y		[in]	屏幕坐标y的值
		 *	z		[in]	深度
		 *	vOut	[in]	存储转换后的世界坐标
		 *	return	[out]	是否成功
		 */
		virtual bool						iUnProject(const int& x, const int& y, const double &z, Vector3d& vOut) = 0;
		//virtual bool						iUnProjectd(const int& x, const int& y, const double& z, Vector3d& vOut) = 0;
		/*
		 *	将深度为0的屏幕坐标(x,y)转换成世界坐标
		 *	x				[in]	屏幕坐标x的值
		 *	y				[in]	屏幕坐标y的值
		 *	vOut			[in]	存储转换后的世界坐标
		 *	bImmediately	[in]	立即
		 *	return			[out]	是否成功
		 */
		virtual bool						iUnProject(const int& x, const int& y, Vector3d& vOut, const bool& bImmediately = true) = 0;
		//virtual bool						iUnProjectd(const int& x, const int& y, Vector3d& vOut, const bool& bImmediately = true) = 0;
		/*
		 *	获取射线起点终点 长度为FarClip
		 *	x				[in]	屏幕坐标x的值
		 *	y				[in]	屏幕坐标y的值
		 *	vStart			[in]	射线起点
		 *	vEnd			[in]	射线终点
		 */
		virtual void						iGetRay(const int& x, const int& y, Vector3d& vStart, Vector3d& vEnd) = 0;

		/* 
		 *	计算包围球直径像素
		 *	M		[in]	模型矩阵
		 *	return	[out]	像素大小
		 */
		virtual Vector4d						iComputePixelSizeVector(const Matrix4d& M) = 0;

		/*
		 *	计算包围球直径像素
		 *	vPixelSizeVector	[in]	像素大小
		 *	tSphere				[in]	包围球
		 *	fLODScale			[in]	多细节层次规模
		 *	return				[out]	直径像素
		 */
		virtual double						iClampedPixelSize(const Vector4d& vPixelSizeVector, const BoundingSphered& tSphere, 
			const double& fLODScale = 1.0) = 0;

		virtual void						iPlayAnimationPath(const std::string& strName, const AnimationPathPlayType_e& eType, 
			const bool& bFromBeginning = true) = 0;

		virtual void						iPlayAnimationPath(const int& nIndex, const AnimationPathPlayType_e& eType, 
			const bool& bFromBeginning = true) = 0;

		virtual void						iStopAnimationPath() = 0;
		virtual void						iDeleteAnimationPath(const std::string& strName) = 0;
		virtual void						iAddAnimationPath(const char* szName, const std::vector<Vector3>& vecPos, 
			const std::vector<Quaternion>& vecRot) = 0;

		virtual void						iGetAnimationPath(std::vector<std::string>& vecPath) = 0;
		virtual bool						iAddAnimationPath(const std::string& strName) = 0;
		virtual void						iGetAnimationPathPoint(const std::string& strName, std::vector<Vector3>& vecPos,
			std::vector<Quaternion>& vecRot) = 0;
		virtual void						iAddAnimationKeyPoint(const std::string& strName, 
			const Vector3& vPos, const Quaternion& vRot) = 0;
		virtual void						iInsertAnimationKeyPoint(const std::string& strName, const int& nIndex,
			const Vector3& vPos, const Quaternion& vRot) = 0;
		
		virtual void						iDeleteAnimationKeyPoint(const std::string& strName, const int& nIndex) = 0;
		virtual void						iSetAnimationKeyPoint(const std::string& strName, const int& nIndex,
			const Vector3& vPos, const Quaternion& vRot) = 0;
		virtual void						iGetAnimationKeyPoint(const std::string& strName, std::vector<Vector3>& vecPos,
			std::vector<Quaternion>& vecRot) = 0;
		virtual void						iGetAnimationKeyPoint(const std::string& strName, const int& nIndex,
			Vector3& vPos, Quaternion& vRot) = 0;
		virtual void						iSetAnimationKeyPointSpeed(const std::string& strName, const float& fSpeed) = 0;
		virtual float						iGetAnimationKeyPointSpeed(const std::string& strName) = 0;
		virtual void						iSetAnimationCurIndex(const std::string& strName, const int& nIndex) = 0;
		virtual bool						iSaveAnimationPath(const std::string& strFilePath) = 0;
		virtual CameraProjectionType_e		iGetProjectType() = 0;

		virtual void						iBoxModel(Vector2& vPoint1, Vector2& vPoint2) = 0;
		virtual void                        iSetViewingAxis(const Vector3d& axis, bool bGoViewPoint = false) = 0;
		virtual bool						iCullBox(const BoundingBoxd& tBBox, const int& nFace) = 0;
		virtual bool						iCullPoint(const Vector3d& vPoint) = 0;
		virtual bool						iCullSphere(const BoundingSphered& tSphere) = 0;



	};

	
}

