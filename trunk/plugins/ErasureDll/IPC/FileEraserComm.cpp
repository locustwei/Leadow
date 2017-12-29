#include "stdafx.h"
#include "FileEraserComm.h"

#define SHAREDATA_NAME _T("LD_FILE_ERASER_SHARE")
#define SHAREDATA_SIZE 1024

CFileEraserComm::CFileEraserComm()
	: m_Data(SHAREDATA_NAME, SHAREDATA_SIZE)
{

}
