#pragma once

class CFileUtils
{
public:
	static BOOL ExtractFileDrive(LPTSTR lpFullName, __out LPTSTR lpDriveName);
	static UINT ExtractFilePath(LPTSTR lpFullName, __out LPTSTR lpFilePath);
	static UINT ExtractFileName(LPTSTR lpFullName, __out LPTSTR lpName);
	static UINT ExtractFileExt(LPTSTR lpFullName, __out LPTSTR lpName);
	static UINT Win32Path2DevicePath(LPTSTR lpFullName, __out LPTSTR lpDevicePath);
	static UINT DevicePathToWin32Path(LPTSTR lpDevicePath, __out LPTSTR lpDosPath);
	//static DWORD GetFileAttribute(LPTSTR lpFullName);
	static DWORD ForceDirectories(LPTSTR lpFullPath);
	static BOOL IsDirectoryExists(LPTSTR lpFullPath);
	//static VOID IncludeTrailingPathDelimiter
	//todo 文件交换数据流
	//static DWORD SetFileInfo(LPTSTR lpFullName, )
	static DWORD GetCompressStatus(LPTSTR lpFullName, PWORD pStatus);
	static BOOL SetCompression(LPTSTR lpFullName, BOOL bCompress);
};

