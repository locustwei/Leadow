#include "stdafx.h"
#include "ExportFunction.h"
#include "Impl\CWJSLib.h"

IWJLibInterface* g_Lib = NULL;

WJS_API IWJLibInterface* WJS_CALL WJSOpen() 
{
	if(g_Lib == NULL)
		g_Lib = new CWJSLib();
	return /*dynamic_cast<IWJLibInterface*>*/(g_Lib);
}

WJS_API VOID WJS_CALL WJSClose()
{
	if (g_Lib)
	{
		delete g_Lib;
		g_Lib = NULL;
	}
}

WJS_API UINT64 WJS_CALL WJSDataSize(WJSFILEDATA stream)
{
	return stream->Size();
}
WJS_API PUINT64 WJS_CALL WJSDataClusters(WJSFILEDATA stream)
{
	return stream->DataClusters();
}
WJS_API UINT WJS_CALL WJSDataClustersCount(WJSFILEDATA stream)
{
	return stream->ClustersCount();
}
WJS_API UINT WJS_CALL WJSDataOffset(WJSFILEDATA stream)
{
	return stream->Offset();
}

//interface WJS_API IWJMftFileRecord//: IInterface
//{
WJS_API UINT64 WJS_CALL WJSFileReferences(WJSFILE file)                     //数组所属目录
{
	return file->FileReferences();
}
WJS_API UINT64 WJS_CALL WJSParentDircetoryReferences(WJSFILE file)                     //数组所属目录
{
	return file->ParentDircetoryReferences();
}
WJS_API BOOL WJS_CALL WJSIsDir(WJSFILE file)
{
	return file->IsDir();
}
WJS_API PFILETIME WJS_CALL WJSCreationTime(WJSFILE file)
{
	return file->CreationTime();
}
WJS_API PFILETIME WJS_CALL WJSChangeTime(WJSFILE file)
{
	return file->ChangeTime();
}
WJS_API PFILETIME WJS_CALL WJSLastWriteTime(WJSFILE file)
{
	return file->LastWriteTime();
}
WJS_API PFILETIME WJS_CALL WJSLastAccessTime(WJSFILE file)
{
	return file->LastAccessTime();
}
WJS_API UINT WJS_CALL WJSFileAttributes(WJSFILE file)
{
	return file->FileAttributes();
}
WJS_API UINT64 WJS_CALL WJSFileSize(WJSFILE file)
{
	return file->Size();
}

WJS_API WCHAR* WJS_CALL WJSFileName(WJSFILE file)
{
	return file->FileName();
}

WJS_API WJSFILEDATA WJS_CALL WJSFileDataInfo(WJSFILE file)
{
	return file->GetDataStream();
}

WJS_API const TCHAR* WJS_CALL WJSMatchExpression(WJSFILE file)
{
	return file->MatchExpression();
}

//};

//用于动态库导出接口。
//interface WJS_API IWJVolume
//{
WJS_API HANDLE WJS_CALL WJSOpenHandle(WJSVOLUME volume)
{
	return volume->OpenHandle();
}
WJS_API VOID WJS_CALL WJSCloseHandle(WJSVOLUME volume)
{
	return volume->CloseHandle();
}
WJS_API const TCHAR* WJS_CALL WJSGetVolumeGuid(WJSVOLUME volume)
{
	return volume->GetVolumeGuid();
}
WJS_API const TCHAR* WJS_CALL WJSGetVolumePath(WJSVOLUME volume)
{
	return volume->GetVolumePath();
}
WJS_API const TCHAR* WJS_CALL WJSGetVolumeLabel(WJSVOLUME volume)
{
	return volume->GetVolumeLabel();
}
WJS_API WJFILESYSTEM_TYPE WJS_CALL WJSGetFileSystem(WJSVOLUME volume)
{
	return volume->GetFileSystem();
}
WJS_API MEDIA_TYPE WJS_CALL WJSGetMediaType(WJSVOLUME volume)
{
	return volume->GetMediaType();
}
WJS_API UINT64 WJS_CALL WJSGetTotalSize(WJSVOLUME volume)
{
	return volume->GetTotalSize();
}

WJS_API const TCHAR* WJGetShlDisplayName(WJSVOLUME volume)
{
	return volume->GetShlDisplayName();
}

class CExportHandlerImpl :
	public IWJSHandler
{
public:
	CExportHandlerImpl()
	{
	};

	virtual ~CExportHandlerImpl()
	{
	};

	virtual VOID  OnBegin(PVOID p) override
	{
		callback(CALLBACK_ACTION_BEGIN, nullptr, param);
	}

	virtual VOID  OnEnd(PVOID p) override
	{
		callback(CALLBACK_ACTION_END, nullptr, param);
		delete this;
	}

	virtual VOID  OnError(WJ_ERROR_CODE code, PVOID p) override
	{
		CALLBACK_PARAM c;
		c.code = code;
		callback(CALLBACK_ACTION_ERROR, &c, param);
	}

	virtual BOOL  Stop(PVOID p) override
	{
		return callback(CALLBACK_ACTION_CONTINUE, nullptr, param);
	}

public:
	WJS_CALL_BACK* callback;
	PVOID param;
};

