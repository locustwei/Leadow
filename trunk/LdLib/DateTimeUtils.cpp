#include "stdafx.h"
#include "FormatSettings.h"
#include "DateTimeUtils.h"

CLdString CDateTimeUtils::DateTimeToString(LARGE_INTEGER & DateTime, TCHAR * Format)
{
	FILETIME st;
	st.dwHighDateTime = DateTime.HighPart;
	st.dwLowDateTime = DateTime.LowPart;
	return DateTimeToString(st, Format);
}

CLdString CDateTimeUtils::DateTimeToString(FILETIME& DateTime, TCHAR* Format)
{
	SYSTEMTIME st;
	FileTimeToSystemTime(&DateTime, &st);
	return DateTimeToString(st, Format);
}

CLdString TokeString(SYSTEMTIME& DateTime, int token, int tokenLen)
{
	CLdString result;
	switch (token)
	{
	case 0:
		result.Format(_T("%d"), DateTime.wYear);
		break;
	case 1:
		result.Format(_T("%d"), DateTime.wMonth);
		break;
	case 2:
		result.Format(_T("%d"), DateTime.wDay);
		break;
	case 3:
		result.Format(_T("%d"), DateTime.wHour);
		break;
	case 4:
		result.Format(_T("%d"), DateTime.wMinute);
		break;
	case 5:
		result.Format(_T("%d"), DateTime.wSecond);
		break;
	case 6:
		result.Format(_T("%d"), DateTime.wMilliseconds);
		break;
	}
	for (int i = 0; i<tokenLen - result.GetLength(); i++)
		result.Insert(0, _T("0"));
	return result;
}

CLdString CDateTimeUtils::DateTimeToString(SYSTEMTIME& DateTime, TCHAR* Format)
{
	CLdString result;
	CLdString format;
	if (Format == NULL)
		format = CFormatSettings::GetShortDateFormat() + _T(" ") + CFormatSettings::GetShortTimeFormat();
	else
		format = Format;
	
	int token = -1, tokenLen = 0;

	for (int i = 0; i < format.GetLength(); i++)
	{
		switch (format[i])
		{
		case 'y':
		case 'Y':
			if (token != 0 && token != -1 && tokenLen != 0)
			{
				result.Append(TokeString(DateTime, token, tokenLen));
				tokenLen = 0;
			}
			tokenLen++;
			token = 0;
			break;
		case 'M':
			if (token != 1 && token != -1 && tokenLen != 0)
			{
				result.Append(TokeString(DateTime, token, tokenLen));
				tokenLen = 0;
			}
			tokenLen++;
			token = 1;
			break;
		case 'd':
		case 'D':
			if (token != 2 && token != -1 && tokenLen != 0)
			{
				result.Append(TokeString(DateTime, token, tokenLen));
				tokenLen = 0;
			}
			tokenLen++;
			token = 2;
			break;
		case 'h':
		case 'H':
			if (token != 3 && token != -1 && tokenLen != 0)
			{
				result.Append(TokeString(DateTime, token, tokenLen));
				tokenLen = 0;
			}
			tokenLen++;
			token = 3;
			break;
		case 'm':
			if (token != 4 && token != -1 && tokenLen != 0)
			{
				result.Append(TokeString(DateTime, token, tokenLen));
				tokenLen = 0;
			}
			tokenLen++;
			token = 4;
			break;
		case 's':
		case 'S':
			if (token != 5 && token != -1 && tokenLen != 0)
			{
				result.Append(TokeString(DateTime, token, tokenLen));
				tokenLen = 0;
			}
			tokenLen++;
			token = 5;
			break;
		default:
			if (token != -1 && tokenLen != 0)
			{
				result.Append(TokeString(DateTime, token, tokenLen));
			}
			if (format[i] != '\'')
				result += format[i];
			token = -1;
			tokenLen = 0;
			break;
		}
	}
	if (token != -1 && tokenLen != 0)
	{
		result.Append(TokeString(DateTime, token, tokenLen));
	}
	return result;
}