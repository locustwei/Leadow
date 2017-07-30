#pragma once
#include "LdStructs.h"

class CLdString;

class CFileUtils
{
public:
	//提取文件名中的盘符
	static BOOL ExtractFileDrive(TCHAR* lpFullName, __out TCHAR* lpDriveName);
	//提取文件名中的路径
	static UINT ExtractFilePath(TCHAR* lpFullName, __out TCHAR* lpFilePath);
	//提取完整文件名中的文件名
	static UINT ExtractFileName(TCHAR* lpFullName, __out TCHAR* lpName);
	//提取文件名中的扩展名
	static UINT ExtractFileExt(TCHAR* lpFullName, __out TCHAR* lpName);
	//Dos路径转换成设备路径（C:\ —— \\device\partion1\)
	static UINT Win32Path2DevicePath(TCHAR* lpFullName, __out TCHAR* lpDevicePath);
	//设备路径转换成Dos路径
	static UINT DevicePathToWin32Path(TCHAR* lpDevicePath, __out TCHAR* lpDosPath);
	//static DWORD GetFileAttribute(TCHAR* lpFullName);
	//（如果不存在）创建文件路径
	static DWORD ForceDirectories(TCHAR* lpFullPath);
	//目录是否存在
	static BOOL IsDirectoryExists(TCHAR* lpFullPath);
	//static VOID IncludeTrailingPathDelimiter
	//todo 文件交换数据流
	//static DWORD SetFileInfo(TCHAR* lpFullName, )
	//获取文件压缩状态（Windows 文件属性非ZIP）
	static DWORD GetCompressStatus(TCHAR* lpFullName, PWORD pStatus);
	static BOOL SetCompression(TCHAR* lpFullName, BOOL bCompress);
	/*
	FindFirst, FindNext 搜索目录（不 递归子目录）
	Param 
	*/
	static DWORD FindFile(TCHAR* lpFullPath, TCHAR* lpFilter, IGernalCallback<LPWIN32_FIND_DATA>* callback, UINT_PTR Param);
	/*
	删除文件（SHFileOperation）
	*/
	static int ShDeleteFile(TCHAR* lpFileName, DWORD dwFlag = FOF_NO_UI);
	static DWORD DeleteFile(TCHAR* lpFileName);

	static void FormatFileSize(INT64 nSize, CLdString& result);
	static void GenerateRandomFileName(int length, CLdString& Out);

};

