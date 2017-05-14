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
};

