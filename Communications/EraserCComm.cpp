#include "stdafx.h"
#include  "EraserCComm.h"

CEraserCComm::CEraserCComm()
	//:m_socket()
	//,m_Abort(false)
{
	
}

CEraserCComm::~CEraserCComm()
{

}

BOOL CEraserCComm::Connect()
{
	if (CLdProcessCommunication::Connect() != 0)
		return false;
	if (!SendData(LFI_EARSE_FILE, nullptr, 0))
		return false;
	int len = Recv();
	if (len < sizeof(COMMUINCATION_DATA))
		return false;
	PCOMMUINCATION_DATA precv = (PCOMMUINCATION_DATA)GetRecvData()->data;
	if (precv->fId != LFI_EARSE_FILE)
		return false;
	return StartSelectThread();
}
