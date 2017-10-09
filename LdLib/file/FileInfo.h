#pragma once

/*
文件、文件夹信息读取

*/

#include "../classes/ldstring.h"
#include "../classes/LdArray.h"

namespace LeadowLib {
	//文件对象类型
	enum VF_FILE_TYPE
	{
		vft_error,     
		vft_adstream,  //文件交换数据量（NTFS支持）
		vft_file,      //文件
		vft_folder,    //文件夹
		vft_volume     //磁盘分区
	};

	//文件对象基类
	class CVirtualFile
	{
	public:
		CVirtualFile():m_FileName()
		{
			m_Tag = 0;
			m_Folder = nullptr;
		};
		//含路径文件名
		virtual TCHAR* GetFullName();   
		//不含路径文件名
		virtual TCHAR* GetFileName();

		virtual INT64 GetDataSize() = 0;
		virtual DWORD GetAttributes() = 0;
		virtual VF_FILE_TYPE GetFileType() = 0;
		//virtual CLdArray<CVirtualFile*>* GetFiles() = 0;
		//所属文件夹
		virtual CVirtualFile* GetFolder() { return m_Folder; };
		//
		UINT_PTR GetTag() { return m_Tag; };
		void SetTag(UINT_PTR Tag) { m_Tag = Tag; };
	protected:
		UINT_PTR m_Tag;
		CLdString m_FileName;
		CVirtualFile* m_Folder;
	};
	//文件交换数据流
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
	//文件
	class CFileInfo : public CVirtualFile
	{
		friend class CFindFileCallbackImpl;
		friend class CFolderInfo;
	public:
		CFileInfo();
		virtual ~CFileInfo();

		//更改对象指向文件名
		virtual bool SetFileName(TCHAR* pFileName);
		bool SetFindData(const TCHAR* pPath, PWIN32_FIND_DATA pData);
		//ADS文件交换数据流
		CLdArray<CVirtualFile*>* GetADStreams();

		//文件属性-----------------------------------------------------------------------------
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
			DWORD nFileCount;        //子文件数（对文件夹包含所以文件夹）
			DWORD nFolderCount;      //子文件夹数（同上）
		} *PFILE_ATTRIBUTE_DATA;

		FILE_ATTRIBUTE_DATA m_AttributeData;

		void ClearValue();
		bool GetFileBasicInfo();
	private:

		CLdArray<CVirtualFile*>* m_Streams;
	};
	//文件夹
	class CFolderInfo : public CFileInfo
	{
		friend class CFindFileCallbackImpl;
	public:
		CFolderInfo();
		//枚举目录下的子文件
		//if bTree 则递归枚举子目录文件
		DWORD FindFiles(bool bTree = false);
		//目录下的文件
		CLdArray<CVirtualFile*>* GetFiles();
		int AddFile(CFileInfo* pFile);
		//子文件数（bTree包含子文件夹）
		UINT GetFilesCount(bool bTree = false) const;
		//查找文件，bSub:在子文件中查找，bPath：比较路径
		CFileInfo* Find(TCHAR* pName, bool bSub = false, bool bPath = false);
		void Sort();
		VF_FILE_TYPE GetFileType() override { return vft_folder; };
	private:
		CLdArray<CVirtualFile*> m_Files;
		int AddFile(PWIN32_FIND_DATA pData);
	};

};