#pragma once
#include <Shlobj.h>
#include "LdStructs.h"
#include "LdString.h"
#include "ldArray.h"
#include "LdMap.h"

typedef struct _SH_HEAD_INFO
{
	int fmt;  //align
	int cxChar; //宽度（字符数）
	TCHAR* szName; //名称
}SH_HEAD_INFO, *PSH_HEAD_INFO;

class CSHFolders
{
public:
	/*
	枚举特殊列信息（保护所有类型的列信息，指定类型（dwFolder）放前面）
	dwFolder 特殊文件夹CLSID
	PSH_HEAD_INFO 列信息
	*/
	static HRESULT EnumFolderColumes(DWORD dwFolder, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param, HWND hOwnerWnd = nullptr);
	static HRESULT EnumFolderColumes(TCHAR* lpFullName, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param, HWND hOwnerWnd = nullptr);
	/*
	枚举特殊目录下的文件（如桌面（CSIDL_DESKTOP）、回收站（CSIDL_BITBUCKET）
	dwFolder 特殊文件夹CLSID
	CLdArray<TCHAR*>* 每个列值（EnumFolderColumes 对应列）
	Param 回掉接口参数
	hOwnerWnd SH类函数OwnerHandle参数
	*/
	static HRESULT EnumFolderObjects(DWORD dwFolder, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd = nullptr);
	static HRESULT EnumFolderObjects(TCHAR* lpFullName, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd = nullptr);
	//FOLDERID_Downloads
	static CLdString GetKnownFolderPath(const REFKNOWNFOLDERID rfid, DWORD dwFlag = KF_FLAG_DEFAULT, HANDLE hToken = NULL);
	/*
	获取文件的属性值90
	*/
	static HRESULT GetFileAttributeValue(TCHAR* lpFullName, CLdArray<TCHAR*>* values);

private:
	static HRESULT GetShellFolder(DWORD dwFolder, IShellFolder2** pFolder, LPITEMIDLIST* pidlist);
	static HRESULT GetShellFolder(TCHAR* lpFullName, IShellFolder2** pFolder, LPITEMIDLIST* pidlist);
	static HRESULT GetFileItemid(TCHAR* lpFullName, LPITEMIDLIST* pidl);
	static HRESULT EnumFolderColumes(IShellFolder2* pFolder, LPITEMIDLIST pidl, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param);
};

