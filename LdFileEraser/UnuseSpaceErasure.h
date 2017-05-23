#pragma once

#include "ErasureMethod.h"

class IErasureCallback
{

};

class CUnuseSpaceErasure
{
public:
	CUnuseSpaceErasure();
	~CUnuseSpaceErasure();

	DWORD Execute(CLdString& Driver, CErasureMethod& method, IErasureCallback* callback);
};

