/*
ʹ��MSXML��VC++�н���XML�ļ�ʱ��ֻ��Ҫ�������漸�㣺
	1����ʼ��COM�⣬CoInitialize(NULL)�����Է���InitInstance()�������档�ͷ�COM�⣬CoUninitialize()�����Է���ExitInstance()�������档
	2����ͷ�ļ�����������´���
*/
#pragma once

#include <comdef.h>


#define XMLNode IXMLDOMElement*
#define XMLList IXMLDOMNodeList*

class CLdXml
{
public:
	CLdXml();
	~CLdXml();
	BOOL OpenFile(LPCTSTR szFileName);
	XMLNode FindNode(LPCTSTR szPath);
	XMLNode AddChild( XMLNode node, LPCTSTR szName );
	BOOL setAttribute(XMLNode node, LPCTSTR szName, VARIANT value);
	BOOL CLdXml::setAttribute(XMLNode node, LPCTSTR szName, LPCTSTR value);
	BOOL SaveFile( LPCTSTR szFileName );
	LPTSTR GetNodeAttrubeAsStr(IXMLDOMNode* node, LPCTSTR szName);
	XMLList FindNodes(LPCTSTR szPath);
private:
	IXMLDOMDocument* pXMLDoc;
	IXMLDOMElement* pRoot;
};
