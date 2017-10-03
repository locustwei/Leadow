#pragma once
#include "LdProcessCommunication.h"

class CEraserCComm:
	//public IEraserThreadCallback
	public CLdProcessCommunication
{
public:
	CEraserCComm();
	~CEraserCComm();

	BOOL Connect();
protected:

private:
};

