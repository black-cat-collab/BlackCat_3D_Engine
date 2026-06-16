#pragma once

namespace bc
{
	class TestClusterFunc : public ClusterFunc
	{
	public:
		TestClusterFunc();
		virtual ~TestClusterFunc();

	protected:
		virtual INode* iCreateClusterLabelNode(int nType);
		virtual void iUpdateClusterLabelNode(FactoryAssemble* pAssemble);
		virtual void iSetClusterLabelSelect(FactoryAssemble* pAssemble, bool bSelect);
	};
}

