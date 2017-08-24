#pragma once
#include "../ldapp/LdStructs.h"
#include "../classes/LdString.h"
#include "../classes/LdArray.h"

namespace LeadowLib {

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
		static int ShDeleteFile(TCHAR* lpFileName, DWORD dwFlag = FOF_NO_UI);
		static DWORD DeleteFile(TCHAR* lpFileName);
		//�ļ���С����ַ���
		static void FormatFileSize(INT64 nSize, CLdString& result);
		//����ļ���
		static void GenerateRandomFileName(int length, CLdString* Out);
		//��ȡ�ļ��������������ƣ�Alternate Data Streams ��
		static DWORD GetFileADSNames(TCHAR* lpFileName, CLdArray<TCHAR*>* result);
	};

};