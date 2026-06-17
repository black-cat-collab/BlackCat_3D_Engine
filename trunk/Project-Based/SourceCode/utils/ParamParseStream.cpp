
#include "ProjectBasedHeader.h"
#include "Project-Based/utils/pLocal.h"
#include "Project-Based/utils/ParamParseStream.h"

using namespace bc;

#define ITEM_CHILDREN ("children")
#define JSON_PARAM ("param")

ParamParseStream* ParamParseStream::m_Instance = NULL;
ParamParseStream::GC ParamParseStream::gc;
ParamParseStream::ParamParseStream()
{
	m_JRoot = Json::Value(Json::arrayValue);
	m_tempRoot = Json::Value(Json::objectValue);
}


ParamParseStream::~ParamParseStream()
{
	for (int i = 0; i < m_vecReadParam.size(); i++)
	{
		if (m_vecReadParam[i].pParam)
		{
			delete m_vecReadParam[i].pParam;
			m_vecReadParam[i].pParam = nullptr;
		}
	}
	m_vecReadParam.clear();
}

ParamParseStream* ParamParseStream::GetInstance()
{
	if (m_Instance == nullptr)
	{
		m_Instance = new ParamParseStream();
	}
	return m_Instance;
}

bool ParamParseStream::IsContainKey(std::string strKey)
{
	for (int i = 0; i < m_vecReadParam.size(); i++)
	{
		if (m_vecReadParam[i].strKey == strKey)
		{
			return true;
		}
	}
	return false;
}

Json::Value ParamParseStream::AddParamToJson(NodeParamBase *pParam, std::string strKey, std::string strDescription, std::string strFilePath)
{
	if (!pParam)
	{
		return NULL;
	}
	Json::Value *tempTarget = getItemRecursion(m_JRoot,strKey);
	if (tempTarget != NULL)
	{
		return NULL;
	}
	Json::Value value;
	char szBuffer[1024] = { 0 };
	//AsciiToUtf8(strDescription.c_str(), sizeof(strDescription.c_str()), szBuffer);
	value["key"] = strKey;
	value["description"] = strDescription;
	value[JSON_PARAM] = Json::Value(Json::ValueType::arrayValue);
	value["nodeType"] = pParam->nNodeType;

	std::string strTypeName = ConvertParamToJson(pParam,value[JSON_PARAM]);
	value["nodeTypeName"] = strTypeName;
	value["sort"] = RENDER_SORT_BOARD;

	m_JRoot.append(value);
	if (!strFilePath.empty())
	{
		std::string strData = value.toStyledString();
		WriteToFile(strFilePath, strData.c_str());
	}
	ReadParam stParam;
	stParam.strKey = strKey;
	stParam.strDescription = strDescription;
	stParam.nNodeType = pParam->nNodeType;
	stParam.pParam = pParam;
	
	return value;
}

Json::Value ParamParseStream::ModifyParamToJson(NodeParamBase *pParam, std::string strKey)
{
	if (!pParam)
	{
		return NULL;
	}
	Json::Value *tempTarget = getItemRecursion(m_JRoot, strKey);
	if (tempTarget == NULL)
	{
		return NULL;
	}
	(*tempTarget)[JSON_PARAM] = Json::Value(Json::ValueType::arrayValue);
	BoardParam *pBoardParam = dynamic_cast<BoardParam*>(pParam);
	if (pParam)
	{
		convertBoardParam(pBoardParam, (*tempTarget)[JSON_PARAM]);
	}
	else if (pParam->nNodeType == NODE_POLYGON)
	{
		convertPolygonParam((PolygonParam*)pParam, (*tempTarget)[JSON_PARAM]);
	}
	else if (pParam->nNodeType == NODE_ROAD)
	{
		convertRoadParam((RoadParam*)pParam, (*tempTarget)[JSON_PARAM]);
	}
	return *tempTarget;
}

Json::Value ParamParseStream::ModifyParamToJson(std::string strJsonData, std::string strKey)
{
	ReadParam stParam;
	readJsonToParam(stParam,strJsonData);

	return NULL;
}

ReadParam* ParamParseStream::GetReadParamByKey(std::string strKey)
{
	for (int i = 0; i < m_vecReadParam.size(); i++)
	{
		if (m_vecReadParam[i].strKey == strKey)
		{
			return &(m_vecReadParam[i]);
		}
	}
	return NULL;
}

void ParamParseStream::ReadParamFromFile(std::string strFile)
{
	std::string strJson = ReadFile(strFile);
	if (!strJson.empty())
	{
		ReadParamFromString(strJson);
	}	
}

void ParamParseStream::ReadParamFromString(const std::string strJson, std::string strKey)
{
	try
	{
		Json::CharReaderBuilder jsonBuilder;
		std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
		Json::Value root;
		std::string jsError;
		if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root,&jsError))
		{
			if (root.isArray())
			{
				for (int i = 0; i < root.size(); i++)
				{
					ReadParam readParam;
					readSingleReadParam(readParam, root[i]);
					if (!strKey.empty())
					{
						readParam.strKey = strKey;
					}

					m_vecReadParam.push_back(readParam);
				}
			}
			else if (root.isObject())
			{
				ReadParam readParam;
				readSingleReadParam(readParam, root);
				if (!strKey.empty())
				{
					readParam.strKey = strKey;
				}

				m_vecReadParam.push_back(readParam);
			}
		}
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::readSingleReadParam(ReadParam &stParam, Json::Value value)
{
	if (!value.isObject())
	{
		return;
	}
	try
	{
		int nType = NODE_UNKNOWN;
		int nTempType = value["nodeType"].asInt();
		Json::Value param = value["param"];
		stParam.strKey = value["key"].asString();
		stParam.strDescription = value["description"].asString();
		if (value.isMember("nodeTypeName"))
		{
			stParam.strNodeTypeName = value["nodeTypeName"].asString();
		}
		if (value["sort"].isString())
		{
			stParam.nSort = atoi(value["sort"].asCString());
		}
		else if (value["sort"].isInt())
		{
			stParam.nSort = value["sort"].asInt();
		}
		nType = GetNodeTypeByName(stParam.strNodeTypeName.c_str());
		if (nType == NODE_UNKNOWN)
		{
			nType = nTempType;
		}
		stParam.nNodeType = nType;

		NodeParamBase *pParam = NULL;
		if (nType == NODE_FONTBOARD)
		{
			pParam = new FontBoardParam;
			readFontBoardParam(param, (FontBoardParam&)(*pParam));
		}
		else if (nType == NODE_BOARD)
		{
			pParam = new BoardParam;
			readBoardParam(param, (BoardParam&)(*pParam));
		}
		else if (nType == NODE_VIDEO_BOARD)
		{
			pParam = new VideoBoardParam;
			readVideoBoardParam(param, (VideoBoardParam&)(*pParam));
		}
		else if (nType == NODE_FONTBOARD_WITH_BUTTON)
		{
			pParam = new FontBoardWithButtonParam;
			readFontBoardWithButtonParam(param, (FontBoardWithButtonParam&)(*pParam));
		}
		else if (nType == NODE_VIDEO_BOARD_WITH_BUTTON)
		{
			pParam = new VideoBoardWithButtonParam;
			readVideoBoardWithButtonParam(param, (VideoBoardWithButtonParam&)(*pParam));
		}
		else if (nType == NODE_BOARD_WITH_FONTBOARD)
		{
			pParam = new BoardWithFontBoardParam;
			readBoardWithFontBoardParam(param, (BoardWithFontBoardParam&)(*pParam));
		}
		else if (nType == NODE_SPOT_LIGHT_BOARD)
		{
			pParam = new SpotLightBoardParam;
			readSpotLightBoardParam(param, (SpotLightBoardParam&)(*pParam));
		}
		else if (nType == NODE_LIGHT_BEAM)
		{
			pParam = new LightBeamParam;
			readLightBeamParam(param, (LightBeamParam&)(*pParam));
		}
		else if (nType == NODE_POLYGON)
		{
			pParam = new PolygonParam;
			readPolygonParam(param, (PolygonParam&)(*pParam));
		}
		else if (nType == NODE_ROAD)
		{
			pParam = new RoadParam;
			readRoadParam(param, (RoadParam&)(*pParam));
		}
		else if (nType == NODE_BOARD_INSTANCE)
		{
			pParam = new BoardInstanceParam;
			readBoardInstanceParam(param, (BoardInstanceParam&)(*pParam));
		}
		else if (nType == NODE_LOCUS_LINE)
		{
			pParam = new LocusLineParam;
			readLocusLineParam(param, (LocusLineParam&)(*pParam));
		}
		else if (nType == NODE_FLYLINE)
		{
			pParam = new FlyLineParam;
			readFlyLineParam(param, (FlyLineParam&)(*pParam));
		}
		else if (nType == NODE_MULTI_FLYLINE)
		{
			pParam = new MultiFlyLineParam;
			readMultiFlyLineParam(param, (MultiFlyLineParam&)(*pParam));
		}

		stParam.pParam = pParam;
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::readJsonToParam(ReadParam &stParam, std::string strJson)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value value;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &value,&jsError))
	{
		if (value.isObject())
		{
			readSingleReadParam(stParam, value);
		}
		else if (value.isArray())
		{
			for (int i = 0; i < value.size(); i++)
			{
				readSingleReadParam(stParam, value[i]);
			}
		}
	}
}

Json::Value ParamParseStream::createItemJson(std::string strText, std::string strKey, eParam_ParseType eType,
	int nSort, const Json::Value value, std::string strDescription)
{
	char szBuffer[1024] = { 0 };
	Json::Value item;
	//AsciiToUtf8(strText.c_str(), sizeof(strText.c_str()), szBuffer);
	item["text"] = strText;
	//AsciiToUtf8(strKey.c_str(), sizeof(strKey.c_str()), szBuffer);
	item["key"] = strKey;
	item["eType"] = eType;
	if (value.isString())
	{
		//AsciiToUtf8(value.asCString(), sizeof(value.asCString()), szBuffer);
		item["value"] = value;
	}
	else if (value.isBool())
	{
		item["value"] = value.asBool();
	}
	else if (value.isInt())
	{
		item["value"] = std::to_string(value.asInt());
	}
	else if (value.isDouble())
	{
		item["value"] = std::to_string(value.asDouble());
	}
	else
	{
		item["value"] = value;
	}
	item["sort"] = nSort;
	//AsciiToUtf8(strDescription.c_str(), sizeof(strDescription.c_str()), szBuffer);
	item["description"] = strDescription;
	item[ITEM_CHILDREN] = Json::Value(Json::ValueType::arrayValue);
	return item;
}

Json::Value& ParamParseStream::getItem(Json::Value &parent, std::string strKey)
{
	Json::Value *value = getItemRecursion(parent,strKey);
	if (value == NULL)
	{
		return m_tempRoot;
	}
	return *value;
}

