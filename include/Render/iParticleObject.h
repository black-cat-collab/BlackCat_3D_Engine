#pragma once

namespace bc
{
	typedef struct ParticleProperty_s
	{	
		// 位置
		Vector3 position;
		// 速度
		Vector3 velocity;
		//年龄
		float age;
		//alpha通道
		float alpha;
		//粒子点精灵大小
		float size;
		//寿命
		float life;
		// 随机数
		Vector3	randomA;
		// 随机数
		Vector3	randomB;

		ParticleProperty_s() :
			age(0.0),
			alpha(0.0),
			size(0.0),
			life(0.0),
			randomA(Vector3(0.0)),
			randomB(Vector3(0.0)),
			position(Vector3(0.0)),
			velocity(Vector3(0.0))
		{

		}
	}ParticleProperty;

	typedef void(*ParticleInitialize)(ParticleProperty* pParticleProperty, const int& nSize, void* pUser);
	
	class IParticleObject
	{
	public:
		virtual void iSwapIndex() = 0;
	};
}

