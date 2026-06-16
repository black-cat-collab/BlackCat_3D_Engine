#include "UserHeader.h"
#include "ProjectSceneElementLoader.h"
#include "../ProjectClientAPI.h"
#include "../test/TestClusterFunc.h"

bc::ProjectSceneElementLoader::ProjectSceneElementLoader(ProjectBasedClient* pClient)
	:DefaultProjectElementLoader(pClient)
{
}

bc::ProjectSceneElementLoader::~ProjectSceneElementLoader()
{

}

CommonLabelFactory* ProjectSceneElementLoader::iCreateElementFactory(int nType, int nNodeType)
{
	CommonLabelFactory* pFactory = nullptr;

	//TestClusterFunc* pClusterFunc = ((ProjectClientAPI*)m_pClient)->m_pTestClusterFunc;
	//pFactory->AddFunc(ELabelFunc::FUNC_CLUSTER, pClusterFunc);
	//pClusterFunc->AddFactory(pFactory);

	ProjectBasedSceneElementTypeInfo* pTypeInfo = GetElementTypeInfoByID(nType);
	if (pTypeInfo)
	{
		pFactory = SceneElementLoader::iCreateElementFactory(nType, nNodeType);
	}

	pFactory->AddFunc(FUNC_READ_HRIGHT_TIF);

	return pFactory;
}
