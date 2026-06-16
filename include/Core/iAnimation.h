#pragma once

namespace bc
{

	class IAnimation;
	enum class AnimationTweenType
	{
		None = 0,
		Line = 1,
		Curve = 2,
		QuadIn = 3,
		QuadOut = 4,
		QuadInOut = 5
	};

	enum class AnimationType
	{
		Animation_None,
		Animation_Transform,
		Animation_Path, // 路径动画
		Animation_Flashing, //闪烁动画
		Animation_Attribute, //属性动画
		Animation_Visiable, //显隐动画
		Animation_Growth //生长动画
	};

	enum class AnimationState
	{
		Play, Stop
	};

	class AnimationConfig
	{
	public:

		AnimationConfig()
		{
			m_bReset = true;
			m_iPlayTimes = 1.0f;
			m_fPosition = 0.0f;
			m_fDuration = 1.0f;
			m_fTimeScale = 1.0f;
		}


		AnimationTweenType m_eTweenType = AnimationTweenType::None;

		bool m_bReset; // 动画播放完了之后，重设到初始状态。
		int32_t m_iPlayTimes; // 播放次数 [0: 无限循环播放, [1~N]: 循环播放 N 次]

		float m_fPosition; // 播放开始时间; 默认是0.0,  最大值是 duration;
		float m_fDuration; // 播放持续时间;
		float m_fTimeScale; // 播放速度

		std::string m_sName;
	};

	typedef void (*AnimationCallback)(INode* pNode, IAnimation* pAnimation, float progress);

	class AnimationParam
	{
	public:
		AnimationType eType = AnimationType::Animation_None;
		AnimationCallback callback = nullptr;

		AnimationParam& operator=(const AnimationParam& rhs)
		{
			eType = rhs.eType;
			callback = rhs.callback;
			return *this;
		}
	};

	class TransformAnimationParam : public AnimationParam
	{
	public:
		TransformAnimationParam()
		{
			eType = AnimationType::Animation_Transform;
			vPosition = Vector3(0.0f);
			vScale = Vector3(1.0f);
			vAxis = Vector3(0.0f);
		}

		TransformAnimationParam& operator=(const AnimationParam& rhs)
		{
			AnimationParam::operator=(rhs);
			return *this;
		}

		TransformAnimationParam& operator=(const TransformAnimationParam& rhs)
		{
			AnimationParam::operator=(rhs);
			vPosition = rhs.vPosition;
			vScale = rhs.vScale;
			vAxis = rhs.vAxis;
			fAngle = rhs.fAngle;
			return *this;
		}

		Vector3 vPosition;
		Vector3 vScale;
		Vector3 vAxis;
		float      fAngle;
	};

	class PathAnimationParam : public AnimationParam
	{
	public:
		PathAnimationParam()
		{
			eType = AnimationType::Animation_Path;
		}

		std::vector<Vector3> vecControlPoints;
	};

	class FlashingAnimationParam : public AnimationParam
	{
	public:
		FlashingAnimationParam()
		{
			eType = AnimationType::Animation_Flashing;
			vStartColor = Vector4(1.0);
			vEndColor = Vector4(1.0, 0.0, 0.0, 1.0);
			bGradient = false;
		}
		bool bGradient;
		Vector4 vStartColor;
		Vector4 vEndColor;
	};

	class VisiableAnimationParam : public AnimationParam
	{
	public:
		VisiableAnimationParam()
		{
			eType = AnimationType::Animation_Visiable;
			bGradient = false;
		}

		bool bGradient;
	};

	class AttributeAnimationParam : public AnimationParam
	{
	public:
		AttributeAnimationParam()
		{
			eType = AnimationType::Animation_Attribute;

			vStartFaceColor = Vector4(1.0);
			vEndFaceColor = Vector4(1.0, 0.0, 0.0, 1.0);

			vStartLineColor = Vector4(1.0);
			vEndLineColor = Vector4(1.0, 0.0, 0.0, 1.0);
			bGradient = false;
		}
		bool bGradient;

		Vector4 vStartFaceColor;
		Vector4 vEndFaceColor;
		Vector4 vStartLineColor;
		Vector4 vEndLineColor;
	};

	class GrowthAnimationParam : public AnimationParam
	{
	public:
		GrowthAnimationParam()
		{
			eType = AnimationType::Animation_Growth;
		}
	};

