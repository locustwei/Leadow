/************************************************************************/
/* ͨ���Ź�Ʊ������������ݽṹ�ͳ������ݡ�
*/
/************************************************************************/

#pragma once
#include "stdafx.h"
#include <commctrl.h>

#define PLUG_FILENAME L"TdxPlugin.dll"  //��������ļ���

#define Stock_ID "��Ʊ" //FeatureID "group.stock" HomeFeatureID "" Title "��Ʊ">
#define Stock_Buy_ID "Stock.Buy" //FeatureID "Stock.Buy" Title "����" Image "1"/>
#define Stock_Sell_ID "Stock.Sell" //FeatureID "Stock.Sell" Title "����" Image "2"/>
#define Stock_marketBuy_ID "Stock.marketBuy" //FeatureID "Stock.marketBuy" Title "�м�����" Image "1"/>
#define Stock_marketsell_ID "Stock.marketsell" //FeatureID "Stock.marketsell" Title "�м�����" Image "2"/>
#define Stock_SingleBuylots_ID "Stock.SingleBuylots" //FeatureID "Stock.SingleBuylots" Title "���ʻ���������" Image "1"/>
#define Stock_SingleSellLots_ID "Stock.SingleSellLots" //FeatureID "Stock.SingleSellLots" Title "���ʻ���������" Image "2"/>
#define query_ID "query" //FeatureID "AlwaysOn" Title "��ѯ" Image "5">
#define Stock_zjgf_ID "Stock.zjgf" //FeatureID "Stock.zjgf" Title "�ʽ�ɷ�"/>
#define Stock_lscjcx_ID "Stock.lscjcx" //FeatureID "Stock.lscjcx" Title "��ʷ�ɽ�"/>
#define Stock_dzd_ID "Stock.dzd" //FeatureID "Stock.dzd" Title "�ʽ���ϸ"/>
#define Stock_zjls_ID "Stock.zjls" //FeatureID "Stock.zjls" Title "�ʽ���ϸ"/>
#define Stock_cjhzcx_id "Stock.cjhzcx" //FeatureID "Stock.cjhzcx" Title "���ճɽ����ܲ�ѯ"/>
#define Stock_wthzcx_ID "Stock.wthzcx" //FeatureID "Stock.wthzcx" Title "����ί�л��ܲ�ѯ"/>
#define Stock_lscjhzcx_ID "Stock.lscjhzcx" //FeatureID "Stock.lscjhzcx" Title "��ʷ�ɽ����ܲ�ѯ"/>		

#define YXJY_ID "YXJY" //FeatureID "AlwaysOn" Title "�ɷݱ���ת��">
#define Stock_yxmr_ID "Stock.yxmr" //FeatureID "Stock.yxmr" Title "��������"/>
#define Stock_yxmc_ID "Stock.yxmc" //FeatureID "Stock.yxmc" Title "��������"/>
#define Stock_djmr_ID "Stock.djmr" //FeatureID "Stock.djmr" Title "��������"/>
#define Stock_djmc_ID "Stock.djmc" //FeatureID "Stock.djmc" Title "��������"/>
#define Stock_cjqrmr_ID "Stock.cjqrmr" //FeatureID "Stock.cjqrmr" Title "�ɽ�ȷ������"/>	
#define Stock_cjqrmc_ID "Stock.cjqrmc" //FeatureID "Stock.cjqrmc" Title "�ɽ�ȷ������"/>		
#define Stock_yxhqcx_ID "Stock.yxhqcx" //FeatureID "Stock.yxhqcx" Title "���������ѯ"/>					

//#define lof_ID "lof" FeatureID "AlwaysOn" Title "����ί��" Image "5">
#define Stock_ymd_ID "Stock.ymd" //FeatureID "Stock.ymd" Title "Ԥ��" Image "4"/>
#define Stock_yjwt_ID "Stock.yjwt" //FeatureID "Stock.yjwt" Title "Ԥ��ί��" Image "7"/>

//�������������ṹ(TcOem.xml�ļ�)
typedef struct _Nav_Tree
{
	//ID "��Ʊ" FeatureID "group.stock" HomeFeatureID "" Title "��Ʊ"
	LPTSTR ID;
	LPTSTR FeatureID;
	LPTSTR Title;
	LPTSTR HomeFeatureID;
	LPTSTR HotKeytc;
	int count;
	_Nav_Tree* Items;
}Nav_Tree, *PNav_Tree;

//TDX����ʱTreeItem Param�����ж�ӦTcOem.xml�ļ������ݽṹ
typedef struct _Tdx_TreeItemData
{
	_Tdx_TreeItemData* pID;
	_Tdx_TreeItemData* pFeatureID;
	CHAR ID[0x40];
	CHAR FeatureID[0x40];
}Tdx_TreeItemData, *PTdx_TreeItemData;

