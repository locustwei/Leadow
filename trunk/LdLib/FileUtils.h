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
	//��ȡ�ļ����е��̷�
	static BOOL ExtractFileDrive(LPTSTR lpFullName, __out LPTSTR lpDriveName);
	//��ȡ�ļ����е�·��
	static UINT ExtractFilePath(LPTSTR lpFullName, __out LPTSTR lpFilePath);
	//��ȡ�����ļ����е��ļ���
	static UINT ExtractFileName(LPTSTR lpFullName, __out LPTSTR lpName);
	//��ȡ�ļ����е���չ��
	static UINT ExtractFileExt(LPTSTR lpFullName, __out LPTSTR lpName);
	//Dos·��ת�����豸·����C:\ ���� \\device\partion1\)
	static UINT Win32Path2DevicePath(LPTSTR lpFullName, __out LPTSTR lpDevicePath);
	//�豸·��ת����Dos·��
	static UINT DevicePathToWin32Path(LPTSTR lpDevicePath, __out LPTSTR lpDosPath);
	//static DWORD GetFileAttribute(LPTSTR lpFullName);
	//����������ڣ������ļ�·��
	static DWORD ForceDirectories(LPTSTR lpFullPath);
	//Ŀ¼�Ƿ����
	static BOOL IsDirectoryExists(LPTSTR lpFullPath);
	//static VOID IncludeTrailingPathDelimiter
	//todo �ļ�����������
	//static DWORD SetFileInfo(LPTSTR lpFullName, )
	//��ȡ�ļ�ѹ��״̬��Windows �ļ����Է�ZIP��
	static DWORD GetCompressStatus(LPTSTR lpFullName, PWORD pStatus);
	static BOOL SetCompression(LPTSTR lpFullName, BOOL bCompress);
};