class CExportSearchHandlerImpl :
	public CExportHandlerImpl,
	public IWJMftSearchHandler
{
public:
	CExportSearchHandlerImpl() :
		CExportHandlerImpl()
	{
	};
	~CExportSearchHandlerImpl() override
	{};

	virtual const TCHAR**  NameMatchs() override
	{
		CALLBACK_PARAM c;
		c.NameMatchs = nullptr;
		callback(CALLBACK_ACTION_NAMEMATCH, &c, param);
		return (const TCHAR**)c.NameMatchs;
	}

	virtual const TCHAR**  NameNotMatchs() override
	{
		CALLBACK_PARAM c;
		c.NameNotMatchs = nullptr;
		callback(CALLBACK_ACTION_NAMENOTMATCH, &c, param);
		return (const TCHAR**)c.NameNotMatchs;
	}

	virtual INT64  MinSize() override
	{
		CALLBACK_PARAM c;
		c.MinSize = -1;
		callback(CALLBACK_ACTION_MINSIZE, &c, param);
		return c.MinSize;
	}

	virtual INT64  MaxSize() override
	{
		CALLBACK_PARAM c;
		c.MaxSize = -1;
		callback(CALLBACK_ACTION_MAXSIZE, &c, param);
		return c.MaxSize;
	}

	virtual UCHAR  FileOrDir() override
	{
		CALLBACK_PARAM c;
		c.FileOrDir = 0;
		callback(CALLBACK_ACTION_FILETYPE, &c, param);
		return c.FileOrDir;
	}

	virtual VOID  OnMftFileInfo(IWJMftFileRecord* pFileInfo, TCHAR* path, PVOID) override
	{
		CALLBACK_PARAM c;
		c.File = pFileInfo;
		c.Path = path;
		callback(CALLBACK_ACTION_FILEINFO, &c, param);
	}


	virtual VOID  OnBegin(PVOID Param) override
	{
		CExportHandlerImpl::OnBegin(Param);
	}

	virtual VOID  OnError(WJ_ERROR_CODE code, PVOID Param) override
	{
		CExportHandlerImpl::OnError(code, Param);
	}

	virtual BOOL  Stop(PVOID param) override
	{
		return CExportHandlerImpl::Stop(param);
	}

	virtual VOID  OnEnd(PVOID Param) override
	{
		CExportHandlerImpl::OnEnd(Param);
	}


	//virtual VOID OnMftFileInfo(IWJMftFileRecord* pFileInfo, TCHAR* path, PFILE_DATA_STREAM stream, PVOID) override
	//{
	//	CALLBACK_PARAM c;
	//	c.DelFile = pFileInfo;
	//	c.Path = path;
	//	c.pStream = stream;
	//	callback(CALLBACK_ACTION_FILEINFO, &c, param);
	//}

};

WJS_API WJ_ERROR_CODE WJSEnumMftFiles(WJMFTREADER reader, WJS_CALL_BACK* callback, PVOID Param)
{
	if (reader)
	{
		CExportSearchHandlerImpl* impl = new CExportSearchHandlerImpl();
		impl->param = Param;
		impl->callback = callback;
		return reader->EnumMftFiles(impl, Param);
	}
	else
		return WJERROR_NOT_EXISTS;
}
WJS_API WJ_ERROR_CODE WJSEnumDeleteFiles(WJMFTREADER reader, WJS_CALL_BACK* callback, PVOID Param)
{
	if (reader)
	{
		CExportSearchHandlerImpl* impl = new CExportSearchHandlerImpl();
		impl->param = Param;
		impl->callback = callback;
		return reader->EnumDeleteFiles(impl, Param);
	}
	else
		return WJERROR_NOT_EXISTS;
}
WJS_API IWJMftFileRecord* WJSGetFile(WJMFTREADER reader, UINT64 FileNumber, BOOL OnlyName)
{
	if (reader)
		return reader->GetFile(FileNumber, OnlyName);
	else
		return nullptr;
}
WJS_API BOOL WJSReadSector(WJMFTREADER reader, UINT64 sector, UINT count, PVOID buffer)
{
	if (reader)
		return reader->ReadSector(sector, count, buffer);
	else
		return false;
}
WJS_API BOOL WJSReadCluster(WJMFTREADER reader, UINT64 Cluster, UINT count, PVOID buffer)
{
	if (reader)
		return reader->ReadCluster(Cluster, count, buffer);
	else
		return false;
}

