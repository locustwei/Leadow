#include "stdafx.h"
#include "WJMftFileImpl.h"


CWJMftFileImpl::CWJMftFileImpl()//:CIntefaceImpl()
{
	ZeroMemory(&m_FileInfo, sizeof(m_FileInfo));
	m_MatchExp = nullptr;
	m_FileData = nullptr;
};

CWJMftFileImpl::~CWJMftFileImpl()
{
	if (m_FileData)
		delete m_FileData;
};

BOOL  CWJMftFileImpl::IsDir()
{
	return (m_FileInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

PFILETIME  CWJMftFileImpl::CreationTime()
{
	return &m_FileInfo.CreationTime;
}

PFILETIME  CWJMftFileImpl::ChangeTime()
{
	return &m_FileInfo.LastWriteTime;
}

PFILETIME  CWJMftFileImpl::LastWriteTime()
{
	return &m_FileInfo.LastWriteTime;
}

PFILETIME  CWJMftFileImpl::LastAccessTime()
{
	return &m_FileInfo.LastWriteTime;
}

UINT64  CWJMftFileImpl::Size()
{
	return m_FileInfo.DataSize;
}

WCHAR*  CWJMftFileImpl::FileName()
{
	return m_FileInfo.Name;
}

IWJMftFileData*  CWJMftFileImpl::GetDataStream()
{
	return m_FileData;
}

UINT64  CWJMftFileImpl::FileReferences()
{
	return m_FileInfo.FileReferenceNumber;
}

UINT64  CWJMftFileImpl::ParentDircetoryReferences()
{
	return m_FileInfo.DirectoryFileReferenceNumber;
}

//USHORT  CWJMftFileImpl::FileNameLength()
//{
//	return m_FileInfo.NameLength;
//}

PMFT_FILE_DATA  CWJMftFileImpl::GetData(PDWORD len)
{
	if (len)
		*len = sizeof(m_FileInfo);
	return &m_FileInfo;
}

VOID CWJMftFileImpl::SetFileInfo(PMFT_FILE_DATA pFile, DWORD Length)
{
	ZeroMemory(&m_FileInfo, sizeof(MFT_FILE_DATA));
	MoveMemory(&m_FileInfo, pFile, Length);
}

UINT CWJMftFileImpl::FileAttributes()
{
	return m_FileInfo.FileAttributes;
}

const TCHAR* CWJMftFileImpl::MatchExpression()
{
	return m_MatchExp;
}

void CWJMftFileImpl::SetMatchExp(const TCHAR* p)
{
	m_MatchExp = p;
}

void CWJMftFileImpl::SetDataInfo(PFILE_DATA_STREAM stream)
{
	if (!stream)
		return;

	if (!m_FileData)
		m_FileData = new CWJMftFileDataImpl();

	m_FileData->SetDataInfo(stream);
}

CWJMftFileDataImpl::CWJMftFileDataImpl()
{
	ZeroMemory(&m_DataInfo, sizeof(m_DataInfo));
}

CWJMftFileDataImpl::~CWJMftFileDataImpl()
{
}

UINT64 CWJMftFileDataImpl::Size()
{
	return m_DataInfo.Size;
}

PUINT64 CWJMftFileDataImpl::DataClusters()
{
	return m_DataInfo.Lcns;
}

UINT CWJMftFileDataImpl::ClustersCount()
{
	return m_DataInfo.LcnCount;
}

ULONG CWJMftFileDataImpl::Offset()
{
	return m_DataInfo.Offset;
}

void CWJMftFileDataImpl::SetDataInfo(PFILE_DATA_STREAM stream)
{
	MoveMemory(&m_DataInfo, stream, sizeof(m_DataInfo));
}
