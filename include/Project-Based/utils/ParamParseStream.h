#pragma once

#include <iostream>

#define SCENE_ELEMENT_PARAM_KEY std::string("scene_element_param_")
namespace bc
{
	enum eParam_ParseType
	{
		FILE_INPUT = 0,		//文件类型
		FLOAT_INPUT,
		INT_INPUT,
		LONG_INPUT,
		STRING_INPUT,
		TEXT,			//文本显示
		SELECT,			//下拉选择框
		BOOL_INPUT,
		VECTOR2_INPUT,
		VECTOR3_INPUT,
		VECTOR4_INPUT,
		ADD_INPUT,			//可变类型（及可增加，可减少）
		FIX_COUNT_INPUT,	//个数确定的,不可变
		GROUP_INPUT,		//结构,即父结构
		COLOR_INPUT,		//颜色选择
	};

	class ReadParam {
	public:
		std::string strKey;
		int nNodeType;
		std::string strDescription;
		NodeParamBase *pParam;
		std::string strNodeTypeName;
		int nSort;

		ReadParam()
		{
			nNodeType = -1;
			pParam = NULL;
			nSort = 200;
		}

		ReadParam& operator = (const ReadParam &src)
		{
			strKey = src.strKey;
			nNodeType = src.nNodeType;
			strDescription = src.strDescription;
			pParam = src.pParam;
			strNodeTypeName = src.strNodeTypeName;
			nSort = src.nSort;

			return (*this);
		}

	};

	class PROJECT_BASED_API ParamParseStream
	{
	public:
		virtual ~ParamParseStream();

		bool IsContainKey(std::string strKey);
		//添加到json中
		Json::Value AddParamToJson(NodeParamBase *pParam, std::string strKey, std::string strDescription, std::string strFilePath = "");
		//修改数据
		Json::Value ModifyParamToJson(NodeParamBase *pParam, std::string strKey);
		//更改整体，即Param的整个数据
		Json::Value ModifyParamToJson(std::string strJsonData, std::string strKey);
		//读取json转换成Param
		void ReadParamFromString(const std::string strJson, std::string strKey = "");
		void ReadParamFromFile(std::string strFile);
		ReadParam* GetReadParamByKey(std::string strKey);
		void Clear(std::string strKey = "");
		void ReadJsonToParam(ReadParam &stParam, std::string strJson);
		std::string ConvertParamToJson(NodeParamBase *pParam, Json::Value &value);

		static ParamParseStream* GetInstance();
		int GetNodeTypeByName(std::string strName);

		//TextShow
		std::string ConvertTextShowToJson(TextShow &textShow);

	protected:
		void readJsonToParam(ReadParam &stParam, std::string strJson);
		void readSingleReadParam(ReadParam &stParam, Json::Value value);
		//基础项的写入
		Json::Value createItemJson(std::string strText, std::string strKey, eParam_ParseType eType,int nSort,
			const Json::Value value, std::string strDescription = "");
		Json::Value& getItem(Json::Value &parent, std::string strKey);
		Json::Value& getItemValue(Json::Value &parent, std::string strKey);

