#pragma once
#include <Shlobj.h>
#include "LdStructs.h"
#include "LdString.h"
#include "ldArray.h"
#include "LdMap.h"

typedef struct _SH_HEAD_INFO
{
	int fmt;  //align
	int cxChar; //��ȣ��ַ�����
	TCHAR* szName; //����
}SH_HEAD_INFO, *PSH_HEAD_INFO;

class CSHFolders
{
public:
	/*
	ö����������Ϣ�������������͵�����Ϣ��ָ�����ͣ�dwFolder����ǰ�棩
	dwFolder �����ļ���CLSID
	PSH_HEAD_INFO ����Ϣ
	*/
	static HRESULT EnumFolderColumes(DWORD dwFolder, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param, HWND hOwnerWnd = nullptr);
	static HRESULT EnumFolderColumes(TCHAR* lpFullName, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param, HWND hOwnerWnd = nullptr);
	/*
	ö������Ŀ¼�µ��ļ��������棨CSIDL_DESKTOP��������վ��CSIDL_BITBUCKET��
	dwFolder �����ļ���CLSID
	CLdArray<TCHAR*>* ÿ����ֵ��EnumFolderColumes ��Ӧ�У�
	Param �ص��ӿڲ���
	hOwnerWnd SH�ຯ��OwnerHandle����
	*/
	static HRESULT EnumFolderObjects(DWORD dwFolder, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd = nullptr);
	static HRESULT EnumFolderObjects(TCHAR* lpFullName, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd = nullptr);
	//FOLDERID_Downloads
	static CLdString GetKnownFolderPath(const REFKNOWNFOLDERID rfid, DWORD dwFlag = KF_FLAG_DEFAULT, HANDLE hToken = NULL);
	/*
	��ȡ�ļ�������ֵ90
	*/
	static HRESULT GetFileAttributeValue(TCHAR* lpFullName, CLdArray<TCHAR*>* values);

private:
	static HRESULT GetShellFolder(DWORD dwFolder, IShellFolder2** pFolder, LPITEMIDLIST* pidlist);
	static HRESULT GetShellFolder(TCHAR* lpFullName, IShellFolder2** pFolder, LPITEMIDLIST* pidlist);
	static HRESULT GetFileItemid(TCHAR* lpFullName, LPITEMIDLIST* pidl);
	static HRESULT EnumFolderColumes(IShellFolder2* pFolder, LPITEMIDLIST pidl, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param);
};

