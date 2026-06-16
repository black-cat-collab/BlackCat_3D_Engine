#pragma once

namespace bc
{
	class CommonLabelFactory;
	class FactoryAssemble;
	class IFunc
	{
	public:
		IFunc();
		virtual ~IFunc();

		virtual void iCreateData(FactoryAssemble* pAssemble) {}		//一开始创建的回调
		virtual void iUpdateData(FactoryAssemble* pAssemble) {}
		virtual void iPreFrameUpdate() {}
		virtual void iFrameUpdate(FactoryAssemble* pAssemble) {}
		virtual void iPostFrameUpdate() {}
		virtual void iShowAssembleForLabel(FactoryAssemble* pAssemble) {}
		virtual void iHideAssembleForLabel(FactoryAssemble* pAssemble) {}
		virtual void iChangeVisible(bool bAllVisible,const std::vector<int> &vecVisibleType) {}
		virtual void iSetLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect) {}
		virtual void iCameraFrameUpdate(INode *pCameraNode) {}
		virtual EventReturnType_e iProcessEvent(const BCEvent& tEvent) { return EventReturnType_e::NONE; }

		ELabelFunc GetFuncType() { return (ELabelFunc)(-1); }
		virtual bool isAllStop() { return true; }  //是否都停止了，用于删除delete自身检测用，因为有线程得保证线程停止了才delete

	protected:

	};
}


