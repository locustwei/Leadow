#pragma once
#include "../classes/ldstring.h"
#include "../classes/LdArray.h"

namespace LeadowLib {
	enum VF_FILE_TYPE
	{
		vft_error,
		vft_file,
		vft_folder,
		vft_volume
	};

	class CVirtualFile
	{
	public:
		CVirtualFile() { m_Tag = 0; };
		virtual TCHAR* GetFullName() = 0;
		virtual TCHAR* GetFileName() = 0;
		virtual INT64 GetDataSize() = 0;
		virtual DWORD GetAttributes() = 0;
		virtual VF_FILE_TYPE GetFileType() = 0;
		virtual CLdArray<CVirtualFile*>* GetFiles() = 0;
		virtual CVirtualFile* GetFolder() = 0;
		UINT_PTR GetTag() { return m_Tag; };
		void SetTag(UINT_PTR Tag) { m_Tag = Tag; };
	protected:
		UINT_PTR m_Tag;
	};

	class CFileInfo : public CVirtualFile
	{
		friend class CFindFileCallbackImpl;
		friend class CFolderInfo;
	public:
		CFileInfo();
		virtual ~CFileInfo();

		//���Ķ���ָ���ļ��ļ�
		virtual bool SetFileName(TCHAR* pFileName);
		bool SetFindData(const TCHAR* pPath, PWIN32_FIND_DATA pData);
		//��Ŀ¼
		CVirtualFile* GetFolder() override;
		//Ŀ¼�µ��ļ�
		CLdArray<CVirtualFile*>* GetFiles() override { return nullptr; };
		//�ļ�����-----------------------------------------------------------------------------
		TCHAR* GetFullName() override;
		TCHAR* GetFileName() override;
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
		} *PFILE_ATTRIBUTE_DATA;

		FILE_ATTRIBUTE_DATA m_AttributeData;
		CLdString m_FileName;
		CFileInfo* m_Folder;

		void ClearValue();
		bool GetFileBasicInfo();
	};

	class CFolderInfo : public CFileInfo
	{
		friend class CFindFileCallbackImpl;
	public:
		CFolderInfo();
		//ö��Ŀ¼�µ����ļ�
		//if bTree ��ݹ�ö����Ŀ¼�ļ�
		DWORD FindFiles(bool bTree = false);
		//Ŀ¼�µ��ļ�
		CLdArray<CVirtualFile*>* GetFiles() override;
		int AddFile(CFileInfo* pFile);
		//���ļ���
		UINT GetFilesCount() const;
		//
		CFileInfo* Find(TCHAR* pName, bool bSub = false);
		void Sort();

		VF_FILE_TYPE GetFileType() override { return vft_folder; };
	private:
		CLdArray<CVirtualFile*> m_Files;
		int AddFile(PWIN32_FIND_DATA pData);
	};

};