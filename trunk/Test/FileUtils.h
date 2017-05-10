#pragma once

class CFileUtils
{
public:
	static BOOL ExtractFileDrive(LPTSTR lpFileName, __out LPTSTR lpDriveName);
	static BOOL ExtractFilePath(LPTSTR lpFileName, __out LPTSTR lpFilePath);
};

