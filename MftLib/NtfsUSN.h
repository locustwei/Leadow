#pragma once
#include "MftChangeListener.h"

class CNtfsUSN : public CMftChangeListener
{
public:
	CNtfsUSN(TCHAR*);
	~CNtfsUSN();

	static BOOL QueryUsnStatus(HANDLE hVolume, PUSN_JOURNAL_DATA outStatus);
	static BOOL CreateUsnJournal(HANDLE hVolume);
	static BOOL DeleteUsnJournal(HANDLE hVolume);
protected:
	USN UpdateFiles() override;
	USN ListenUpdate(HANDLE hStopEvent, IMftChangeListenerHandler*, UINT_PTR param) override;

private:
	HANDLE m_Handle;
};

