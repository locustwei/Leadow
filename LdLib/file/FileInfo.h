#pragma once

/*
�ļ����ļ�����Ϣ��ȡ

*/

#include "../classes/ldstring.h"
#include "../classes/LdArray.h"

namespace LeadowLib {
	//�ļ���������
	enum VF_FILE_TYPE
	{
		vft_error,     
		vft_adstream,  //�ļ�������������NTFS֧�֣�
		vft_file,      //�ļ�
		vft_folder,    //�ļ���
		vft_volume     //���̷���
	};

	//�ļ��������
	class CVirtualFile
	{
	public:
		CVirtualFile():m_FileName()
		{
			m_Tag = 0;
			m_Folder = nullptr;
		};
		//��·���ļ���
		virtual TCHAR* GetFullName();   
		//����·���ļ���
		virtual TCHAR* GetFileName();

		virtual INT64 GetDataSize() = 0;
		virtual DWORD GetAttributes() = 0;
		virtual VF_FILE_TYPE GetFileType() = 0;
		//virtual CLdArray<CVirtualFile*>* GetFiles() = 0;
		//�����ļ���
		virtual CVirtualFile* GetFolder() { return m_Folder; };
		//
		UINT_PTR GetTag() { return m_Tag; };
		void SetTag(UINT_PTR Tag) { m_Tag = Tag; };
	protected:
		UINT_PTR m_Tag;
		CLdString m_FileName;
		CVirtualFile* m_Folder;
	};
	//�ļ�����������
	class CADStream: public CVirtualFile
	{
		friend class CFileInfo;
	public:
		CADStream() :m_File(nullptr) { m_DataSize = 0; };
		INT64 GetDataSize() override { return m_DataSize; };
		DWORD GetAttributes() override { return 0; };
		VF_FILE_TYPE GetFileType() override { return vft_adstream; };
		//CLdArray<CVirtualFile*>* GetFiles() override { return nullptr; };
		CVirtualFile* GetFolder() override { return m_File; };

		TCHAR* GetFileName() override;
	private:
		CVirtualFile* m_File;
		INT64 m_DataSize;
	};
	//�ļ�
	class CFileInfo : public CVirtualFile
	{
		friend class CFindFileCallbackImpl;
		friend class CFolderInfo;
	public:
		CFileInfo();
		virtual ~CFileInfo();

		//���Ķ���ָ���ļ���
		virtual bool SetFileName(TCHAR* pFileName);
		bool SetFindData(const TCHAR* pPath, PWIN32_FIND_DATA pData);
		//ADS�ļ�����������
		CLdArray<CVirtualFile*>* GetADStreams();

		//�ļ�����-----------------------------------------------------------------------------
		FILETIME GetCreateTime();
		FILETIME GetLastWriteTime();
		FILETIME GetLastAccessTime();
		INT64 GetDataSize() override;
		DWORD GetAttributes() override;
		VF_FILE_TYPE GetFileType() override { return vft_file; };
	protected:
		typedef struct FILE_ATTRIBUTE_DATA {
			DWORD dwFileAttributes;
			FILETIME ftCreationTime;
			FILETIME ftLastAccessTime;
			FILETIME ftLastWriteTime;
			UINT64 nFileSize;
			DWORD nFileCount;        //���ļ��������ļ��а��������ļ��У�
			DWORD nFolderCount;      //���ļ�������ͬ�ϣ�
		} *PFILE_ATTRIBUTE_DATA;

		FILE_ATTRIBUTE_DATA m_AttributeData;

		void ClearValue();
		bool GetFileBasicInfo();
	private:

		CLdArray<CVirtualFile*>* m_Streams;
	};
	//�ļ���
	class CFolderInfo : public CFileInfo
	{
		friend class CFindFileCallbackImpl;
	public:
		CFolderInfo();
		//ö��Ŀ¼�µ����ļ�
		//if bTree ��ݹ�ö����Ŀ¼�ļ�
		DWORD FindFiles(bool bTree = false);
		//Ŀ¼�µ��ļ�
		CLdArray<CVirtualFile*>* GetFiles();
		int AddFile(CFileInfo* pFile);
		//���ļ�����bTree�������ļ��У�
		UINT GetFilesCount(bool bTree = false) const;
		//�����ļ���bSub:�����ļ��в��ң�bPath���Ƚ�·��
		CFileInfo* Find(TCHAR* pName, bool bSub = false, bool bPath = false);
		void Sort();
		VF_FILE_TYPE GetFileType() override { return vft_folder; };
	private:
		CLdArray<CVirtualFile*> m_Files;
		int AddFile(PWIN32_FIND_DATA pData);
	};

};