		Json::Value* getItemRecursion(Json::Value &parent, std::string strKey);
		Json::Value* getItemValueRecursion(Json::Value &parent, std::string strKey);
		//NodeParamBase
		virtual void readNodeParamBase(const std::string strJson, NodeParamBase& pParam);
		virtual void readNodeParamBase(Json::Value root, NodeParamBase& pParam);
		virtual void convertNodeParamBase(NodeParamBase* pParam, Json::Value& jsonItem);
		//BoardParam
		virtual void readBoardParam(const std::string strJson, BoardParam &pParam);
		virtual void readBoardParam(Json::Value root, BoardParam &pParam);
		virtual void convertBoardParam(BoardParam *pParam,Json::Value &jsonItem);
		//BoardBaseParam
		virtual void readBoardBaseParam(const std::string strJson, BoardBaseParam &pParam);
		virtual void readBoardBaseParam(Json::Value root, BoardBaseParam &pParam);
		virtual void convertBoardBaseParam(BoardBaseParam *pParam, Json::Value &jsonItem);
		//FontBoardParam
		virtual void readFontBoardParam(const std::string strJson, FontBoardParam &pParam);
		virtual void readFontBoardParam(Json::Value root, FontBoardParam &pParam);
		virtual void convertFontBoardParam(FontBoardParam *pParam, Json::Value &jsonItem);
		//VideoBoardParam
		virtual void readVideoBoardParam(const std::string strJson, VideoBoardParam &pParam);
		virtual void readVideoBoardParam(Json::Value root, VideoBoardParam &pParam);
		virtual void convertVideoBoardParam(VideoBoardParam *pParam, Json::Value &jsonItem);
		//ButtonBoardParam
		virtual void readButtonBoardParam(const std::string strJson, ButtonBoardParam &pParam);
		virtual void readButtonBoardParam(Json::Value root, ButtonBoardParam &pParam);
		virtual void convertButtonBoardParam(ButtonBoardParam *pParam, Json::Value &jsonItem);
		//FontBoardWithButtonParam
		virtual void readFontBoardWithButtonParam(const std::string strJson, FontBoardWithButtonParam &pParam);
		virtual void readFontBoardWithButtonParam(Json::Value root, FontBoardWithButtonParam &pParam);
		virtual void convertFontBoardWithButtonParam(FontBoardWithButtonParam *pParam, Json::Value &jsonItem);
		//ChildVideoBoardParam
		//virtual void readChildVideoBoardParam(const string strJson, ChildVideoBoardParam &pParam);
		//virtual void readChildVideoBoardParam(Json::Value root, ChildVideoBoardParam &pParam);
		//virtual void convertChildVideoBoardParam(ChildVideoBoardParam *pParam, Json::Value &jsonItem);
		//VideoBoardWithButtonParam
		virtual void readVideoBoardWithButtonParam(const std::string strJson, VideoBoardWithButtonParam &pParam);
		virtual void readVideoBoardWithButtonParam(Json::Value root, VideoBoardWithButtonParam &pParam);
		virtual void convertVideoBoardWithButtonParam(VideoBoardWithButtonParam *pParam, Json::Value &jsonItem);
		//BoardWithFontBoardParam
		virtual void readBoardWithFontBoardParam(const std::string strJson, BoardWithFontBoardParam &pParam);
		virtual void readBoardWithFontBoardParam(Json::Value root, BoardWithFontBoardParam &pParam);
		virtual void convertBoardWithFontBoardParam(BoardWithFontBoardParam *pParam, Json::Value &jsonItem);
		//SpotLightBoardParam
		virtual void readSpotLightBoardParam(const std::string strJson, SpotLightBoardParam &pParam);
		virtual void readSpotLightBoardParam(Json::Value root, SpotLightBoardParam &pParam);
		virtual void convertSpotLightBoardParam(SpotLightBoardParam *pParam, Json::Value &jsonItem);
		//LightBeamParam
		virtual void readLightBeamParam(const std::string strJson, LightBeamParam &pParam);
		virtual void readLightBeamParam(Json::Value root, LightBeamParam &pParam);
		virtual void convertLightBeamParam(LightBeamParam *pParam, Json::Value &jsonItem);
		//PolygonParam
		virtual void readPolygonParam(const std::string strJson, PolygonParam &pParam);
		virtual void readPolygonParam(Json::Value root, PolygonParam &pParam);
		virtual void convertPolygonParam(PolygonParam *pParam, Json::Value &jsonItem);
		//RoadParam
		virtual void readRoadParam(const std::string strJson, RoadParam &pParam);
		virtual void readRoadParam(Json::Value root, RoadParam &pParam);
		virtual void convertRoadParam(RoadParam *pParam, Json::Value &jsonItem);
		virtual void readRoadParamV1(Json::Value root, RoadParam& pParam);
		virtual void convertRoadParamV1(RoadParam* pParam, Json::Value& jsonItem);
		//BoardInstanceParam
		virtual void readBoardInstanceParam(const std::string strJson, BoardInstanceParam &pParam);
		virtual void readBoardInstanceParam(Json::Value root, BoardInstanceParam &pParam);
		virtual void convertBoardInstanceParam(BoardInstanceParam *pParam, Json::Value &jsonItem);
		//LocusLineParam
		virtual void readLocusLineParam(const std::string strJson, LocusLineParam &pParam);
		virtual void readLocusLineParam(Json::Value root, LocusLineParam &pParam);
		virtual void convertLocusLineParam(LocusLineParam *pParam, Json::Value &jsonItem);
		//FlyLineParam
		virtual void readFlyLineParam(const std::string strJson, FlyLineParam& pParam);
		virtual void readFlyLineParam(Json::Value root, FlyLineParam& pParam);
		virtual void convertFlyLineParam(FlyLineParam* pParam, Json::Value& jsonItem);
		//MultiFlyLineParam
		virtual void readMultiFlyLineParam(const std::string strJson, MultiFlyLineParam& pParam);
		virtual void readMultiFlyLineParam(Json::Value root, MultiFlyLineParam& pParam);
		virtual void convertMultiFlyLineParam(MultiFlyLineParam* pParam, Json::Value& jsonItem);
		//TextShow
		virtual void readTextShow(const std::string strJson, TextShow& textShow);
		virtual void readTextShow(Json::Value root, TextShow& textShow);
		virtual void convertTextShow(TextShow* pParam, Json::Value& jsonItem);


	protected:
		ParamParseStream();

		std::vector<ReadParam> m_vecReadParam;


	private:
		static ParamParseStream *m_Instance;

		Json::Value m_JRoot;
		Json::Value m_tempRoot;		//出错时的返回
		Json::Value m_tempValue;		//出错时的返回

	private:
		class GC // 垃圾回收类
		{
		public:
			GC()
			{
				std::cout << "GC construction" << std::endl;
			}

			~GC()
			{
				std::cout << "GC destruction" << std::endl;
				if (m_Instance != NULL)
				{
					delete m_Instance;
					m_Instance = NULL;
					std::cout << "ParamParseStream construction" << std::endl;
				}
			}
		};
		static GC gc;  //垃圾回收类的静态成员
	};
}


