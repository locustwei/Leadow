#pragma once
#include "ldstring.h"
#include "LdStructs.h"

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
	//��ȡ�ļ����е��̷�
	static BOOL ExtractFileDrive(TCHAR* lpFullName, __out TCHAR* lpDriveName);
	//��ȡ�ļ����е�·��
	static UINT ExtractFilePath(TCHAR* lpFullName, __out TCHAR* lpFilePath);
	//��ȡ�����ļ����е��ļ���
	static UINT ExtractFileName(TCHAR* lpFullName, __out TCHAR* lpName);
	//��ȡ�ļ����е���չ��
	static UINT ExtractFileExt(TCHAR* lpFullName, __out TCHAR* lpName);
	//Dos·��ת�����豸·����C:\ ���� \\device\partion1\)
	static UINT Win32Path2DevicePath(TCHAR* lpFullName, __out TCHAR* lpDevicePath);
	//�豸·��ת����Dos·��
	static UINT DevicePathToWin32Path(TCHAR* lpDevicePath, __out TCHAR* lpDosPath);
	//static DWORD GetFileAttribute(TCHAR* lpFullName);
	//����������ڣ������ļ�·��
	static DWORD ForceDirectories(TCHAR* lpFullPath);
	//Ŀ¼�Ƿ����
	static BOOL IsDirectoryExists(TCHAR* lpFullPath);
	//static VOID IncludeTrailingPathDelimiter
	//todo �ļ�����������
	//static DWORD SetFileInfo(TCHAR* lpFullName, )
	//��ȡ�ļ�ѹ��״̬��Windows �ļ����Է�ZIP��
	static DWORD GetCompressStatus(TCHAR* lpFullName, PWORD pStatus);
	static BOOL SetCompression(TCHAR* lpFullName, BOOL bCompress);
	/*
	FindFirst, FindNext ����Ŀ¼���� �ݹ���Ŀ¼��
	Param 
	*/
	static DWORD FindFile(TCHAR* lpFullPath, TCHAR* lpFilter, IGernalCallback<LPWIN32_FIND_DATA>* callback, UINT_PTR Param);
	/*
	ɾ���ļ���SHFileOperation��
	*/
	static int DeleteFile(TCHAR* lpFileName, DWORD dwFlag = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NO_UI | FOF_SILENT);
};

