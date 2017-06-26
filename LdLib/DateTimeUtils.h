#pragma once
class CDateTimeUtils
{
public:
	static CLdString DateTimeToString(LARGE_INTEGER& DateTime, TCHAR* Format = NULL);
	static CLdString DateTimeToString(FILETIME& DateTime, TCHAR* Format = NULL);
	static CLdString DateTimeToString(SYSTEMTIME& DateTime, TCHAR* Format = NULL);
};

