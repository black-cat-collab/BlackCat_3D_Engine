#pragma once

namespace bc
{
	class CommonLabelFactory;
	class PROJECT_BASED_API DetailScaleFunc : public IFunc
	{
	public:
		DetailScaleFunc(CommonLabelFactory *pComFactory);
		virtual ~DetailScaleFunc();
		ELabelFunc GetFuncType() { return FUNC_SCALE_DETAIL; }
		CommonLabelFactory* GetComFactory();
		//缩放详情面板（一直保持比如全屏这样的逻辑），fScale指正常的比例下的缩放倍数
		void ToScaleDetail(float fDisToCamera,FactotryAssembleDetail *pDetailAssemble);	
		//正常
		void ToNormalDetail(FactotryAssembleDetail* pDetailAssemble);


	public:
		virtual void iCreateData(FactoryAssemble* pAssemble);
		virtual void iUpdateData(FactoryAssemble* pAssemble);
		virtual void iPreFrameUpdate();
		virtual void iFrameUpdate(FactoryAssemble* pAssemble);
		virtual void iPostFrameUpdate();
		virtual void iOnSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);
		virtual void iCameraFrameUpdate(INode* pCameraNode);

	private:	

	private:
		CommonLabelFactory* m_pComFactory;
		std::vector<FactotryAssembleDetail*> m_vecCurDetailAssemble;
		std::map<int, float> m_mapDisToCamera;
	};
}