WJS_API USHORT WJSGetBytesPerSector(WJMFTREADER reader)
{
	if (reader)
		return reader->GetBytesPerSector();
	else
		return 0;
}
WJS_API USHORT WJSGetSectorsPerCluster(WJMFTREADER reader)
{
	if (reader)
		return reader->GetSectorsPerCluster();
	else
		return 0;
}
WJS_API UINT64 WJSGetTotalCluster(WJMFTREADER reader)
{
	if (reader)
		return reader->GetTotalCluster();
	else
		return 0;
}

WJS_API UINT WJS_CALL WJSGetVolumeCount()
{
	if (g_Lib)
		return g_Lib->GetVolumeCount();
	else
		return 0;	
}
WJS_API WJSVOLUME WJS_CALL WJSGetVolume(int idx)
{
	if (g_Lib)
		return g_Lib->GetVolume(idx);
	else
		return nullptr;
}

WJS_API WJ_ERROR_CODE WJS_CALL WJSSearchVolume(WJSVOLUME volume, WJS_CALL_BACK* callback, PVOID Param)
{
	if (g_Lib)
	{
		CExportSearchHandlerImpl* impl = new CExportSearchHandlerImpl();
		impl->param = Param;
		impl->callback = callback;
		return g_Lib->SearchVolume(volume, impl);
	}
	else
		return WJERROR_NOT_EXISTS;
}
/*
WJS_API WJ_ERROR_CODE WJS_CALL WJSSearchIndexFile(WJSINDEXFILE file, WJS_CALL_BACK* callback, PVOID Param)
{
	if (g_Lib)
	{
		CExportSearchHandlerImpl* impl = new CExportSearchHandlerImpl();
		impl->param = Param;
		impl->callback = callback;
		return g_Lib->SearchIndexFile(file, impl);
	}
	else
		return WJERROR_NOT_EXISTS;
}

class CExportChangeHandlerImpl :
	public CExportHandlerImpl,
	public IWJMftChangeHandler
{
public:
	CExportChangeHandlerImpl()
	{
	};

	~CExportChangeHandlerImpl()  override
	{};

	virtual BOOL  OnFileChange(WJFILE_CHANGED_REASON reason, IWJMftFileRecord* pFileInfo) override
	{
		CALLBACK_PARAM c;
		c.ChangeFile = pFileInfo;
		c.reason = reason;
		return callback(CALLBACK_ACTION_FILECHANGE, &c, param);
	}

	virtual VOID  OnBegin(PVOID Param) override
	{
		CExportHandlerImpl::OnBegin(Param);
	}

	virtual VOID  OnError(WJ_ERROR_CODE code, PVOID Param) override
	{
		CExportHandlerImpl::OnError(code, Param);
	}

	virtual BOOL  Stop(PVOID Param) override
	{
		return CExportHandlerImpl::Stop(Param);
	}

	virtual VOID  OnEnd(PVOID Param) override
	{
		CExportHandlerImpl::OnEnd(Param);
	}
};

WJS_API WJ_ERROR_CODE WJS_CALL WJSListenFileChaged(WJSVOLUME volume, WJSINDEXFILE idxfile, WJS_CALL_BACK* callback, PVOID Param)
{
	if (g_Lib)
	{
		CExportChangeHandlerImpl* impl = new CExportChangeHandlerImpl();
		impl->param = Param;
		impl->callback = callback;
		return g_Lib->ListenFileChaged(volume, idxfile, impl);
	}
	else
		return WJERROR_NOT_EXISTS;
}
*/
WJS_API WJ_ERROR_CODE WJS_CALL WJSSearchDeletedFile(WJSVOLUME volume, WJS_CALL_BACK* callback, PVOID Param)
{
	if (g_Lib)
	{
		CExportSearchHandlerImpl* impl = new CExportSearchHandlerImpl();
		impl->param = Param;
		impl->callback = callback;
		return g_Lib->SearchDeletedFile(volume, impl);
	}
	else
		return WJERROR_NOT_EXISTS;
}

WJS_API WJMFTREADER WJSCreateMftReader(WJSVOLUME volume)
{
	if (g_Lib)
		return g_Lib->CreateMftReader(volume);
	else
		return nullptr;
}
WJS_API WJ_ERROR_CODE WJVolumeCanReader(WJSVOLUME volume)
{
	if (g_Lib)
		return g_Lib->VolumeCanReader(volume);
	else
		return WJERROR_NOT_IMPL;
}

//WJS_API UINT WJS_CALL WJSGetFileFullname(IWJVolume* volume, UINT64 FileReferenceNumber, TCHAR* OutName, UINT bufflen)
//{
//	CWJVolume* v = dynamic_cast<CWJVolume*>(volume);
//	if (v->GetExtendData())
//	{
//		CWJMftIndexFile* idxfile = (CWJMftIndexFile*)v->GetExtendData();
//		return idxfile->GetFileFullname(FileReferenceNumber, OutName, bufflen);
//	}
//	else
//	{
//		return 0;
//	}
//}