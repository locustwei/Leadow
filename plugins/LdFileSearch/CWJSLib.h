#pragma once
#include "..\MFT\MftReader.h"
#include "..\MFT\MftChangeListener.h"
#include "WJVolume.h"
#include "..\WJDefines.h"
#include "WJMftReader.h"
#include "WJMftIndexFile.h"

class CWJSLib: public IWJLibInterface
{

public:
	CWJSLib();
	~CWJSLib() override;
protected:
	virtual UINT  GetVolumeCount() override;
	virtual IWJVolume* GetVolume(int idx) override;
	virtual WJ_ERROR_CODE SearchVolume(IWJVolume*, IWJMftSearchHandler*) override;
	//virtual WJ_ERROR_CODE SearchIndexFile(IWJMftIndexFile*, IWJMftSearchHandler*) override;
	//virtual WJ_ERROR_CODE ListenFileChaged(IWJVolume*, IWJMftIndexFile*, IWJMftChangeHandler*) override;
	virtual WJ_ERROR_CODE SearchDeletedFile(IWJVolume*, IWJMftSearchHandler*) override;
	//virtual IWJMftIndexFile* CreateIndexFile(IWJVolume*, const TCHAR* Filename, IWJSHandler*) override;
	virtual IWJMftReader * CreateMftReader(IWJVolume *) override;
	virtual WJ_ERROR_CODE VolumeCanReader(IWJVolume*) override;
	virtual IWJMftIndexFile* CreateIndexFile(IWJVolume*, const TCHAR* Filename, IWJSHandler*, BOOL ListenChange) override;
	virtual WJ_ERROR_CODE SearchIndexFile(IWJMftIndexFile*, IWJMftSearchHandler*) override;
private:
	typedef struct VOLUME_ACTOR
	{
		CWJVolume* volume;
		CMftReader* reader;
	}* PVOLUME_ACTOR;

	CLdArray<CWJVolume*> m_Volumes;
	CLdArray<CWJMftReader*> m_VolumeReaders;
	CLdArray<CWJMftIndexFile*> m_VolumeIndexFiles;

	VOID EnumDiskVolumes();	

	// Í¨¹ý IWJLibInterface ¼Ì³Ð
	CWJMftReader* FindReader(IWJVolume * volume);
	CWJMftIndexFile* FindIndexFile(IWJVolume * volume);
};