Json::Value& ParamParseStream::getItemValue(Json::Value &parent, std::string strKey)
{
	Json::Value *value = getItemValueRecursion(parent, strKey);
	if (value == NULL)
	{
		m_tempValue = Json::Value(Json::nullValue);
		return m_tempValue;
	}
	return *value;
}

Json::Value* ParamParseStream::getItemRecursion(Json::Value &parent, std::string strKey)
{
	if (parent.isObject())
	{
		if (parent.isMember("key") && parent["key"].asString() == strKey)
		{
			return &parent;
		}
		else 
		{
			return NULL;
		}
	}
	else if (parent.isArray())
	{
		for (int i = 0; i < parent.size(); i++)
		{
			Json::Value* result = getItemRecursion(parent[i],strKey);
			if (result != NULL)
			{
				return result;
			}
		}
	}

	return NULL;
}

Json::Value* ParamParseStream::getItemValueRecursion(Json::Value &parent, std::string strKey)
{
	Json::Value *root = getItemRecursion(parent,strKey);
	if (root != NULL)
	{
		Json::Value &value = *root;
		return &(value["value"]);
	}
	return NULL;
}

//NodeParamBase
void ParamParseStream::readNodeParamBase(const std::string strJson, NodeParamBase& pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readNodeParamBase(root, pParam);
	}
}

void ParamParseStream::readNodeParamBase(Json::Value root, NodeParamBase& pParam)
{
	try
	{
		if (!getItemValue(root, "strFlagName").isNull())
		{
			pParam.strFlagName = getItemValue(root, "strFlagName").asString();
		}
		if (!getItemValue(root, "nVersion").isNull())
		{
			pParam.nVersion = atoi(getItemValue(root, "nVersion").asCString());
		}
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertNodeParamBase(NodeParamBase* pParam, Json::Value& jsonItem)
{
	jsonItem.append(createItemJson("业务标识名称", "strFlagName", STRING_INPUT, 1, pParam->strFlagName));
	jsonItem.append(createItemJson("版本", "nVersion", INT_INPUT, 1, pParam->nVersion));
}

void ParamParseStream::readBoardParam(const std::string strJson, BoardParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root,&jsError))
	{
		readBoardParam(root, pParam);
	}
}

