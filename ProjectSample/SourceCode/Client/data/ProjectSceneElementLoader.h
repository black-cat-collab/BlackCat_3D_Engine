#pragma once
namespace bc 
{
	class ProjectSceneElementLoader:public DefaultProjectElementLoader
	{
	public:
		ProjectSceneElementLoader(ProjectBasedClient* pClient);
		virtual ~ProjectSceneElementLoader();

	protected:
		virtual CommonLabelFactory* iCreateElementFactory(int nType, int nNodeType);
	};
}


