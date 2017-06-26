#pragma once
#include "LdString.h"
#include "LdArray.h"

class CFormatSettings
{
public:
	CFormatSettings();
	~CFormatSettings();

	static CLdString GetCurrencyString();
	static BYTE GetCurrencyFormat();
	static BYTE GetCurrencyDecimals();
	static TCHAR GetDateSeparator();
	static TCHAR GetTimeSeparator();
	static TCHAR GetListSeparator();
	static CLdString GetShortDateFormat();
	static CLdString GetLongDateFormat();
	static CLdString GetTimeAMString();
	static CLdString GetTimePMString();
	static CLdString GetShortTimeFormat();
	static CLdString GetLongTimeFormat();
	static CLdArray<CLdString*> GetShortMonthNames();
	static CLdArray<CLdString*> GetLongMonthNames();
	static CLdArray<CLdString*> GetShortDayNames();
	static CLdArray<CLdString*> GetLongDayNames();
	static TCHAR GetThousandSeparator();
	static TCHAR GetDecimalSeparator();
	//WORD GetTwoDigitYearCenturyWindow();
	static BYTE GetNegCurrFormat();

	static CLdString GetLocaleStr(LCTYPE LocalType, TCHAR* Default = _T(""), LCID Local = 0);
};