void ParamParseStream::readBoardParam(Json::Value root, BoardParam &pParam)
{
	try
	{
		readNodeParamBase(root, pParam);
		if (!getItemValue(root, "strID").isNull())
		{
			pParam.strFlagName = getItemValue(root, "strID").asString();
		}
		pParam.fWidth = atof(getItemValue(root, "fWidth").asCString());
		pParam.fHeight = atof(getItemValue(root, "fHeight").asCString());
		pParam.fLength = atof(getItemValue(root, "fLength").asCString());
		pParam.vBoardCenter = StringToVector2(getItemValue(root, "vBoardCenter").asCString());
		pParam.strImage = getItemValue(root, "strImage").asString();
		pParam.bClickEnable = getItemValue(root, "bClickEnable").asBool();
		pParam.bCheckEnable = getItemValue(root, "bCheckEnable").asBool();
		pParam.bHoverEnable = getItemValue(root, "bHoverEnable").asBool();
		if (!getItemValue(root, "fScale").isNull())
		{
			pParam.fScale = atof(getItemValue(root, "fScale").asCString());
		}
		if (!getItemValue(root, "fSelectedRate").isNull())
		{
			pParam.fSelectedRate = atof(getItemValue(root, "fSelectedRate").asCString());
		}
		if (!getItemValue(root, "bClickScale").isNull())
		{
			pParam.bClickScale = getItemValue(root, "bClickScale").asBool();
		}
		pParam.strPlaceHolderImage = getItemValue(root, "strPlaceHolderImage").asString();
		if (!getItemValue(root, "strModelName").isNull())
		{
			pParam.strModelName = getItemValue(root, "strModelName").asString();
		}
		if (!pParam.strModelName.empty())
		{
			pParam.bCreateModel = true;
		}
		if (!getItemValue(root, "vModelAngle").isNull())
		{
			pParam.vModelAngle = StringToVector3(getItemValue(root, "vModelAngle").asCString());
		}
		if (!getItemValue(root, "vModelScale").isNull())
		{
			pParam.vModelScale = StringToVector3(getItemValue(root, "vModelScale").asCString());
		}
		pParam.strCheckedImage = getItemValue(getItem(root, "bCheckEnable")[ITEM_CHILDREN], "strCheckedImage").asString();
		if (!pParam.bCheckEnable)
		{
			pParam.strCheckedImage = "";
		}
		pParam.strHoverImage = getItemValue(getItem(root, "bHoverEnable")[ITEM_CHILDREN], "strHoverImage").asString();
		pParam.direction = atoi(getItemValue(root, "direction").asCString());
		pParam.bDynamicScaleEnable = getItemValue(root, "bDynamicScaleEnable").asBool();
		pParam.fDynamicScale = atof(getItemValue(getItem(root, "bDynamicScaleEnable")[ITEM_CHILDREN], "fDynamicScale").asCString());
		if (pParam.bDynamicScaleEnable)
		{
			pParam.fScale = pParam.fDynamicScale;
		}
		pParam.fDynamicMaxScale = atof(getItemValue(getItem(root, "bDynamicScaleEnable")[ITEM_CHILDREN], "fDynamicMaxScale").asCString());
		pParam.fDynamicMinScale = atof(getItemValue(getItem(root, "bDynamicScaleEnable")[ITEM_CHILDREN], "fDynamicMinScale").asCString());
		if (!getItemValue(root, "fVisibleDistance").isNull())
		{
			pParam.fVisibleDistance = atof(getItemValue(root, "fVisibleDistance").asCString());
		}
		if (!getItemValue(root, "fHideLessDistance").isNull())
		{
			pParam.fHideLessDistance = atof(getItemValue(root, "fHideLessDistance").asCString());
		}
		pParam.bGlowEnable = getItemValue(root, "bGlowEnable").asBool();
		if (!getItemValue(root, "fGlowIntensity").isNull())
		{
			pParam.fGlowIntensity = atof(getItemValue(root, "fGlowIntensity").asCString());
		}
		pParam.bGlowEnable = getItemValue(root, "bGlowEnable").asBool();
		pParam.bWithLine = getItemValue(root, "bWithLine").asBool();
		pParam.vLineColor = StringToVector4(getItemValue(getItem(root, "bWithLine")[ITEM_CHILDREN], "vLineColor").asCString());
		pParam.vLineColor = ConvertColorFormat255(pParam.vLineColor);
		pParam.vHoverLineColor = StringToVector4(getItemValue(getItem(root, "bWithLine")[ITEM_CHILDREN], "vHoverLineColor").asCString());
		pParam.vHoverLineColor = ConvertColorFormat255(pParam.vHoverLineColor);
		pParam.vCheckedLineColor = StringToVector4(getItemValue(getItem(root, "bWithLine")[ITEM_CHILDREN], "vCheckedLineColor").asCString());
		pParam.vCheckedLineColor = ConvertColorFormat255(pParam.vCheckedLineColor);
		pParam.fLineWidth = atof(getItemValue(getItem(root, "bWithLine")[ITEM_CHILDREN], "fLineWidth").asCString());
		pParam.bLinePoint = getItemValue(getItem(root, "bWithLine")[ITEM_CHILDREN], "bLinePoint").asBool();
		pParam.bWithBase = getItemValue(root, "bWithBase").asBool();
		readBoardBaseParam(getItem(root, "bWithBase")[ITEM_CHILDREN], pParam.tBaseParam);
		pParam.bRotateAroundTarget = getItemValue(root, "bRotateAroundTarget").asBool();
		pParam.vDistanceToTarget = StringToVector2(getItemValue(getItem(root, "bRotateAroundTarget")[ITEM_CHILDREN], "vDistanceToTarget").asCString());

		if (!getItemValue(root, "bNeedClarity").isNull())
		{
			pParam.bNeedClarity = getItemValue(root, "bNeedClarity").asBool();
		}
		if (!getItemValue(root, "fAlphaThreshold").isNull())
		{
			pParam.fAlphaThreshold = atof(getItemValue(root, "fAlphaThreshold").asCString());
		}
		if (!getItemValue(root, "eTransparentType").isNull())
		{
			pParam.eTransparentType = (TransparentType_e)atoi(getItemValue(root, "eTransparentType").asCString());
		}
		if (!getItemValue(root, "vTopLeftOffset").isNull())
		{
			pParam.vTopLeftOffset = StringToVector2(getItemValue(root, "vTopLeftOffset").asCString());
		}
		if (!getItemValue(root, "bDepthTest").isNull())
		{
			pParam.bDepthTest = getItemValue(root, "bDepthTest").asBool();
		}
		if (!getItemValue(root, "bDepthClamp").isNull())
		{
			pParam.bDepthClamp = getItemValue(root, "bDepthClamp").asBool();
		}
		if (pParam.nNodeType == NODE_BOARD_INSTANCE)
		{
			if (pParam.bCheckEnable)
			{
				pParam.strHoverImage = pParam.strCheckedImage;
			}
			else
			{
				pParam.strHoverImage = "";
			}
		}
		pParam.bCheckEnable = false;	//不使用自带的状态切换，业务来处理
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertBoardParam(BoardParam *pParam,Json::Value &jsonItem)
{
	//主要
	convertNodeParamBase(pParam, jsonItem);
	jsonItem.append(createItemJson("宽","fWidth",FLOAT_INPUT,1,pParam->fWidth));
	jsonItem.append(createItemJson("高", "fHeight", FLOAT_INPUT, 1, pParam->fHeight));
	jsonItem.append(createItemJson("厚", "fLength", FLOAT_INPUT, 1, pParam->fLength));
	jsonItem.append(createItemJson("中点", "vBoardCenter", VECTOR2_INPUT, 1, Vector2ToString(pParam->vBoardCenter)));
	jsonItem.append(createItemJson("常规放大倍数", "fScale", FLOAT_INPUT, 1, pParam->fScale));
	jsonItem.append(createItemJson("常规选中放大倍数", "fSelectedRate", FLOAT_INPUT, 1, pParam->fSelectedRate));
	jsonItem.append(createItemJson("是否点击后能缩放", "bClickScale", BOOL_INPUT, 1, pParam->bClickScale));
	jsonItem.append(createItemJson("常规背景", "strImage", FILE_INPUT, 1, pParam->strImage));
	jsonItem.append(createItemJson("占位图", "strPlaceHolderImage", FILE_INPUT, 1, pParam->strPlaceHolderImage));
	jsonItem.append(createItemJson("模型路径", "strModelName", FILE_INPUT, 1, pParam->strModelName));
	jsonItem.append(createItemJson("模型角度", "vModelAngle", VECTOR3_INPUT, 1, Vector3ToString(Vector3d(pParam->vModelAngle))));
	jsonItem.append(createItemJson("模型缩放", "vModelScale", VECTOR3_INPUT, 1, Vector3ToString(Vector3d(pParam->vModelScale))));
	jsonItem.append(createItemJson("是否可点击", "bClickEnable", BOOL_INPUT, 1, pParam->bClickEnable));
	jsonItem.append(createItemJson("是否可选中", "bCheckEnable", BOOL_INPUT, 1, pParam->bCheckEnable));
	jsonItem.append(createItemJson("是否可悬停", "bHoverEnable", BOOL_INPUT, 1, pParam->bHoverEnable));
	getItem(jsonItem, "bCheckEnable")[ITEM_CHILDREN].append(createItemJson("选中背景", "strCheckedImage", FILE_INPUT, 1, pParam->strCheckedImage));
	getItem(jsonItem, "bHoverEnable")[ITEM_CHILDREN].append(createItemJson("悬停背景", "strHoverImage", FILE_INPUT, 1, pParam->strHoverImage));
	jsonItem.append(createItemJson("方向类型", "direction", SELECT, 1, pParam->direction, "0:NormalDirect,1:BillBoard,2:FrontToCamera"));
	jsonItem.append(createItemJson("是否可动态缩放", "bDynamicScaleEnable", BOOL_INPUT, 1, pParam->bDynamicScaleEnable));
	getItem(jsonItem, "bDynamicScaleEnable")[ITEM_CHILDREN].append(createItemJson("动态缩放比例", "fDynamicScale", FLOAT_INPUT, 1, pParam->fDynamicScale));
	getItem(jsonItem, "bDynamicScaleEnable")[ITEM_CHILDREN].append(createItemJson("最大动态缩放比例", "fDynamicMaxScale", FLOAT_INPUT, 1, pParam->fDynamicMaxScale));
	getItem(jsonItem, "bDynamicScaleEnable")[ITEM_CHILDREN].append(createItemJson("最小动态缩放比例", "fDynamicMinScale", FLOAT_INPUT, 1, pParam->fDynamicMinScale));
	jsonItem.append(createItemJson("距离范围内可见", "fVisibleDistance", FLOAT_INPUT, 1, pParam->fVisibleDistance));
	jsonItem.append(createItemJson("小于范围内隐藏", "fHideLessDistance", FLOAT_INPUT, 1, pParam->fHideLessDistance));
	jsonItem.append(createItemJson("是否有辉光", "bGlowEnable", BOOL_INPUT, 1, pParam->bGlowEnable));
	jsonItem.append(createItemJson("辉光强度", "fGlowIntensity", FLOAT_INPUT, 1, pParam->fGlowIntensity));
	jsonItem.append(createItemJson("是否有引线", "bWithLine", BOOL_INPUT, 1, pParam->bWithLine));
	getItem(jsonItem, "bWithLine")[ITEM_CHILDREN].append(createItemJson("引线颜色", "vLineColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vLineColor))));
	getItem(jsonItem, "bWithLine")[ITEM_CHILDREN].append(createItemJson("引线悬停颜色", "vHoverLineColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vHoverLineColor))));
	getItem(jsonItem, "bWithLine")[ITEM_CHILDREN].append(createItemJson("引线选中颜色", "vCheckedLineColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vCheckedLineColor))));
	getItem(jsonItem, "bWithLine")[ITEM_CHILDREN].append(createItemJson("引线宽度", "fLineWidth", FLOAT_INPUT, 1, pParam->fLineWidth));
	getItem(jsonItem, "bWithLine")[ITEM_CHILDREN].append(createItemJson("是否有引线点", "bLinePoint", BOOL_INPUT, 1, pParam->bLinePoint));
	jsonItem.append(createItemJson("是否有底座", "bWithBase", BOOL_INPUT, 1, pParam->bWithBase));
	Json::Value baseJsonValue;
	convertBoardBaseParam(&pParam->tBaseParam,baseJsonValue);
	getItem(jsonItem, "bWithBase")[ITEM_CHILDREN] = baseJsonValue;
	jsonItem.append(createItemJson("是否深度测试", "bDepthTest", BOOL_INPUT, 1, pParam->bDepthTest));
	jsonItem.append(createItemJson("是否关闭远近平面裁剪", "bDepthClamp", BOOL_INPUT, 1, pParam->bDepthClamp));

	// 次要
	jsonItem.append(createItemJson("是否相对旋转", "bRotateAroundTarget", BOOL_INPUT, 2, pParam->bRotateAroundTarget));
	getItem(jsonItem, "bRotateAroundTarget")[ITEM_CHILDREN].append(createItemJson("相对偏移", "vDistanceToTarget", VECTOR2_INPUT, 2, Vector2ToString(pParam->vDistanceToTarget)));
	jsonItem.append(createItemJson("是否需要透明", "bNeedClarity", BOOL_INPUT, 2, pParam->bNeedClarity));
	jsonItem.append(createItemJson("透明阈值", "fAlphaThreshold", FLOAT_INPUT, 2, pParam->fAlphaThreshold));
	jsonItem.append(createItemJson("透明模式", "eTransparentType", SELECT, 2, pParam->eTransparentType, "0:TRANSPARENT_NONE,1:TRANSPARENT_BLEND,2:TRANSPARENT_AVERAGE"));
	jsonItem.append(createItemJson("左上角边距", "vTopLeftOffset", VECTOR2_INPUT, 1, Vector2ToString(pParam->vTopLeftOffset)));
}

void ParamParseStream::readBoardBaseParam(const std::string strJson, BoardBaseParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(),strJson.c_str() + strJson.length(), &root,&jsError))
	{
		readBoardBaseParam(root, pParam);
	}
}

void ParamParseStream::readBoardBaseParam(Json::Value root, BoardBaseParam &pParam)
{
	try
	{
		pParam.fWidth = atof(getItemValue(root, "fWidth").asCString());
		pParam.fHeight = atof(getItemValue(root, "fHeight").asCString());
		pParam.fLength = atof(getItemValue(root, "fLength").asCString());
		pParam.strImage = getItemValue(root, "strImage").asString();
		pParam.fHeightToBoard = atof(getItemValue(root, "fHeightToBoard").asCString());
		pParam.strModelName = getItemValue(root, "strModelName").asString();
		pParam.bGlowEnable = getItemValue(root, "bGlowEnable").asBool();
		if (!getItemValue(root, "fGlowIntensity").isNull())
		{
			pParam.fGlowIntensity = atof(getItemValue(root, "fGlowIntensity").asCString());
		}
		if (!getItemValue(root, "vModelAngle").isNull())
		{
			pParam.vModelAngle = StringToVector3(getItemValue(root, "vModelAngle").asCString());
		}
		if (!getItemValue(root, "vModelScale").isNull())
		{
			pParam.vModelScale = StringToVector3(getItemValue(root, "vModelScale").asCString());
		}
		pParam.fScale = atof(getItemValue(root, "fScale").asCString());
		pParam.nDirection = (BoardDirect_e)atoi(getItemValue(root, "nDirection").asCString());
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertBoardBaseParam(BoardBaseParam *pParam, Json::Value &jsonItem)
{
	jsonItem.append(createItemJson("宽", "fWidth", FLOAT_INPUT, 1, pParam->fWidth));
	jsonItem.append(createItemJson("高", "fHeight", FLOAT_INPUT, 1, pParam->fHeight));
	jsonItem.append(createItemJson("厚", "fLength", FLOAT_INPUT, 1, pParam->fLength));
	jsonItem.append(createItemJson("常规背景", "strImage", FILE_INPUT, 1, pParam->strImage));
	jsonItem.append(createItemJson("离面板高度", "fHeightToBoard", FLOAT_INPUT, 1, pParam->fHeightToBoard));
	jsonItem.append(createItemJson("模型名字", "strModelName", FILE_INPUT, 1, pParam->strModelName));
	jsonItem.append(createItemJson("模型角度", "vModelAngle", VECTOR3_INPUT, 1, Vector3ToString(Vector3d(pParam->vModelAngle))));
	jsonItem.append(createItemJson("模型缩放","vModelScale", VECTOR3_INPUT, 1, Vector3ToString(Vector3d(pParam->vModelScale))));
	jsonItem.append(createItemJson("是否有辉光", "bGlowEnable", BOOL_INPUT, 1, pParam->bGlowEnable));
	jsonItem.append(createItemJson("辉光强度", "fGlowIntensity", FLOAT_INPUT, 1, pParam->fGlowIntensity));
	jsonItem.append(createItemJson("缩放", "fScale", FLOAT_INPUT, 1, pParam->fScale));
	jsonItem.append(createItemJson("方向类型", "nDirection", SELECT, 1, pParam->nDirection, "0:NormalDirect,1:BillBoard"));
}

void ParamParseStream::readFontBoardParam(const std::string strJson, FontBoardParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readFontBoardParam(root, pParam);
	}
}

void ParamParseStream::readFontBoardParam(Json::Value root, FontBoardParam &pParam)
{
	try
	{
		readBoardParam(root, pParam);
		pParam.bCheckEnable = getItemValue(root, "bCheckEnable").asBool();
		Json::Value textArray = getItem(root, "vecText")[ITEM_CHILDREN];
		std::vector<TextShow> vecText;
		for (int i = 0; i < textArray.size(); i++)
		{
			TextShow stTextShow;
			readTextShow(textArray[i],stTextShow);
			pParam.vecText.push_back(stTextShow);
		}
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertFontBoardParam(FontBoardParam *pParam, Json::Value &jsonItem)
{
	convertBoardParam(pParam,jsonItem);
	eParam_ParseType eType = ADD_INPUT;
	if (pParam->vecText.size() > 0)
	{
		eType = FIX_COUNT_INPUT;
	}
	jsonItem.append(createItemJson("文字", "vecText", eType, 2, ""));
	for (int i = 0; i < pParam->vecText.size(); i++)
	{
		Json::Value item;
		convertTextShow(&pParam->vecText[i],item);

		getItem(jsonItem, "vecText")[ITEM_CHILDREN].append(item);
	}
}

void ParamParseStream::readVideoBoardParam(const std::string strJson, VideoBoardParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readVideoBoardParam(root, pParam);
	}
}

void ParamParseStream::readVideoBoardParam(Json::Value root, VideoBoardParam &pParam)
{
	readBoardParam(root,pParam);
}

void ParamParseStream::convertVideoBoardParam(VideoBoardParam *pParam, Json::Value &jsonItem)
{
	convertBoardParam(pParam,jsonItem);
}

void ParamParseStream::readButtonBoardParam(const std::string strJson, ButtonBoardParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readButtonBoardParam(root, pParam);
	}
}

void ParamParseStream::readButtonBoardParam(Json::Value root, ButtonBoardParam &pParam)
{
	try
	{
		readFontBoardParam(root, pParam);
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertButtonBoardParam(ButtonBoardParam *pParam, Json::Value &jsonItem)
{
	convertFontBoardParam(pParam,jsonItem);
}

void ParamParseStream::readFontBoardWithButtonParam(const std::string strJson, FontBoardWithButtonParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readFontBoardWithButtonParam(root, pParam);
	}
}

void ParamParseStream::readFontBoardWithButtonParam(Json::Value root, FontBoardWithButtonParam &pParam)
{
	try
	{
		readFontBoardParam(root, pParam);
		pParam.bCheckEnable = getItemValue(root, "bCheckEnable").asBool();
		Json::Value childArray = getItem(root, "vecBoardParam")[ITEM_CHILDREN];
		std::string str = childArray.toStyledString();
		ButtonBoardParam stParam;
		for (int i = 0; i < childArray.size(); i++)
		{
			readButtonBoardParam(childArray[i], stParam);
			stParam.bDynamicScaleEnable = pParam.bDynamicScaleEnable;
			stParam.fDynamicMaxScale = pParam.fDynamicMaxScale;
			stParam.fDynamicMinScale = pParam.fDynamicMinScale;
			stParam.fDynamicScale = pParam.fDynamicScale;
			stParam.fScale = pParam.fScale;
			stParam.bRotateAroundTarget = false;
			pParam.vecBoardParam.push_back(stParam);
		}
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertFontBoardWithButtonParam(FontBoardWithButtonParam *pParam, Json::Value &jsonItem)
{
	convertFontBoardParam(pParam,jsonItem);
	eParam_ParseType eType = ADD_INPUT;
	if (pParam->vecBoardParam.size() > 0)
	{
		eType = FIX_COUNT_INPUT;
	}
	jsonItem.append(createItemJson("按钮集", "vecBoardParam", eType, 2, ""));
	for (int i = 0; i < pParam->vecBoardParam.size(); i++)
	{
		Json::Value item;
		convertButtonBoardParam(&(pParam->vecBoardParam[i]), item);
		getItem(jsonItem, "vecBoardParam")[ITEM_CHILDREN].append(item);
	}
}

//void ParamParseStream::readChildVideoBoardParam(const string strJson, ChildVideoBoardParam &pParam)
//{
//	Json::Reader reader(Json::Features::strictMode());
//	Json::Value root;
//	if (reader.parse(strJson.c_str(), root))
//	{
//		readChildVideoBoardParam(root, pParam);
//	}
//}
//
//void ParamParseStream::readChildVideoBoardParam(Json::Value root, ChildVideoBoardParam &pParam)
//{
//	try
//	{
//		readVideoBoardParam(root, pParam);
//		StringToVector2(getItemValue(root, "vTopLeftOffset").asString(), pParam.vTopLeftOffset);
//		pParam.strID = getItemValue(root, "strID").asString();
//	}
//	catch (const std::exception&)
//	{
//
//	}
//}
//
//void ParamParseStream::convertChildVideoBoardParam(ChildVideoBoardParam *pParam, Json::Value &jsonItem)
//{
//	convertVideoBoardParam(pParam,jsonItem);
//	jsonItem.append(createItemJson("左上角边距", "vTopLeftOffset", VECTOR2_INPUT, 1, Vector2ToString(pParam->vTopLeftOffset)));
//	jsonItem.append(createItemJson("视频id", "strID", STRING_INPUT, 1, pParam->strID));
//}

void ParamParseStream::readVideoBoardWithButtonParam(const std::string strJson, VideoBoardWithButtonParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readVideoBoardWithButtonParam(root, pParam);
	}
}

void ParamParseStream::readVideoBoardWithButtonParam(Json::Value root, VideoBoardWithButtonParam &pParam)
{
	try
	{
		readFontBoardParam(root, pParam);
		//按钮
		{
			Json::Value childArray = getItem(root, "vecBoardParam")[ITEM_CHILDREN];				
			for (int i = 0; i < childArray.size(); i++)
			{	
				ButtonBoardParam stParam;
				readButtonBoardParam(childArray[i], stParam);
				stParam.bDynamicScaleEnable = pParam.bDynamicScaleEnable;
				stParam.fDynamicMaxScale = pParam.fDynamicMaxScale;
				stParam.fDynamicMinScale = pParam.fDynamicMinScale;
				stParam.fDynamicScale = pParam.fDynamicScale;
				stParam.fScale = pParam.fScale;
				stParam.bRotateAroundTarget = false;
				pParam.vecBoardParam.push_back(stParam);
			}
		}
		//视频
		{
			Json::Value childArray = getItem(root, "vecVideoParam")[ITEM_CHILDREN];		
			for (int i = 0; i < childArray.size(); i++)
			{
				VideoBoardParam stParam;
				readVideoBoardParam(childArray[i], stParam);
				stParam.bDynamicScaleEnable = pParam.bDynamicScaleEnable;
				stParam.fDynamicMaxScale = pParam.fDynamicMaxScale;
				stParam.fDynamicMinScale = pParam.fDynamicMinScale;
				stParam.fDynamicScale = pParam.fDynamicScale;
				stParam.fScale = pParam.fScale;
				stParam.bRotateAroundTarget = false;
				pParam.vecVideoParam.push_back(stParam);
			}
		}
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertVideoBoardWithButtonParam(VideoBoardWithButtonParam *pParam, Json::Value &jsonItem)
{
	convertFontBoardParam(pParam,jsonItem);
	//按钮
	{
		eParam_ParseType eType = ADD_INPUT;
		if (pParam->vecBoardParam.size() > 0)
		{
			eType = FIX_COUNT_INPUT;
		}
		jsonItem.append(createItemJson("按钮集", "vecBoardParam", eType, 2, ""));
		for (int i = 0; i < pParam->vecBoardParam.size(); i++)
		{
			Json::Value item;
			convertButtonBoardParam(&(pParam->vecBoardParam[i]), item);
			getItem(jsonItem, "vecBoardParam")[ITEM_CHILDREN].append(item);
		}
	}
	
	//视频
	{
		eParam_ParseType eType = ADD_INPUT;
		if (pParam->vecVideoParam.size() > 0)
		{
			eType = FIX_COUNT_INPUT;
		}
		jsonItem.append(createItemJson("视频集", "vecVideoParam", eType, 2, ""));
		for (int i = 0; i < pParam->vecVideoParam.size(); i++)
		{
			Json::Value item;
			convertVideoBoardParam(&(pParam->vecVideoParam[i]), item);
			getItem(jsonItem, "vecVideoParam")[ITEM_CHILDREN].append(item);
		}
	}
}

void ParamParseStream::readBoardWithFontBoardParam(const std::string strJson, BoardWithFontBoardParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readBoardWithFontBoardParam(root, pParam);
	}
}

void ParamParseStream::readBoardWithFontBoardParam(Json::Value root, BoardWithFontBoardParam &pParam)
{
	try
	{
		readBoardParam(root, pParam);
		pParam.nFontLocation = (BoardWithFontBoardParam::BoardWithFontBoardLocation)atoi(getItemValue(root,
			"nFontLocation").asCString());
		pParam.fFontBoardOffset = atof(getItemValue(root, "fFontBoardOffset").asCString());
		readFontBoardParam(getItem(root, "tFontBoardParam")[ITEM_CHILDREN], pParam.tFontBoardParam);
		pParam.tFontBoardParam.direction = pParam.direction;
		pParam.tFontBoardParam.bDynamicScaleEnable = pParam.bDynamicScaleEnable;
		pParam.tFontBoardParam.fDynamicMaxScale = pParam.fDynamicMaxScale;
		pParam.tFontBoardParam.fDynamicMinScale = pParam.fDynamicMinScale;
		pParam.tFontBoardParam.fDynamicScale = pParam.fDynamicScale;
		pParam.tFontBoardParam.fScale = pParam.fScale;
		pParam.bDetailEnable = getItemValue(root, "bDetailEnable").asBool();
		pParam.nDetailFontLocation = (BoardWithFontBoardParam::BoardWithFontBoardLocation)atoi(getItemValue(getItem(root, "bDetailEnable")[ITEM_CHILDREN],
			"nDetailFontLocation").asCString());
		pParam.fDetailFontBoardOffset = atof(getItemValue(getItem(root, "bDetailEnable")[ITEM_CHILDREN], "fDetailFontBoardOffset").asCString());
		readFontBoardParam(getItem(root, "bDetailEnable")[ITEM_CHILDREN], pParam.tDetailFontBoardParam);
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertBoardWithFontBoardParam(BoardWithFontBoardParam *pParam, Json::Value &jsonItem)
{
	convertBoardParam(pParam,jsonItem);

	jsonItem.append(createItemJson("本体相对位置", "nFontLocation", SELECT, 1, pParam->nFontLocation, "0:TOP,1:BOTTOM,2:LEFT,3:RIGHT,4:FRONT,5:BACK,6:CENTER"));
	jsonItem.append(createItemJson("本体偏移量", "fFontBoardOffset", FLOAT_INPUT, 1, pParam->fFontBoardOffset));
	jsonItem.append(createItemJson("文字结构", "tFontBoardParam", GROUP_INPUT, 1, ""));
	convertFontBoardParam(&(pParam->tFontBoardParam), getItem(jsonItem, "tFontBoardParam")[ITEM_CHILDREN]);
	jsonItem.append(createItemJson("是否有详情面板", "bDetailEnable", BOOL_INPUT, 1, pParam->bDetailEnable));
	getItem(jsonItem,"bDetailEnable")[ITEM_CHILDREN].append(createItemJson("详情面板位置", "nDetailFontLocation", INT_INPUT, 1, pParam->nDetailFontLocation));
	getItem(jsonItem, "bDetailEnable")[ITEM_CHILDREN].append(createItemJson("详情面板偏移量", "fDetailFontBoardOffset", FLOAT_INPUT, 1, pParam->fDetailFontBoardOffset));
	convertFontBoardParam(&(pParam->tDetailFontBoardParam), getItem(jsonItem, "bDetailEnable")[ITEM_CHILDREN]);
}

void ParamParseStream::readSpotLightBoardParam(const std::string strJson, SpotLightBoardParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readSpotLightBoardParam(root, pParam);
	}
}

void ParamParseStream::readSpotLightBoardParam(Json::Value root, SpotLightBoardParam &pParam)
{
	try
	{
		readBoardParam(root, pParam);

		pParam.fOriginHeight = atof(getItemValue(root, "fOriginHeight").asCString());
		pParam.vDirection = StringToVector3(getItemValue(root, "vDirection").asCString());
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertSpotLightBoardParam(SpotLightBoardParam *pParam, Json::Value &jsonItem)
{
	convertBoardParam(pParam,jsonItem);

	jsonItem.append(createItemJson("光线长度", "fOriginHeight", FLOAT_INPUT, 1, pParam->fOriginHeight));
	jsonItem.append(createItemJson("初始方向", "vDirection", VECTOR3_INPUT, 1, Vector3ToString(Vector3d(pParam->vDirection))));
}

void ParamParseStream::readLightBeamParam(const std::string strJson, LightBeamParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readLightBeamParam(root, pParam);
	}
}

void ParamParseStream::readLightBeamParam(Json::Value root, LightBeamParam &pParam)
{
	try
	{
		readBoardParam(root, pParam);

		pParam.bAnimationGrow = getItemValue(root, "bAnimationGrow").asBool();
		pParam.fFontBoardOffset = atof(getItemValue(root, "fFontBoardOffset").asCString());
		pParam.nFontLocation = (LightBeamParam::LightBeamFontBoardLocation)atoi(getItemValue(root, "nFontLocation").asCString());
		readFontBoardParam(getItem(root, "tFontBoardParam")[ITEM_CHILDREN], pParam.tFontBoardParam);
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertLightBeamParam(LightBeamParam *pParam, Json::Value &jsonItem)
{
	convertBoardParam(pParam, jsonItem);

	jsonItem.append(createItemJson("是否有动画", "bAnimationGrow", BOOL_INPUT, 1, pParam->bAnimationGrow));
	jsonItem.append(createItemJson("偏移量", "fFontBoardOffset", FLOAT_INPUT, 1, pParam->fFontBoardOffset));
	jsonItem.append(createItemJson("面板方向", "nFontLocation", INT_INPUT, 1, pParam->nFontLocation));
	jsonItem.append(createItemJson("面板参数", "tFontBoardParam", GROUP_INPUT, 1, ""));
	convertFontBoardParam(&(pParam->tFontBoardParam),getItem(jsonItem, "tFontBoardParam")[ITEM_CHILDREN]);
}

void ParamParseStream::readPolygonParam(const std::string strJson, PolygonParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readPolygonParam(root, pParam);
	}
}

void ParamParseStream::readPolygonParam(Json::Value root, PolygonParam &pParam)
{
	try
	{
		pParam.bWithLine = true;
		readNodeParamBase(root, pParam);
		pParam.strBaseImage = getItemValue(root, "strImage").asString();
		pParam.vBaseColor = StringToVector4(getItemValue(root, "vColor").asCString());
		pParam.vBaseColor = ConvertColorFormat255(pParam.vBaseColor);
		if (!getItemValue(root, "vSelectColor").isNull())
		{
			pParam.vSelectColor = StringToVector4(getItemValue(root, "vSelectColor").asCString());
			pParam.vSelectColor = ConvertColorFormat255(pParam.vSelectColor);
		}
		pParam.bClickEnable = getItemValue(root, "bClickEnable").asBool();
		if (!getItemValue(root, "bWithLine").isNull())
		{
			pParam.bWithLine = getItemValue(root, "bWithLine").asBool();
		}
		pParam.fLineWidth = atof(getItemValue(root, "fLineWidth").asCString());
		pParam.vLineColor = StringToVector4(getItemValue(root, "vLineColor").asCString());
		pParam.vLineColor = ConvertColorFormat255(pParam.vLineColor);
		if (!getItemValue(root, "bDepthTest").isNull())
		{
			pParam.bDepthTest = getItemValue(root, "bDepthTest").asBool();
		}
		if (!getItemValue(root, "bWithFence").isNull())
		{
			pParam.bWithFence = getItemValue(root, "bWithFence").asBool();
		}
		if (!getItemValue(root, "fFenceHeight").isNull())
		{
			pParam.fFenceHeight = atof(getItemValue(root, "fFenceHeight").asCString());
		}
		if (!getItemValue(root, "strFenceImage").isNull())
		{
			pParam.strFenceImage = getItemValue(root, "strFenceImage").asString();
		}
		if (!getItemValue(root, "vFenceColor").isNull())
		{
			pParam.vFenceColor = StringToVector4(getItemValue(root, "vFenceColor").asCString());
			pParam.vFenceColor = ConvertColorFormat255(pParam.vFenceColor);
		}
		if (!getItemValue(root, "bHoverEnable").isNull())
		{
			pParam.bHoverEnable = getItemValue(root, "bHoverEnable").asBool();
			pParam.strHoverBaseImage = getItemValue(getItem(root, "bHoverEnable")[ITEM_CHILDREN], "strHoverBaseImage").asString();
			pParam.strHoverFenceImage = getItemValue(getItem(root, "bHoverEnable")[ITEM_CHILDREN], "strHoverFenceImage").asString();
			pParam.vHoverColor = StringToVector4(getItemValue(getItem(root, "bHoverEnable")[ITEM_CHILDREN], "vHoverColor").asCString());
			pParam.vHoverColor = ConvertColorFormat255(pParam.vHoverColor);
			pParam.vHoverFenceColor = StringToVector4(getItemValue(getItem(root, "bHoverEnable")[ITEM_CHILDREN], "vHoverFenceColor").asCString());
			pParam.vHoverFenceColor = ConvertColorFormat255(pParam.vHoverFenceColor);
			pParam.vHoverLineColor = StringToVector4(getItemValue(getItem(root, "bHoverEnable")[ITEM_CHILDREN], "vHoverLineColor").asCString());
			pParam.vHoverLineColor = ConvertColorFormat255(pParam.vHoverLineColor);
		}
		if (!getItemValue(root, "vHoverColor").isNull())
		{
			pParam.vHoverColor = StringToVector4(getItemValue(root, "vHoverColor").asCString());
			pParam.vHoverColor = ConvertColorFormat255(pParam.vHoverColor);
		}
		if (!getItemValue(root, "fUVRepeat").isNull())
		{
			pParam.fUVRepeat = atof(getItemValue(root, "fUVRepeat").asCString());
		}
		if (!getItemValue(root, "bBaseGlowEnable").isNull())
		{
			pParam.bBaseGlowEnable = getItemValue(root, "bBaseGlowEnable").asBool();
		}
		if (!getItemValue(root, "fBaseGlowIntensity").isNull())
		{
			pParam.fBaseGlowIntensity = atof(getItemValue(root, "fBaseGlowIntensity").asCString());
		}
		if (!getItemValue(root, "bFenceGlowEnable").isNull())
		{
			pParam.bFenceGlowEnable = getItemValue(root, "bFenceGlowEnable").asBool();
		}
		if (!getItemValue(root, "fFenceGlowIntensity").isNull())
		{
			pParam.fFenceGlowIntensity = atof(getItemValue(root, "fFenceGlowIntensity").asCString());
		}
		if (getItemValue(root, "bCircle").isBool())
		{
			pParam.bCircle = getItemValue(root, "bCircle").asBool();
		}
		if (pParam.vBaseColor.a > 0 || (pParam.bHoverEnable && pParam.vHoverColor.a > 0) || !pParam.strBaseImage.empty() || (pParam.bHoverEnable && !pParam.strHoverBaseImage.empty()))
		{
			pParam.bCalculateCenter = true;
		}
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertPolygonParam(PolygonParam *pParam, Json::Value &jsonItem)
{
	convertNodeParamBase(pParam, jsonItem);
	jsonItem.append(createItemJson("底面图片", "strImage", FILE_INPUT, 1, pParam->strBaseImage));
	jsonItem.append(createItemJson("是否可悬浮", "bHoverEnable", BOOL_INPUT, 1, pParam->bHoverEnable));
	getItem(jsonItem, "bHoverEnable")[ITEM_CHILDREN].append(createItemJson("悬停底面背景", "strHoverBaseImage", FILE_INPUT, 1, pParam->strHoverBaseImage));
	getItem(jsonItem, "bHoverEnable")[ITEM_CHILDREN].append(createItemJson("悬停围栏背景", "strHoverFenceImage", FILE_INPUT, 1, pParam->strHoverFenceImage));
	getItem(jsonItem, "bHoverEnable")[ITEM_CHILDREN].append(createItemJson("悬停底面颜色", "vHoverColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vHoverColor))));
	getItem(jsonItem, "bHoverEnable")[ITEM_CHILDREN].append(createItemJson("悬停围栏颜色", "vHoverFenceColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vHoverFenceColor))));
	getItem(jsonItem, "bHoverEnable")[ITEM_CHILDREN].append(createItemJson("悬停边线颜色", "vHoverLineColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vHoverLineColor))));
	jsonItem.append(createItemJson("底面颜色(有图片优先使用图片)", "vColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vBaseColor))));
	jsonItem.append(createItemJson("是否可点击", "bClickEnable", BOOL_INPUT, 1, pParam->bClickEnable));
	jsonItem.append(createItemJson("底面是否有辉光", "bBaseGlowEnable", BOOL_INPUT, 1, pParam->bBaseGlowEnable));
	jsonItem.append(createItemJson("底面辉光强度", "fBaseGlowIntensity", FLOAT_INPUT, 1, pParam->fBaseGlowIntensity));
	jsonItem.append(createItemJson("是否带边框线", "bWithLine", BOOL_INPUT, 1, pParam->bWithLine));
	jsonItem.append(createItemJson("边线宽度", "fLineWidth", FLOAT_INPUT, 1, pParam->fLineWidth));
	jsonItem.append(createItemJson("边线颜色", "vLineColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vLineColor))));
	jsonItem.append(createItemJson("是否开启深度测试", "bDepthTest", BOOL_INPUT, 1, pParam->bDepthTest));
	jsonItem.append(createItemJson("是否带围栏", "bWithFence", BOOL_INPUT, 1, pParam->bWithFence));
	jsonItem.append(createItemJson("围栏高度", "fFenceHeight", FLOAT_INPUT, 1, pParam->fFenceHeight));
	jsonItem.append(createItemJson("围栏贴图", "strFenceImage", FILE_INPUT, 1, pParam->strFenceImage));
	jsonItem.append(createItemJson("围栏颜色(有图片优先使用图片)", "vFenceColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vFenceColor))));
	jsonItem.append(createItemJson("围栏是否有辉光", "bFenceGlowEnable", BOOL_INPUT, 1, pParam->bFenceGlowEnable));
	jsonItem.append(createItemJson("围栏辉光强度", "fFenceGlowIntensity", FLOAT_INPUT, 1, pParam->fFenceGlowIntensity));
	jsonItem.append(createItemJson("图片重复范围", "fUVRepeat", FLOAT_INPUT, 1, pParam->fUVRepeat));
	jsonItem.append(createItemJson("是否圆形", "bCircle", BOOL_INPUT, 1, pParam->bCircle));
}

void ParamParseStream::readRoadParam(const std::string strJson, RoadParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readRoadParam(root, pParam);
	}
}

void ParamParseStream::readRoadParam(Json::Value root, RoadParam &pParam)
{
	try
	{
		readNodeParamBase(root, pParam);
		if (pParam.nVersion == 1)
		{
			readRoadParamV1(root, pParam);
		}
		else
		{
			pParam.strImage = getItemValue(root, "strImage").asString();
			if (!getItemValue(root, "fStep").isNull())
			{
				pParam.vecStep.push_back(atof(getItemValue(root, "fStep").asCString()));
			}
			pParam.vecWidth.push_back(atof(getItemValue(root, "fWidth").asCString()));
			if (!getItemValue(root, "nCalcHeightType").isNull())
			{
				pParam.nCalcHeightType = atoi(getItemValue(root, "nCalcHeightType").asCString());
			}
			if (!getItemValue(root, "fFixedHeight").isNull())
			{
				pParam.fFixedHeight = atof(getItemValue(root, "fFixedHeight").asCString());
			}
			pParam.strHeightFilePath = getItemValue(root, "strHeightFilePath").asString();
			if (!getItemValue(root, "fAdjustHeight").isNull())
			{
				pParam.fAdjustHeight = atof(getItemValue(root, "fAdjustHeight").asCString());
			}
			pParam.strID = getItemValue(root, "strRoadID").asString();
			pParam.bAnimation = getItemValue(root, "bAnimation").asBool();
			pParam.fAnimationSpeed = atof(getItemValue(root, "fAnimationSpeed").asCString());
			//pParam.strRoadFilePath = getItemValue(root, "strRoadFilePath").asString();
			if (!getItemValue(root, "bDepthTest").isNull())
			{
				pParam.bDepthTest = getItemValue(root, "bDepthTest").asBool();
			}
			//if (StartWith(pParam.strRoadFilePath, APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDirName))
			//{
			//	//添加绝对路径
			//	pParam.strRoadFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + string("/") 
			//		+ pParam.strRoadFilePath;
			//}
			if (StartWith(pParam.strHeightFilePath, APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDirName))
			{
				//添加绝对路径
				pParam.strHeightFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + std::string("/")
					+ pParam.strHeightFilePath;
			}
			pParam.bGlow = getItemValue(root, "bGlow").asBool();
			if (!getItemValue(root, "fGlowIntensity").isNull())
			{
				pParam.fGlowIntensity = atof(getItemValue(root, "fGlowIntensity").asCString());
			}
			if (!getItemValue(root, "bGlowFlash").isNull())
			{
				pParam.bGlowFlash = getItemValue(root, "bGlowFlash").asBool();
			}
			if (!getItemValue(root, "fGlowFlashSpeed").isNull())
			{
				pParam.fGlowFlashSpeed = atof(getItemValue(root, "fGlowFlashSpeed").asCString());
			}
			if (!getItemValue(root, "bIntersectEnable").isNull())
			{
				pParam.bIntersectEnable = getItemValue(root, "bIntersectEnable").asBool();
			}
		}
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertRoadParam(RoadParam *pParam, Json::Value &jsonItem)
{
	convertNodeParamBase(pParam, jsonItem);
	if (pParam->nVersion == 1)
	{
		convertRoadParamV1(pParam, jsonItem);
	}
	else
	{
		jsonItem.append(createItemJson("道路贴图", "strImage", FILE_INPUT, 1, pParam->strImage));
		float fWidth = 5.0f;
		if (pParam->vecWidth.size() != 0)
		{
			fWidth = pParam->vecWidth[0];
		}
		jsonItem.append(createItemJson("道路宽度", "fWidth", FLOAT_INPUT, 1, fWidth));
		float fStep = 5.0f;
		if (pParam->vecStep.size() != 0)
		{
			fStep = pParam->vecStep[0];
		}
		jsonItem.append(createItemJson("道路步长", "fStep", FLOAT_INPUT, 1, fStep));
		jsonItem.append(createItemJson("计算高度类型", "nCalcHeightType", SELECT, 1, pParam->nCalcHeightType, "0:FIXED_HEIGHT,1:CALC_WITH_MAP,2:COLL_WITH_NODE"));
		jsonItem.append(createItemJson("固定高度", "fFixedHeight", FLOAT_INPUT, 1, pParam->fFixedHeight));
		jsonItem.append(createItemJson("场景高度图路径", "strHeightFilePath", STRING_INPUT, 1, pParam->strHeightFilePath));
		jsonItem.append(createItemJson("高度计算后的调整", "fAdjustHeight", FLOAT_INPUT, 1, pParam->fAdjustHeight));
		jsonItem.append(createItemJson("贴图是否流动", "bAnimation", BOOL_INPUT, 1, pParam->bAnimation));
		jsonItem.append(createItemJson("贴图流动速度", "fAnimationSpeed", FLOAT_INPUT, 1, pParam->fAnimationSpeed));
		//jsonItem.append(createItemJson("模型路径", "strRoadFilePath", FILE_INPUT, 1, pParam->strRoadFilePath));
		jsonItem.append(createItemJson("是否开启深度测试", "bDepthTest", BOOL_INPUT, 1, pParam->bDepthTest));
		jsonItem.append(createItemJson("是否开启辉光", "bGlow", BOOL_INPUT, 1, pParam->bGlow));
		jsonItem.append(createItemJson("辉光强度", "fGlowIntensity", FLOAT_INPUT, 1, pParam->fGlowIntensity));
		jsonItem.append(createItemJson("是否辉光闪烁", "bGlowFlash", BOOL_INPUT, 1, pParam->bGlowFlash));
		jsonItem.append(createItemJson("辉光闪烁速度", "fGlowFlashSpeed", FLOAT_INPUT, 1, pParam->fGlowFlashSpeed));
		jsonItem.append(createItemJson("是否计算相交", "bIntersectEnable", BOOL_INPUT, 1, pParam->bIntersectEnable));
	}
}

void ParamParseStream::readRoadParamV1(Json::Value root, RoadParam& pParam)
{
	try
	{
		pParam.vecStep.push_back(atof(getItemValue(root, "fStep").asCString()));
		pParam.vecWidth.push_back(atof(getItemValue(root, "fWidth").asCString()));
		pParam.nCalcHeightType = atoi(getItemValue(root, "nCalcHeightType").asCString());
		pParam.fFixedHeight = atof(getItemValue(root, "fFixedHeight").asCString());
		pParam.strHeightFilePath = getItemValue(root, "strHeightFilePath").asString();
		pParam.fAdjustHeight = atof(getItemValue(root, "fAdjustHeight").asCString());
		pParam.strID = getItemValue(root, "strRoadID").asString();
		pParam.bDepthTest = getItemValue(root, "bDepthTest").asBool();
		pParam.eType = (RoadType)atoi(getItemValue(root, "eType").asCString());
		pParam.nNumArc = atoi(getItemValue(root, "nNumArc").asCString());
		if (StartWith(pParam.strHeightFilePath, APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strResourceDirName))
		{
			//添加绝对路径
			pParam.strHeightFilePath = APIProvider::GetSystemAPI()->iEngineAPI->iGetProjectConfig().sPathConfig.strDataDir + std::string("/")
				+ pParam.strHeightFilePath;
		}
		pParam.bGlow = getItemValue(root, "bGlow").asBool();
		{
			Json::Value& jAttr = getItem(root, "bGlow")[ITEM_CHILDREN];
			pParam.fGlowIntensity = atof(getItemValue(jAttr, "fGlowIntensity").asCString());
			pParam.bGlowFlash = getItemValue(jAttr, "bGlowFlash").asBool();
			pParam.fGlowFlashSpeed = atof(getItemValue(jAttr, "fGlowFlashSpeed").asCString());
			pParam.bIntersectEnable = getItemValue(jAttr, "bIntersectEnable").asBool();
		}

		//归类
		{
			Json::Value& jAttr = getItem(root, "attrs_image")[ITEM_CHILDREN];
			pParam.strImage = getItemValue(jAttr, "strImage").asString();
			pParam.bAnimation = getItemValue(jAttr, "bAnimation").asBool();
			pParam.fAnimationSpeed = atof(getItemValue(jAttr, "fAnimationSpeed").asCString());
		}
		{
			Json::Value& jAttr = getItem(root, "attrs_color")[ITEM_CHILDREN];
			pParam.vLineColor = StringToVector4(getItemValue(jAttr, "vLineColor").asCString());
			pParam.vLineColor = ConvertColorFormat255(pParam.vLineColor);
			pParam.fLineWidth = atof(getItemValue(jAttr, "fLineWidth").asCString());
			pParam.vPointColor = StringToVector4(getItemValue(jAttr, "vPointColor").asCString());
			pParam.vPointColor = ConvertColorFormat255(pParam.vPointColor);
			pParam.fPointSize = atof(getItemValue(jAttr, "fPointSize").asCString());
		}	
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertRoadParamV1(RoadParam* pParam, Json::Value& jsonItem)
{
	jsonItem.append(createItemJson("道路类型", "eType", SELECT, 1, pParam->eType, "0:贴图,1:颜色实现,2:颜色虚线"));
	float fWidth = 5.0f;
	if (pParam->vecWidth.size() != 0)
	{
		fWidth = pParam->vecWidth[0];
	}
	jsonItem.append(createItemJson("道路宽度", "fWidth", FLOAT_INPUT, 1, fWidth));
	float fStep = 5.0f;
	if (pParam->vecStep.size() != 0)
	{
		fStep = pParam->vecStep[0];
	}
	jsonItem.append(createItemJson("道路步长", "fStep", FLOAT_INPUT, 1, fStep));
	jsonItem.append(createItemJson("计算高度类型", "nCalcHeightType", SELECT, 1, pParam->nCalcHeightType, "0:FIXED_HEIGHT,1:CALC_WITH_MAP,2:COLL_WITH_NODE"));
	jsonItem.append(createItemJson("固定高度", "fFixedHeight", FLOAT_INPUT, 1, pParam->fFixedHeight));
	jsonItem.append(createItemJson("场景高度图路径", "strHeightFilePath", STRING_INPUT, 1, pParam->strHeightFilePath));
	jsonItem.append(createItemJson("高度计算后的调整", "fAdjustHeight", FLOAT_INPUT, 1, pParam->fAdjustHeight));
	jsonItem.append(createItemJson("是否开启深度测试", "bDepthTest", BOOL_INPUT, 1, pParam->bDepthTest));
	jsonItem.append(createItemJson("是否开启辉光", "bGlow", BOOL_INPUT, 1, pParam->bGlow));
	jsonItem.append(createItemJson("道路拐角半径", "nNumArc", INT_INPUT, 1, pParam->nNumArc));
	{
		Json::Value& jAttrs = getItem(jsonItem, "bGlow")[ITEM_CHILDREN];
		jAttrs.append(createItemJson("辉光强度", "fGlowIntensity", FLOAT_INPUT, 1, pParam->fGlowIntensity));
		jAttrs.append(createItemJson("是否辉光闪烁", "bGlowFlash", BOOL_INPUT, 1, pParam->bGlowFlash));
		jAttrs.append(createItemJson("辉光闪烁速度", "fGlowFlashSpeed", FLOAT_INPUT, 1, pParam->fGlowFlashSpeed));
		jAttrs.append(createItemJson("是否计算相交", "bIntersectEnable", BOOL_INPUT, 1, pParam->bIntersectEnable));
	}
	//归类
	//贴图类型
	{
		jsonItem.append(createItemJson("贴图类型参数", "attrs_image", BOOL_INPUT, 1, true));
		Json::Value& jAttrs_Image = getItem(jsonItem, "attrs_image")[ITEM_CHILDREN];
		jAttrs_Image.append(createItemJson("道路贴图", "strImage", FILE_INPUT, 1, pParam->strImage));
		jAttrs_Image.append(createItemJson("贴图是否流动", "bAnimation", BOOL_INPUT, 1, pParam->bAnimation));
		jAttrs_Image.append(createItemJson("贴图流动速度", "fAnimationSpeed", FLOAT_INPUT, 1, pParam->fAnimationSpeed));
	}
	//颜色类型
	{
		jsonItem.append(createItemJson("颜色类型参数", "attrs_color", BOOL_INPUT, 1, false));
		Json::Value& jAttrs_Image = getItem(jsonItem, "attrs_color")[ITEM_CHILDREN];
		jAttrs_Image.append(createItemJson("道路颜色", "vLineColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vLineColor))));
		jAttrs_Image.append(createItemJson("道路宽度", "fLineWidth", FLOAT_INPUT, 1, pParam->fLineWidth));
		jAttrs_Image.append(createItemJson("道路插入点颜色", "vPointColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->vPointColor))));
		jAttrs_Image.append(createItemJson("道路插入点大小", "fPointSize", FLOAT_INPUT, 1, pParam->fPointSize));
	}
}

void ParamParseStream::readBoardInstanceParam(const std::string strJson, BoardInstanceParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readBoardInstanceParam(root, pParam);
	}
}

void ParamParseStream::readBoardInstanceParam(Json::Value root, BoardInstanceParam &pParam)
{
	try
	{
		readBoardParam(root,pParam);
		pParam.bHoverEnable = false;	//true的话悬浮会有外框，所以改为false
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertBoardInstanceParam(BoardInstanceParam *pParam, Json::Value &jsonItem)
{
	convertBoardParam(pParam,jsonItem);
}

void ParamParseStream::readLocusLineParam(const std::string strJson, LocusLineParam &pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readLocusLineParam(root, pParam);
	}
}

void ParamParseStream::readLocusLineParam(Json::Value root, LocusLineParam &pParam)
{
	try
	{
		if (!getItemValue(root, "fLineWidth").isNull())
		{
			pParam.stDefaultSegment.fWidth = atof(getItemValue(root, "fLineWidth").asCString());
		}
		if (!getItemValue(root, "nLineStyle").isNull())
		{
			pParam.stDefaultSegment.nLineStyle = atoi(getItemValue(root, "nLineStyle").asCString());
		}
		if (!getItemValue(root, "nDottedFactor").isNull())
		{
			pParam.stDefaultSegment.nDottedFactor = atoi(getItemValue(root, "nDottedFactor").asCString());
		}
		if (!getItemValue(root, "usDottedStyle").isNull())
		{
			pParam.stDefaultSegment.usDottedStyle = atoi(getItemValue(root, "usDottedStyle").asCString());
		}
		if (!getItemValue(root, "vLineColor").isNull())
		{
			pParam.stDefaultSegment.vColor = StringToVector4(getItemValue(root, "vLineColor").asCString());
			pParam.stDefaultSegment.vColor = ConvertColorFormat255(pParam.stDefaultSegment.vColor);
		}
		if (!getItemValue(root, "bDepthTest").isNull())
		{
			pParam.bDepthTest = getItemValue(root, "bDepthTest").asBool();
		}
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertLocusLineParam(LocusLineParam *pParam, Json::Value &jsonItem)
{
	jsonItem.append(createItemJson("线宽", "fLineWidth", FLOAT_INPUT, 1, pParam->stDefaultSegment.fWidth));
	jsonItem.append(createItemJson("线的风格", "nLineStyle", INT_INPUT, 1, pParam->stDefaultSegment.nLineStyle));
	jsonItem.append(createItemJson("虚线比率", "nDottedFactor", INT_INPUT, 1, pParam->stDefaultSegment.nDottedFactor));
	jsonItem.append(createItemJson("虚线风格", "usDottedStyle", STRING_INPUT, 1, pParam->stDefaultSegment.nDottedFactor));
	jsonItem.append(createItemJson("线的颜色", "vLineColor", COLOR_INPUT, 1, Vector4ToString(ColorToFormat255(pParam->stDefaultSegment.vColor))));
	jsonItem.append(createItemJson("是否开启深度测试", "bDepthTest", BOOL_INPUT, 1, pParam->bDepthTest));
}

void ParamParseStream::readFlyLineParam(const std::string strJson, FlyLineParam& pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readFlyLineParam(root, pParam);
	}
}

void ParamParseStream::readFlyLineParam(Json::Value root, FlyLineParam& pParam)
{
	try
	{
		pParam.strMaterial = getItemValue(root, "strMaterial").asString();
		pParam.nTubularSegment = atof(getItemValue(root, "nTubularSegment").asCString());
		pParam.nRadialSegment = atof(getItemValue(root, "nRadialSegment").asCString());
		pParam.fRadius = atof(getItemValue(root, "fRadius").asCString());
		pParam.bAnimation = getItemValue(root, "bAnimation").asBool();
		pParam.fAnimationSpeed = atof(getItemValue(root, "fAnimationSpeed").asCString());
		pParam.bGlowEnable = getItemValue(root, "bGlow").asBool();
		pParam.fGlowIntensity = atof(getItemValue(root, "fGlowIntensity").asCString());
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertFlyLineParam(FlyLineParam* pParam, Json::Value& jsonItem)
{
	jsonItem.append(createItemJson("飞线材质", "strMaterial", FILE_INPUT, 1, pParam->strMaterial)); 
	jsonItem.append(createItemJson("沿长度方向分段数", "nTubularSegment", INT_INPUT, 1, pParam->nTubularSegment));
	jsonItem.append(createItemJson("沿圆柱方向分段数", "nRadialSegment", INT_INPUT, 1, pParam->nRadialSegment));
	jsonItem.append(createItemJson("圆柱半径", "fRadius", FLOAT_INPUT, 1, pParam->fRadius));
	jsonItem.append(createItemJson("贴图是否流动", "bAnimation", BOOL_INPUT, 1, pParam->bAnimation));
	jsonItem.append(createItemJson("贴图流动速度", "fAnimationSpeed", FLOAT_INPUT, 1, pParam->fAnimationSpeed));
	jsonItem.append(createItemJson("是否开启辉光", "bGlow", BOOL_INPUT, 1, pParam->bGlowEnable));
	jsonItem.append(createItemJson("辉光强度", "fGlowIntensity", FLOAT_INPUT, 4, pParam->fGlowIntensity));
}

void ParamParseStream::readMultiFlyLineParam(const std::string strJson, MultiFlyLineParam& pParam)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readMultiFlyLineParam(root, pParam);
	}
}

void ParamParseStream::readMultiFlyLineParam(Json::Value root, MultiFlyLineParam& pParam)
{
	try
	{
		pParam.strMaterialFirst = getItemValue(root, "strMaterialFirst").asString();
		pParam.strMaterialSecond = getItemValue(root, "strMaterialSecond").asString();
		pParam.nTubularSegment = atof(getItemValue(root, "nTubularSegment").asCString());
		pParam.nRadialSegment = atof(getItemValue(root, "nRadialSegment").asCString());
		pParam.fRadius = atof(getItemValue(root, "fRadius").asCString());
		pParam.bAnimation = getItemValue(root, "bAnimation").asBool();
		pParam.fAnimationSpeed = atof(getItemValue(root, "fAnimationSpeed").asCString());
		pParam.bAnimation = getItemValue(root, "bSwitchAnimation").asBool();
		pParam.fAnimationSpeed = atof(getItemValue(root, "fSwitchAnimationSpeed").asCString());
		pParam.bGlowEnable = getItemValue(root, "bGlow").asBool();
		pParam.fGlowIntensity = atof(getItemValue(root, "fGlowIntensity").asCString());
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::convertMultiFlyLineParam(MultiFlyLineParam* pParam, Json::Value& jsonItem)
{
	jsonItem.append(createItemJson("飞线第一段材质", "strMaterialFirst", FILE_INPUT, 1, pParam->strMaterialFirst));
	jsonItem.append(createItemJson("飞线第二段材质", "strMaterialSecond", FILE_INPUT, 1, pParam->strMaterialSecond));
	jsonItem.append(createItemJson("沿长度方向分段数", "nTubularSegment", INT_INPUT, 1, pParam->nTubularSegment));
	jsonItem.append(createItemJson("沿圆柱方向分段数", "nRadialSegment", INT_INPUT, 1, pParam->nRadialSegment));
	jsonItem.append(createItemJson("圆柱半径", "fRadius", FLOAT_INPUT, 1, pParam->fRadius));
	jsonItem.append(createItemJson("贴图是否流动", "bAnimation", BOOL_INPUT, 1, pParam->bAnimation));
	jsonItem.append(createItemJson("贴图流动速度", "fAnimationSpeed", FLOAT_INPUT, 1, pParam->fAnimationSpeed));
	jsonItem.append(createItemJson("贴图是否切换", "bSwitchAnimation", BOOL_INPUT, 1, pParam->bSwitchAnimation));
	jsonItem.append(createItemJson("贴图切换速度", "fSwitchAnimationSpeed", FLOAT_INPUT, 1, pParam->fSwitchAnimationSpeed));
	jsonItem.append(createItemJson("是否开启辉光", "bGlow", BOOL_INPUT, 1, pParam->bGlowEnable));
	jsonItem.append(createItemJson("辉光强度", "fGlowIntensity", FLOAT_INPUT, 4, pParam->fGlowIntensity));
}

void ParamParseStream::readTextShow(const std::string strJson, TextShow& textShow)
{
	Json::CharReaderBuilder jsonBuilder;
	std::unique_ptr<Json::CharReader> reader(jsonBuilder.newCharReader());
	Json::Value root;
	std::string jsError;
	if (reader->parse(strJson.c_str(), strJson.c_str() + strJson.length(), &root, &jsError))
	{
		readTextShow(root, textShow);
	}
}

void ParamParseStream::readTextShow(Json::Value root, TextShow& textShow)
{
	char sz[1024] = { 0 };
	const char* pTemp = getItemValue(root, "strText").asCString();
	Utf8ToAscii(pTemp, sizeof(sz), sz);
	textShow.strText = sz;
	textShow.vTextPos = StringToVector2(getItemValue(root, "vTextPos").asString());
	textShow.vTextSize = StringToVector2(getItemValue(root, "vTextSize").asString());
	//textShow.vTextSize.y = textShow.vTextSize.x * TEXT_SIZE_HW_SCALE;
	auto tempVec3 = StringToVector3(getItemValue(root, "vColor").asString());
	textShow.vColor = Vector4(tempVec3.x, tempVec3.y, tempVec3.z, 1.0);
	textShow.vColor = ConvertColorFormat255(textShow.vColor);
	textShow.strID = getItemValue(root, "strID").asString();
	if (!getItemValue(root, "bAdjustWidth").isNull())
	{
		textShow.bAdjustWidth = getItemValue(root, "bAdjustWidth").asBool();
	}
	if (!getItemValue(root, "bWidthCenter").isNull())
	{
		textShow.bWidthCenter = getItemValue(root, "bWidthCenter").asBool();
	}
	if (!getItemValue(root, "bHeightCenter").isNull())
	{
		textShow.bHeightCenter = getItemValue(root, "bHeightCenter").asBool();
	}
	if (!getItemValue(root, "nLineMaxLength").isNull())
	{
		textShow.nLineMaxLength = atoi(getItemValue(root, "nLineMaxLength").asString().c_str());
	}
	if (!getItemValue(root, "nMaxLine").isNull())
	{
		textShow.nMaxLine = atoi(getItemValue(root, "nMaxLine").asString().c_str());
	}
	if (!getItemValue(root, "strDescription").isNull())
	{
		textShow.strDescription = getItemValue(root, "strDescription").asString();
	}
	if (!getItemValue(root, "bCenter").isNull())
	{
		textShow.bWidthCenter = getItemValue(root, "bCenter").asBool();
	}
}

void ParamParseStream::convertTextShow(TextShow* pParam, Json::Value& jsonItem)
{
	if (!pParam)
	{
		return;
	}
	try
	{
		jsonItem.append(createItemJson("位置", "vTextPos", VECTOR2_INPUT, 2, Vector2ToString(pParam->vTextPos)));
		jsonItem.append(createItemJson("内容", "strText", STRING_INPUT, 2, pParam->strText));
		jsonItem.append(createItemJson("大小", "vTextSize", VECTOR2_INPUT, 2, Vector2ToString(pParam->vTextSize)));
		jsonItem.append(createItemJson("颜色", "vColor", COLOR_INPUT, 2, Vector4ToString(ColorToFormat255(pParam->vColor))));
		jsonItem.append(createItemJson("ID", "strID", STRING_INPUT, 2, pParam->strID));
		jsonItem.append(createItemJson("宽度是否根据文字进行缩放", "bAdjustWidth", BOOL_INPUT, 2, pParam->bAdjustWidth));
		jsonItem.append(createItemJson("是否宽度居中", "bWidthCenter", BOOL_INPUT, 2, pParam->bWidthCenter));
		jsonItem.append(createItemJson("是否高度居中", "bHeightCenter", BOOL_INPUT, 2, pParam->bHeightCenter));
		jsonItem.append(createItemJson("单行最大长度", "nLineMaxLength", INT_INPUT, 2, pParam->nLineMaxLength));
		jsonItem.append(createItemJson("最大行数", "nMaxLine", INT_INPUT, 2, pParam->nMaxLine));
		jsonItem.append(createItemJson("描述", "strDescription", STRING_INPUT, 2, pParam->strDescription));
	}
	catch (const std::exception&)
	{

	}
}

void ParamParseStream::Clear(std::string strKey)
{
	for (std::vector<ReadParam>::iterator it = m_vecReadParam.begin(); it != m_vecReadParam.end();)
	{
		ReadParam* pReadParam = &(*it);
		if (strKey.empty() || StartWith((pReadParam->strKey), SCENE_ELEMENT_PARAM_KEY))
		{
			if (pReadParam->pParam)
			{
				delete pReadParam->pParam;
				pReadParam->pParam = NULL;
			}
			m_vecReadParam.erase(it);
		}
		else
		{
			it++;
		}
	}
	m_JRoot.clear();
}

void ParamParseStream::ReadJsonToParam(ReadParam &stParam, std::string strJson)
{
	readJsonToParam(stParam,strJson);
}

std::string ParamParseStream::ConvertParamToJson(NodeParamBase *pParam,Json::Value &value)
{
	if (!pParam)
	{
		return "";
	}
	std::string strTypeName;
	if (pParam->nNodeType == NODE_FONTBOARD)
	{
		convertFontBoardParam((FontBoardParam*)pParam, value);
		strTypeName = "NODE_FONTBOARD";
	}
	else if (pParam->nNodeType == NODE_BOARD)
	{
		convertBoardParam((BoardParam*)pParam, value);
		strTypeName = "NODE_BOARD";
	}
	else if (pParam->nNodeType == NODE_VIDEO_BOARD)
	{
		convertVideoBoardParam((VideoBoardParam*)pParam, value);
		strTypeName = "NODE_VIDEO_BOARD";
	}
	else if (pParam->nNodeType == NODE_FONTBOARD_WITH_BUTTON)
	{
		convertFontBoardWithButtonParam((FontBoardWithButtonParam*)pParam, value);
		strTypeName = "NODE_FONTBOARD_WITH_BUTTON";
	}
	else if (pParam->nNodeType == NODE_VIDEO_BOARD_WITH_BUTTON)
	{
		convertVideoBoardWithButtonParam((VideoBoardWithButtonParam*)pParam, value);
		strTypeName = "NODE_VIDEO_BOARD_WITH_BUTTON";
	}
	else if (pParam->nNodeType == NODE_BOARD_WITH_FONTBOARD)
	{
		convertBoardWithFontBoardParam((BoardWithFontBoardParam*)pParam, value);
		strTypeName = "NODE_BOARD_WITH_FONTBOARD";
	}
	else if (pParam->nNodeType == NODE_SPOT_LIGHT_BOARD)
	{
		convertSpotLightBoardParam((SpotLightBoardParam*)pParam, value);
		strTypeName = "NODE_SPOT_LIGHT_BOARD";
	}
	else if (pParam->nNodeType == NODE_LIGHT_BEAM)
	{
		convertLightBeamParam((LightBeamParam*)pParam, value);
		strTypeName = "NODE_LIGHT_BEAM";
	}
	else if (pParam->nNodeType == NODE_POLYGON)
	{
		convertPolygonParam((PolygonParam*)pParam, value);
		strTypeName = "NODE_POLYGON";
	}
	else if (pParam->nNodeType == NODE_ROAD)
	{
		convertRoadParam((RoadParam*)pParam, value);
		strTypeName = "NODE_ROAD";
	}
	else if (pParam->nNodeType == NODE_BOARD_INSTANCE)
	{
		convertBoardInstanceParam((BoardInstanceParam*)pParam, value);
		strTypeName = "NODE_BOARD_INSTANCE";
	}
	else if (pParam->nNodeType == NODE_LOCUS_LINE)
	{
		convertLocusLineParam((LocusLineParam*)pParam, value);
		strTypeName = "NODE_LOCUSLINE";
	}
	else if (pParam->nNodeType == NODE_FLYLINE)
	{
		convertFlyLineParam((FlyLineParam*)pParam, value);
		strTypeName = "NODE_FLYLINE";
	}
	else if (pParam->nNodeType == NODE_MULTI_FLYLINE)
	{
		convertMultiFlyLineParam((MultiFlyLineParam*)pParam, value);
		strTypeName = "NODE_MULTI_FLYLINE";
	}

	return strTypeName;
}

int ParamParseStream::GetNodeTypeByName(std::string strName)
{
	//IViewManager* pViewManager = APIProvider::GetSystemAPI()->iCoreAPI->iGetViewManager();
	//return pViewManager->iGetNodeType(strName.c_str());

	if (strName == "NODE_BOARD")
	{
		return NodeType_e::NODE_BOARD;
	}
	else if (strName == "NODE_FONTBOARD")
	{
		return NodeType_e::NODE_FONTBOARD;
	}
	else if (strName == "NODE_VIDEO_BOARD")
	{
		return NodeType_e::NODE_VIDEO_BOARD;
	}
	else if (strName == "NODE_FONTBOARD_WITH_BUTTON")
	{
		return NodeType_e::NODE_FONTBOARD_WITH_BUTTON;
	}
	else if (strName == "NODE_VIDEO_BOARD_WITH_BUTTON")
	{
		return NodeType_e::NODE_VIDEO_BOARD_WITH_BUTTON;
	}
	else if (strName == "NODE_BOARD_WITH_FONTBOARD")
	{
		return NodeType_e::NODE_BOARD_WITH_FONTBOARD;
	}
	else if (strName == "NODE_SPOT_LIGHT_BOARD")
	{
		return NodeType_e::NODE_SPOT_LIGHT_BOARD;
	}
	else if (strName == "NODE_LIGHT_BEAM")
	{
		return NodeType_e::NODE_LIGHT_BEAM;
	}
	else if (strName == "NODE_POLYGON")
	{
		return NodeType_e::NODE_POLYGON;
	}
	else if (strName == "NODE_ROAD")
	{
		return NodeType_e::NODE_ROAD;
	}
	else if (strName == "NODE_BOARD_INSTANCE")
	{
		return NodeType_e::NODE_BOARD_INSTANCE;
	}
	else if (strName == "NODE_LOCUSLINE")
	{
		return NodeType_e::NODE_LOCUS_LINE;
	}
	else if (strName == "NODE_FLYLINE")
	{
		return NodeType_e::NODE_FLYLINE;
	}
	else if (strName == "NODE_MULTI_FLYLINE")
	{
		return NodeType_e::NODE_MULTI_FLYLINE;
	}
	return NODE_UNKNOWN;
}

std::string ParamParseStream::ConvertTextShowToJson(TextShow& textShow)
{
	std::string strResult = "";
	try
	{
		Json::Value item;
		convertTextShow(&textShow,item);

		strResult = item.toStyledString();
	}
	catch (const std::exception&)
	{

	}
	return strResult;
}

