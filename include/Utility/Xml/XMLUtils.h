#pragma once
#include <string>

namespace bc
{
	class XMLUtils
	{
		public:
			static void ReadColor(XMLNodePtr& node, const std::string& key, Vector4& outColor);
			static void ReadColor(XMLNodePtr& node, const std::string& key, Vector4d& outColor);
			static void ReadPosition(XMLNodePtr& node, const std::string& key, Vector3& pos);
			static void ReadPosition(XMLNodePtr& node, const std::string& key, Vector3d& pos);
			static void ReadFValue(XMLNodePtr& node, const std::string& key, float& value);
			static void ReadIValue(XMLNodePtr& node, const std::string& key, int32_t& value);
			static void ReadINT(XMLNodePtr& node, const std::string& key, int32_t& value);
			static void ReadStringValue(XMLNodePtr& node, const std::string& key, std::string& value);
			static void ReadStringAttribute(XMLNodePtr& node, const std::string& key, const std::string& attr, std::string& value);
			static void ReadString(XMLNodePtr& node, const std::string& key, std::string& value);

			static void ReadBoolean(XMLNodePtr& node, const std::string& key, bool& value);
			static void ReadBooleanEnable(XMLNodePtr& node, const std::string& key, bool& value);

			static void WriteColorNode(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const Vector4& inColor);
			static void WriteColorNode(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const Vector4d& inColor);
			static void WritePositionNode(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const Vector3& inPos);
			static void WritePositionNode(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const Vector3d& inPos);
			static void WriteFValueNode(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const float& value);

			static void WriteStringValueNode(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const std::string& value);

			static void WriteIValueNode(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const int32_t& value);
			static void WriteBooleanEnable(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const bool& value);
			static void WriteBoolean(XMLDoc& doc, XMLNodePtr& root, XMLNodePtr& node, const bool& value);
	};
};