	class IAnimation
	{
	public:

		virtual ~IAnimation() = default;
		virtual void Init(const AnimationParam* pParam) = 0;

		virtual void iAdvance(float time) = 0;

		virtual void iPlay(const std::string& animationName = "", int playTimes = -1) = 0;
		virtual void iPlay(const AnimationConfig& config) = 0;
		virtual void iStop() = 0;
		virtual bool iIsDone() = 0;

		virtual void iAddTarget(INode* pNode) = 0;

		virtual AnimationState iGetState() = 0;
	};

	//class IAnimatioinCallback
	//{
	//public:
	//	virtual void iProgress(INode* pNode, float progress) = 0;
	//	virtual void iFinish(INode* pNode) = 0;
	//	virtual void iStop(INode* pNode) = 0;
	//	virtual void iStart(INode* pNode) = 0;
	//};

	//class IAnimation
	//{
	//public:
	//	enum TrackType 
	//	{
	//		TYPE_VALUE, // Set a value in a property, can be interpolated.
	//		TYPE_POSITION_3D, // Position 3D track, can be compressed.
	//		TYPE_ROTATION_3D, // Rotation 3D track, can be compressed.
	//		TYPE_SCALE_3D, // Scale 3D track, can be compressed.
	//		TYPE_BLEND_SHAPE, // Blend Shape track, can be compressed.
	//		TYPE_METHOD, // Call any method on a specific node.
	//		TYPE_BEZIER, // Bezier curve.
	//		TYPE_AUDIO,
	//		TYPE_ANIMATION,
	//	};

	//	enum InterpolationType 
	//	{
	//		INTERPOLATION_NEAREST,
	//		INTERPOLATION_LINEAR,
	//		INTERPOLATION_CUBIC,
	//		INTERPOLATION_LINEAR_ANGLE,
	//		INTERPOLATION_CUBIC_ANGLE,
	//	};

	//	enum UpdateMode
	//	{
	//		UPDATE_CONTINUOUS,
	//		UPDATE_DISCRETE,
	//		UPDATE_CAPTURE,
	//	};

	//	enum LoopMode
	//	{
	//		LOOP_NONE,
	//		LOOP_LINEAR,
	//		LOOP_PINGPONG,
	//	};

	//	enum LoopedFlag
	//	{
	//		LOOPED_FLAG_NONE,
	//		LOOPED_FLAG_END,
	//		LOOPED_FLAG_START,
	//	};

	//	enum FindMode 
	//	{
	//		FIND_MODE_NEAREST,
	//		FIND_MODE_APPROX,
	//		FIND_MODE_EXACT,
	//	};

	//	virtual int iAddTrack(TrackType p_type, int p_at_pos = -1) = 0;
	//	virtual void iRemoveTrack(int p_track) = 0;
	//	virtual int iGetTrackCount() const = 0;
	//	virtual TrackType iTrackGetType(int p_track) const = 0;

	//	//virtual void iTrackSetPath(int p_track, const std::string& path) = 0;
	//	//virtual const std::string& iTrackGetPath(int p_track) const = 0;
	//	//virtual int iFindTrack(const std::string& p_path, const TrackType p_type) const = 0;

	//	virtual void iAdvance(double pTime) = 0;
	//	virtual void iSetAnimationCallback(IAnimatioinCallback* pCallback) = 0;

	//	virtual void iAddNode(INode* pNode) = 0;
	//	virtual void iRemoveNode(INode* pNode) = 0;

	//	virtual int iInsertPositionTrackKey(int track, double time, const Vector3& position) = 0;
	//	virtual void iGetPositionTrackByKey(int track, int key, Vector3* position) = 0;

	//	virtual int iInsertRotationTrackKey(int track, double time, const Vector3& rotation) = 0;
	//	virtual void iGetRotationTrackByKey(int track, int key, Vector3* rotation) = 0;

	//	virtual int iInsertScaleTrackKey(int track, double time, const Vector3& scale) = 0;
	//	virtual void iGetScaleTrackByKey(int track, int key, Vector3* scale) = 0;

	//	virtual void iPlay() = 0;
	//	virtual void iPause() = 0;
	//	virtual void iStop() = 0;
	//	virtual void iSeek(double time) = 0;
	//	virtual bool iIsPlaying() const = 0;
	//};
}

