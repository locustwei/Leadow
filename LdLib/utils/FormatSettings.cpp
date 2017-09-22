#include "stdafx.h"
#include "FormatSettings.h"

namespace LeadowLib {

	CFormatSettings::CFormatSettings()
	{
	}


	CFormatSettings::~CFormatSettings()
	{
	}

	CLdString CFormatSettings::GetCurrencyString()
	{
		return GetLocaleStr(LOCALE_SCURRENCY);
	}

	BYTE CFormatSettings::GetCurrencyFormat()
	{
		CLdString s = GetLocaleStr(LOCALE_ICURRENCY);

		return CLdString::Try2Int(s.GetData());
	}

	BYTE CFormatSettings::GetCurrencyDecimals()
	{
		CLdString s = GetLocaleStr(LOCALE_ICURRDIGITS);
		return CLdString::Try2Int(s.GetData());
	}

	TCHAR CFormatSettings::GetDateSeparator()
	{
		CLdString s = GetLocaleStr(LOCALE_SDATE);
		if (s.IsEmpty())
			return '/';
		else
			return s[0];
	}

	TCHAR CFormatSettings::GetTimeSeparator()
	{
		CLdString s = GetLocaleStr(LOCALE_STIME);
		if (s.IsEmpty())
			return ':';
		else
			return s[0];
	}

	TCHAR CFormatSettings::GetListSeparator()
	{
		CLdString s = GetLocaleStr(LOCALE_SLIST);
		if (s.IsEmpty())
			return ',';
		else
			return s[0];
	}

	CLdString CFormatSettings::GetShortDateFormat()
	{
		CLdString s = GetLocaleStr(LOCALE_SSHORTDATE, _T("m/d/yy"));

		return s;
	}

	CLdString CFormatSettings::GetLongDateFormat()
	{
		return GetLocaleStr(LOCALE_SLONGDATE, _T("mm/dd/yyyy"));
	}

	CLdString CFormatSettings::GetTimeAMString()
	{
		return GetLocaleStr(LOCALE_S1159, _T("am"));
	}

	CLdString CFormatSettings::GetTimePMString()
	{
		return GetLocaleStr(LOCALE_S2359, _T("pm"));
	}

	CLdString CFormatSettings::GetShortTimeFormat()
	{
		return GetLocaleStr(LOCALE_STIMEFORMAT, _T("ss:mm:hh"));
	}

	CLdString CFormatSettings::GetLongTimeFormat()
	{
		return GetShortTimeFormat() + _T(" AMPM");
	}

	CLdArray<CLdString*> CFormatSettings::GetShortMonthNames()
	{
		CLdArray<CLdString*> result;
		for (int i = 0; i < 12; i++)
		{
			result.Add(new CLdString(GetLocaleStr(LOCALE_SABBREVMONTHNAME1 + i)));
		}
		return result;
	}

	CLdArray<CLdString*> CFormatSettings::GetLongMonthNames()
	{
		CLdArray<CLdString*> result;
		for (int i = 0; i < 12; i++)
		{
			result.Add(new CLdString(GetLocaleStr(LOCALE_SMONTHNAME1 + i)));
		}
		return result;
	}

	CLdArray<CLdString*> CFormatSettings::GetShortDayNames()
	{
		CLdArray<CLdString*> result;
		for (int i = 0; i < 7; i++)
		{
			result.Add(new CLdString(GetLocaleStr(LOCALE_SABBREVDAYNAME1 + i)));
		}
		return result;
	}

	CLdArray<CLdString*> CFormatSettings::GetLongDayNames()
	{
		CLdArray<CLdString*> result;
		for (int i = 0; i < 7; i++)
		{
			result.Add(new CLdString(GetLocaleStr(LOCALE_SDAYNAME1 + i)));
		}
		return result;
	}

	TCHAR CFormatSettings::GetThousandSeparator()
	{
		CLdString s = GetLocaleStr(LOCALE_STHOUSAND);
		if (s.IsEmpty())
			return ',';
		else
			return s[0];
	}

	TCHAR CFormatSettings::GetDecimalSeparator()
	{
		CLdString s = GetLocaleStr(LOCALE_SDECIMAL);
		if (s.IsEmpty())
			return '.';
		else
			return s[0];
	}

	CLdString CFormatSettings::GetLocaleStr(LCTYPE LocalType, TCHAR * Default, LCID Local)
	{
		CLdString result((UINT)255);
		if (Local == 0)
		{
			Local = GetThreadLocale();
		}
		GetLocaleInfo(Local, LocalType, result.GetData(), 255);
		return result;
	}
}