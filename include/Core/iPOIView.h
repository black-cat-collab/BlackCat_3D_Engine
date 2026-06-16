#pragma once

namespace bc
{
	class POIData
	{
	public:
		int			nLayer;
		Vector3d		vPosition;
		float		fDistanceToCamera;
		std::string strID;
		std::string strText;
		Vector4		vColor;
		float		fFontScale;
		POIData() :
			nLayer(0),
			fDistanceToCamera(0.0f),
			vColor(1.0f),
			fFontScale(0)
		{

		}
	};

	class IPOIView : public IViewComponent
	{
	public:
		virtual int iGetViewComponentType()
		{
			return VIEW_COMPONENT_TYPE_POI;
		}

		virtual void	iAddLayer(const float& fHeight, const float& fDistance,
			const std::vector<POIData*>& vecData, const int& nBlockWidth = -1, const int& nBlockHeight = -1) = 0;
		virtual void	iClear() = 0;
		virtual void	iSetMaxVisibleCount(const int& nCount) = 0;
		virtual void	iSetMaxVisibleLayer(const int& nLayer) = 0;
		virtual void	iSetLoad(const bool& bLoad) = 0;
	};

}