//TDX������TreeItem Param�Ľṹ(TreeView_GetItemParam)
typedef struct _Tdx_TreeItemParam
{
	_Tdx_TreeItemParam* pNext;
	_Tdx_TreeItemParam* pPrev;
	DWORD unknow_1;
	_Tdx_TreeItemParam* pParent;
	DWORD unknow_2;
	DWORD unknow_3;
	PTdx_TreeItemData pData;
}Tdx_TreeItemParam, *PTdx_TreeItemParam;

//ͨ���ŵ����˵�����TreeView���ڶ�Ӧ
typedef struct _Tdx_Tree_HWND
{
	HWND hTreeView;
	PTdx_TreeItemParam pItem;
}Tdx_Tree_HWND, *PTdx_Tree_HWND;

//ͨ���ŵ����˵���Ŀ��DialogBox��Ӧ
typedef struct _Tdx_TreeItem_Dlg
{
	HWND hTreeView;
	HTREEITEM hItem;
	HWND hDialog;
}Tdx_TreeItem_Dlg, *PTdx_TreeItem_Dlg;


/*  �������õĺ���

//�޸�TcOem.xml�ļ������뱾�������װʱ����
LPTSTR AddPlugInConfig(CLdXml* xml)
{
	XMLNode node = xml->FindNode(L"//Addin");
	if(!node)
		return NULL;
	XMLList childs;
	LPTSTR fName = NULL;

	if(SUCCEEDED(node->get_childNodes(&childs))){
		long lengtth;
		childs->get_length(&lengtth);
		for(int i=0; i<lengtth; i++){
			IXMLDOMNode* child;
			childs->get_item(i, &child);

			fName = xml->GetNodeAttrubeAsStr(node, L"FileName");
			if(fName){
				if(wcscmp(PLUG_FILENAME, fName)==0)
					return xml->GetNodeAttrubeAsStr(node, L"MapFile");
			}
		}

		XMLNode xmlnode = xml->AddChild(node, L"ITEM");
		xml->setAttribute(xmlnode, L"FileName", PLUG_FILENAME);
		xml->setAttribute(xmlnode, L"Pivotal", L"NO");
		xml->setAttribute(xmlnode, L"Enable", L"YES");
		xml->setAttribute(xmlnode, L"MapFile", fName);
	}

	return fName;
}

Nav_Tree NavTree = {0};

//��ȡTcOem.xml�ļ��У�ͨ���Ž��׳�����ർ�������ܲ˵�
void LoadNavTree(CLdXml& xml, IXMLDOMNode* node, PNav_Tree navtree)
{
	if(node==NULL || navtree==NULL)
		return;

	LPTSTR nv = xml.GetNodeAttrubeAsStr(node, L"ID");
	if(nv){
		navtree->ID = nv;
	}
	nv = xml.GetNodeAttrubeAsStr(node, L"FeatureID");
	if(nv)
		navtree->FeatureID = nv;
	nv = xml.GetNodeAttrubeAsStr(node, L"Title");
	if(nv)
		navtree->Title = nv;
	nv = xml.GetNodeAttrubeAsStr(node, L"HomeFeatureID");
	if(nv)
		navtree->HomeFeatureID = nv;
	nv = xml.GetNodeAttrubeAsStr(node, L"HotKeytc");
	if(nv){
		navtree->HotKeytc = nv;
		LPTSTR title = new TCHAR[80];
		ZeroMemory(title, 80*sizeof(TCHAR));
		wsprintf(title, navtree->Title, L"[", nv, L"]");
		delete navtree->Title;
		navtree->Title = title;
	}


	XMLList childs;
	if(SUCCEEDED(node->get_childNodes(&childs))){
		childs->get_length((long*)&navtree->count);
		navtree->Items = new Nav_Tree[navtree->count];
		for(int i=0; i<navtree->count; i++){
			IXMLDOMNode* child;
			childs->get_item(i, &child);
			LoadNavTree(xml, (IXMLDOMNode*)child, &navtree->Items[i]);
		}
	}
}

BOOL InitNavTree()
{
	CLdXml xml;
	if(xml.OpenFile(L"..\\TcOem.xml")){
		XMLNode node = xml.FindNode(L"//UI/Nav/Content");
		if(node){
			LoadNavTree(xml, node, &NavTree);
			return TRUE;
		}
	}
	return FALSE;
}

int GetNavTreeItemCount(PNav_Tree treeitem)
{
	int result = 0;
	for(int i=0; i<treeitem->count; i++)
		result += GetNavTreeItemCount(&treeitem->Items[i]);
	return result + treeitem->count;
}
*/