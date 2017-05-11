#pragma once

class CFileUtils
{
public:
	static BOOL ExtractFileDrive(LPTSTR lpFullName, __out LPTSTR lpDriveName);
	static UINT ExtractFilePath(LPTSTR lpFullName, __out LPTSTR lpFilePath);
	static UINT ExtractFileName(LPCTSTR lpFullName, __out LPCTSTR lpName);
};

