#pragma once

namespace bc
{
	class  TWXmlParser
	{
	public:
		TWXmlParser();
		virtual ~TWXmlParser();

		bool Load(char* strName);
		bool Save(char* strName);

		XMLNodePtr GetFirstNode();
		XMLNodePtr GetFirstChildNode(XMLNodePtr pNode);
		XMLNodePtr GetNextNode(XMLNodePtr pNode);
	public:
		XMLDoc		m_Document;
		XMLNodePtr	m_pCurNode;
	};
}

