#pragma once

namespace bc
{
	class CommonLabelFactory;
	class PROJECT_BASED_API ReadHeightTifFunc : public IFunc
	{
	public:
		ReadHeightTifFunc(CommonLabelFactory *pComFactory);
		virtual ~ReadHeightTifFunc();
		ELabelFunc GetFuncType() { return FUNC_READ_HRIGHT_TIF; }
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent);
		CommonLabelFactory* GetComFactory();


	public:
		virtual void iCreateData(FactoryAssemble* pAssemble);
		virtual void iUpdateData(FactoryAssemble* pAssemble);
		virtual void iPreFrameUpdate();
		virtual void iFrameUpdate(FactoryAssemble* pAssemble);
		virtual void iPostFrameUpdate();

	private:	

	private:
		CommonLabelFactory* m_pComFactory;
		FactoryAssemble* m_pNearAssemble;		//离相机当前最近的需要读取高地图的Assemble
		float m_fNearDis;			//离相机最近的距离

	protected:

	};
}


