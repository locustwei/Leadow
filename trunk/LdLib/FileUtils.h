#pragma once
#include "ldstring.h"

class CFileInfo
{
public:
	CFileInfo();
	CFileInfo(TCHAR* pFileName);
	~CFileInfo();
	LARGE_INTEGER GetCreateTime();
	LARGE_INTEGER GetChangeTime();
	LARGE_INTEGER GetLastWriteTime();
	LARGE_INTEGER GetLastAccessTime();
	INT64 GetAllocateSize();
	INT64 GetDataSize();
	DWORD GetAttributes();
	CLdString& GetFileName();
	VOID SetFileName(TCHAR* pFileName);
	static CLdString FormatFileSize(INT64 nSize);
private:
	FILE_BASIC_INFO m_Baseinfo;
	FILE_STANDARD_INFO m_StandrardInfo;
	CLdString Name;                //

	void ClearValue();
	void GetFileBasicInfo();
	void GetFileStandardInfo();
};

class CFileUtils
{
public:
	static DWORD OpenFileDlg(HWND hOwner, LPCTSTR lpFilter, LPCTSTR lpFolder, DWORD dwFlags);
	//提取文件名中的盘符
	static BOOL ExtractFileDrive(LPTSTR lpFullName, __out LPTSTR lpDriveName);
	//提取文件名中的路径
	static UINT ExtractFilePath(LPTSTR lpFullName, __out LPTSTR lpFilePath);
	//提取完整文件名中的文件名
	static UINT ExtractFileName(LPTSTR lpFullName, __out LPTSTR lpName);
	//提取文件名中的扩展名
	static UINT ExtractFileExt(LPTSTR lpFullName, __out LPTSTR lpName);
	//Dos路径转换成设备路径（C:\ —— \\device\partion1\)
	static UINT Win32Path2DevicePath(LPTSTR lpFullName, __out LPTSTR lpDevicePath);
	//设备路径转换成Dos路径
	static UINT DevicePathToWin32Path(LPTSTR lpDevicePath, __out LPTSTR lpDosPath);
	//static DWORD GetFileAttribute(LPTSTR lpFullName);
	//（如果不存在）创建文件路径
	static DWORD ForceDirectories(LPTSTR lpFullPath);
	//目录是否存在
	static BOOL IsDirectoryExists(LPTSTR lpFullPath);
	//static VOID IncludeTrailingPathDelimiter
	//todo 文件交换数据流
	//static DWORD SetFileInfo(LPTSTR lpFullName, )
	//获取文件压缩状态（Windows 文件属性非ZIP）
	static DWORD GetCompressStatus(LPTSTR lpFullName, PWORD pStatus);
	static BOOL SetCompression(LPTSTR lpFullName, BOOL bCompress);
};

