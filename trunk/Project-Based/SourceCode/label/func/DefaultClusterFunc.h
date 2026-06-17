#pragma once

namespace bc
{
	class DefaultClusterFunc : public ClusterFunc
	{
	public:
		DefaultClusterFunc(CommonLabelFactory *pFactory);
		virtual ~DefaultClusterFunc();

	protected:
		virtual INode* iCreateClusterLabelNode(int nType);
		virtual void iUpdateClusterLabelNode(FactoryAssemble* pAssemble);
		virtual void iSetClusterLabelSelect(FactotryAssembleDetail* pAssemble, bool bSelect);

	private:
		CommonLabelFactory* m_pFactory;

	};
}


