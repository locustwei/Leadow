#include "stdafx.h"
#include "LdString.h"
#include <stdio.h>

#define reallocstrW(pStr, ccSize) (WCHAR*)realloc(pStr, ccSize * sizeof(WCHAR))
#define reallocstrA(pStr, ccSize) (char*)realloc(pStr, ccSize * sizeof(char))

namespace LeadowLib {

#pragma region  StringW

	CLdStringW::CLdStringW()
	{
		m_pstr = NULL;
	}


	CLdStringW::~CLdStringW()
	{
		if (m_pstr)
			free(m_pstr);
		m_pstr = NULL;
	}

	CLdStringW::CLdStringW(const WCHAR ch)
	{
		m_pstr = reallocstrW(NULL, 2);
		m_pstr[0] = ch;
		m_pstr[1] = 0;
	}

	CLdStringW::CLdStringW(WCHAR* lpsz)
	{
		m_pstr = NULL;
		Assign(lpsz);
	}

	CLdStringW::CLdStringW(char* lpsz)
	{
		m_pstr = nullptr;
		Assign(lpsz);
	}

	void CLdStringW::SetLength(UINT cSize)
	{
		m_pstr = reallocstrW(m_pstr, cSize);
		ZeroMemory(m_pstr, cSize * sizeof(WCHAR));
	}

	CLdStringW::CLdStringW(UINT ccSize)
	{
		m_pstr = NULL;
		SetLength(ccSize);
	}

	CLdStringW::CLdStringW(const CLdStringW& src)
	{
		m_pstr = NULL;
		Assign(src.m_pstr);
	}

	CLdStringW CLdStringW::ToString()
	{
		return m_pstr;
	}

	int CLdStringW::GetLength() const
	{
		if (!m_pstr)
			return 0;
		else
			return (int)wcslen(m_pstr);
	}

	CLdStringW::operator WCHAR*() const
	{
		return m_pstr;
	}

	//CLdStringW::operator LPTSTR() const
	//{
	//	return m_pstr;
	//}

	void CLdStringW::Append(WCHAR* pstr)
	{
		if (pstr == nullptr)
		{
			Assign(pstr);
			return;
		}
		int oLength = GetLength();
		int nNewLength = oLength + (int)wcslen(pstr);
		m_pstr = reallocstrW(m_pstr, (nNewLength + 1));
		wcscpy(m_pstr + oLength, pstr);
	}

	void CLdStringW::Assign(WCHAR* pstr, int cchMax)
	{
		if (pstr == NULL)
		{
			free(m_pstr);
			m_pstr = NULL;
			return;
		}
		cchMax = (cchMax < 0 ? (int)wcslen(pstr) : cchMax);
		if (cchMax == 0)
			return;
		if (cchMax > GetLength()) {
			m_pstr = reallocstrW(m_pstr, (cchMax + 1));
		}
		wcsncpy(m_pstr, pstr, cchMax);
		m_pstr[cchMax] = '\0';
	}

	void CLdStringW::Assign(char* pstr, int nLength)
	{
		SetLength((UINT)strlen(pstr) + 1);
		MultiByteToWideChar(CP_ACP, NULL, pstr, -1, m_pstr, (int)strlen(pstr));
	}

	bool CLdStringW::IsEmpty() const
	{
		return m_pstr == NULL || m_pstr[0] == ('\0');
	}

	void CLdStringW::Empty()
	{
		if (m_pstr)
			m_pstr[0] = 0;
	}

	WCHAR* CLdStringW::GetData() const
	{
		return m_pstr;
	}

	VOID CLdStringW::CopyTo(WCHAR * pStr)
	{
		if (GetLength() > 0)
			wcscpy(pStr, m_pstr);
	}

	void CLdStringW::Trim()
	{
		if (IsEmpty())
			return;
		for (int i = GetLength() - 1; i > 0; i--)
		{
			if (m_pstr[i] == ' ')
			{
				m_pstr[i + 1] = '\0';
			}
			else
				break;
		}
		for (int i = 0; i < GetLength(); i++)
		{
			if (m_pstr[i] != ' ')
			{
				Delete(0, i);
				break;
			}
		}
	}

	WCHAR CLdStringW::GetAt(int nIndex) const
	{
		if (!m_pstr)
			return '\0';
		return m_pstr[nIndex];
	}

	WCHAR CLdStringW::operator[] (int nIndex) const
	{
		if (!m_pstr)
			return '\0';
		return m_pstr[nIndex];
	}

	const CLdStringW& CLdStringW::operator=(const CLdStringW& src)
	{
		Assign(src);
		return *this;
	}

	const CLdStringW& CLdStringW::operator=(WCHAR* lpStr)
	{

		if (lpStr)
		{
			Assign(lpStr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

	const CLdStringW& CLdStringW::operator=(const WCHAR* lpStr)
	{
		return operator=((WCHAR*)lpStr);
	}

	const CLdStringW& CLdStringW::operator=(char* lpStr)
	{
		Assign(lpStr);
		return *this;
	}

	const CLdStringW& CLdStringW::operator=(const char* lpStr)
	{
		return operator=((char*)lpStr);
	}

	const CLdStringW& CLdStringW::operator=(const WCHAR ch)
	{
		Empty();
		Assign((WCHAR*)&ch, 1);
		return *this;
	}

	CLdStringW CLdStringW::operator+(const CLdStringW& src) const
	{
		CLdStringW sTemp = *this;
		sTemp.Append(src);
		return sTemp;
	}

	CLdStringW CLdStringW::operator+(WCHAR* lpStr) const
	{
		if (lpStr)
		{
			CLdStringW sTemp = *this;
			sTemp.Append(lpStr);
			return sTemp;
		}

		return *this;
	}

	const CLdStringW& CLdStringW::operator+=(const CLdStringW& src)
	{
		Append(src);
		return *this;
	}

	const CLdStringW& CLdStringW::operator+=(WCHAR* lpStr)
	{
		if (lpStr)
		{
			Append(lpStr);
		}

		return *this;
	}

	const CLdStringW& CLdStringW::operator+=(const WCHAR ch)
	{
		WCHAR str[] = { ch, ('\0') };
		Append(str);
		return *this;
	}

	bool CLdStringW::operator == (WCHAR* str) const { return (Compare(str) == 0); };
	bool CLdStringW::operator != (WCHAR* str) const { return (Compare(str) != 0); };
	bool CLdStringW::operator <= (WCHAR* str) const { return (Compare(str) <= 0); };
	bool CLdStringW::operator <  (WCHAR* str) const { return (Compare(str) < 0); };
	bool CLdStringW::operator >= (WCHAR* str) const { return (Compare(str) >= 0); };
	bool CLdStringW::operator >  (WCHAR* str) const { return (Compare(str) > 0); };

	void CLdStringW::SetAt(int nIndex, WCHAR ch)
	{
		if (m_pstr)
			m_pstr[nIndex] = ch;
	}

	int CLdStringW::Compare(WCHAR* lpsz) const
	{
		if (m_pstr == lpsz)
			return 0;
		if (!m_pstr)
			return -1;
		else if (!lpsz)
			return 1;
		return wcscmp(m_pstr, lpsz);
	}

	int CLdStringW::CompareNoCase(WCHAR* lpsz) const
	{
		if (!m_pstr)
			return -1;
		return wcsicmp(m_pstr, lpsz);
	}

	void CLdStringW::MakeUpper()
	{
		if (!m_pstr)
			return;
		wcsupr(m_pstr);
	}

	void CLdStringW::MakeLower()
	{
		if (!m_pstr)
			return;
		wcslwr(m_pstr);
	}

	CLdStringW CLdStringW::Left(int iLength) const
	{
		if (iLength < 0) iLength = 0;
		if (iLength > GetLength()) iLength = GetLength();
		CLdStringW result;
		result.Assign(m_pstr, iLength);
		return result;
	}

	CLdStringW CLdStringW::Mid(int iPos, int iLength) const
	{
		if (iLength < 0) iLength = GetLength() - iPos;
		if (iPos + iLength > GetLength()) iLength = GetLength() - iPos;
		if (iLength <= 0) return CLdStringW();
		CLdStringW result;
		result.Assign(m_pstr + iPos, iLength);
		return result;
	}

	CLdStringW CLdStringW::Right(int iLength) const
	{
		int iPos = GetLength() - iLength;
		if (iPos < 0) {
			iPos = 0;
			iLength = GetLength();
		}
		CLdStringW result;
		result.Assign(m_pstr + iPos, iLength);
		return result;
	}

	int CLdStringW::Find(WCHAR ch, int iPos /*= 0*/) const
	{
		if (!m_pstr)
			return -1;

		if (iPos != 0 && (iPos < 0 || iPos >= GetLength())) return -1;
		WCHAR* p = wcschr(m_pstr + iPos, ch);
		if (p == NULL) return -1;
		return (int)(p - m_pstr);
	}

	int CLdStringW::Find(WCHAR* pstrSub, int iPos /*= 0*/) const
	{
		if (!m_pstr)
			return -1;

		if (iPos != 0 && (iPos < 0 || iPos > GetLength())) return -1;
		WCHAR* p = wcsstr(m_pstr + iPos, pstrSub);
		if (p == NULL) return -1;
		return (int)(p - m_pstr);
	}

	int CLdStringW::ReverseFind(WCHAR ch) const
	{
		if (!m_pstr)
			return -1;

		WCHAR* p = wcsrchr(m_pstr, ch);
		if (p == NULL) return -1;
		return (int)(p - m_pstr);
	}

	int CLdStringW::Replace(WCHAR* pstrFrom, WCHAR* pstrTo)
	{
		if (!m_pstr || !pstrFrom || pstrFrom[0] == '\0')
			return -1;

		CLdStringW sTemp;
		int nCount = 0;
		int iPos = Find(pstrFrom);
		if (iPos < 0) return 0;
		int cchFrom = (int)wcslen(pstrFrom);
		int cchTo = 0;
		if (pstrTo)
			cchTo = (int)wcslen(pstrTo);
		while (iPos >= 0) {
			sTemp = Left(iPos);
			if(pstrTo)
				sTemp += pstrTo;
			sTemp += Mid(iPos + cchFrom);
			Assign(sTemp);
			iPos = Find(pstrFrom, iPos + cchTo);
			nCount++;
		}
		return nCount;
	}

	void CLdStringW::Delete(int nStart, int nLength)
	{
		if (!m_pstr || nStart < 0 || nLength <= 0 || wcslen(m_pstr) <= (size_t)nStart)
			return;
		if (nStart + nLength >= (int)wcslen(m_pstr))
		{
			m_pstr[nStart] = '\0';
			return;
		}
		WCHAR* p = m_pstr + nStart;
		WCHAR* p1 = p + nLength;
		wcscpy(p, p1);
	}

	void CLdStringW::Insert(int nStart, WCHAR* lpStr)
	{
		if (!m_pstr && nStart == 0)
		{
			Assign(lpStr);
			return;
		}
		if (!m_pstr || nStart < 0 || !lpStr)
			return;

		if (nStart >= (int)wcslen(m_pstr))
		{
			Append(lpStr);
			return;
		}

		CLdStringW s(lpStr);
		s += m_pstr + nStart;
		m_pstr[nStart] = '\0';
		*this += s;
	}

	int CLdStringW::Format(WCHAR* pstrFormat, ...)
	{
		WCHAR* szSprintf;
		va_list argList;
		int nLen;
		va_start(argList, pstrFormat);
		nLen = _vsnwprintf(NULL, 0, pstrFormat, argList);
		szSprintf = (WCHAR*)malloc((nLen + 1) * sizeof(WCHAR));
		ZeroMemory(szSprintf, (nLen + 1) * sizeof(WCHAR));
		int iRet = _vsnwprintf(szSprintf, nLen + 1, pstrFormat, argList);
		va_end(argList);
		Assign(szSprintf);
		free(szSprintf);
		return iRet;
	}

	int CLdStringW::Try2Int(WCHAR* lpStr, int Default)
	{
		if (!lpStr)
			return Default;
		else
		{
			int result;// = _wtoi(pstr);
			if (swscanf(lpStr, L"%d", &result) > 0)
				return result;
			else
				return Default;
		}
	}
#pragma endregion  

#pragma region StringA  


	CLdStringA::CLdStringA()
	{
		m_pstr = NULL;
	}


	CLdStringA::~CLdStringA()
	{
		if (m_pstr)
			free(m_pstr);
		m_pstr = NULL;
	}

	CLdStringA::CLdStringA(const char ch)
	{
		m_pstr = reallocstrA(NULL, 2);
		m_pstr[0] = ch;
		m_pstr[1] = 0;
	}

	CLdStringA::CLdStringA(char* lpsz)
	{
		m_pstr = NULL;
		Assign(lpsz);
	}

	CLdStringA::CLdStringA(wchar_t* lpsz)
	{
		m_pstr = nullptr;
		Assign(lpsz);
	}

	void CLdStringA::SetLength(UINT cSize)
	{
		m_pstr = reallocstrA(m_pstr, cSize);
		ZeroMemory(m_pstr, cSize * sizeof(char));
	}

	CLdStringA::CLdStringA(UINT ccSize)
	{
		m_pstr = NULL;
		SetLength(ccSize);
	}

	CLdStringA::CLdStringA(const CLdStringA& src)
	{
		m_pstr = NULL;
		Assign(src.m_pstr);
	}

	CLdStringA CLdStringA::ToString()
	{
		return m_pstr;
	}

	int CLdStringA::GetLength() const
	{
		if (!m_pstr)
			return 0;
		else
			return (int)strlen(m_pstr);
	}

	CLdStringA::operator char*() const
	{
		return m_pstr;
	}

	//CLdStringA::operator LPTSTR() const
	//{
	//	return m_pstr;
	//}

	void CLdStringA::Append(char* pstr)
	{
		if (pstr == nullptr)
		{
			Assign(pstr);
			return;
		}
		int oLength = GetLength();
		int nNewLength = oLength + (int)strlen(pstr);
		m_pstr = reallocstrA(m_pstr, (nNewLength + 1));
		strcpy(m_pstr + oLength, pstr);
	}

	void CLdStringA::Assign(char* pstr, int cchMax)
	{
		if (pstr == NULL)
		{
			free(m_pstr);
			m_pstr = NULL;
			return;
		}
		cchMax = (cchMax < 0 ? (int)strlen(pstr) : cchMax);
		if (cchMax == 0)
			return;
		if (cchMax > GetLength()) {
			m_pstr = reallocstrA(m_pstr, (cchMax + 1));
		}
		strncpy(m_pstr, pstr, cchMax);
		m_pstr[cchMax] = ('\0');
	}

	void CLdStringA::Assign(wchar_t* pstr, int nLength)
	{
		SetLength(((UINT)wcslen(pstr) + 1)*sizeof(wchar_t));
		WideCharToMultiByte(CP_ACP, NULL, pstr, -1, m_pstr, (int)wcslen(pstr) * sizeof(TCHAR), NULL, FALSE);
	}

	bool CLdStringA::IsEmpty() const
	{
		return m_pstr == NULL || m_pstr[0] == ('\0');
	}

	void CLdStringA::Empty()
	{
		if (m_pstr)
			m_pstr[0] = 0;
	}

	char* CLdStringA::GetData() const
	{
		return m_pstr;
	}

	VOID CLdStringA::CopyTo(char * pStr)
	{
		if (GetLength() > 0)
			strcpy(pStr, m_pstr);
	}

	void CLdStringA::Trim()
	{
		if (!m_pstr)
			return;
		for (int i = GetLength() - 1; i > 0; i--)
		{
			if (m_pstr[i] == ' ')
			{
				m_pstr[i + 1] = '\0';
			}
			else
				break;
		}
		for (int i = 0; i < GetLength(); i++)
		{
			if (m_pstr[i] != ' ')
			{
				Delete(0, i);
				break;
			}
		}
	}

	char CLdStringA::GetAt(int nIndex) const
	{
		if (!m_pstr)
			return '\0';
		return m_pstr[nIndex];
	}

	char CLdStringA::operator[] (int nIndex) const
	{
		if (!m_pstr)
			return '\0';
		return m_pstr[nIndex];
	}

	const CLdStringA& CLdStringA::operator=(const CLdStringA& src)
	{
		Assign(src);
		return *this;
	}

	const CLdStringA& CLdStringA::operator=(char* lpStr)
	{

		if (lpStr)
		{
			Assign(lpStr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

	const CLdStringA& CLdStringA::operator=(const char* lpStr)
	{
		return operator=((char*)lpStr);
	}

	const CLdStringA& CLdStringA::operator=(wchar_t* lpStr)
	{
		Assign(lpStr);
		return *this;
	}

	const CLdStringA& CLdStringA::operator=(const wchar_t* lpStr)
	{
		return operator=((wchar_t*)lpStr);
	}

	const CLdStringA& CLdStringA::operator=(const char ch)
	{
		Empty();
		Assign((char*)&ch, 1);
		return *this;
	}

	CLdStringA CLdStringA::operator+(const CLdStringA& src) const
	{
		CLdStringA sTemp = *this;
		sTemp.Append(src);
		return sTemp;
	}

	CLdStringA CLdStringA::operator+(char* lpStr) const
	{
		if (lpStr)
		{
			CLdStringA sTemp = *this;
			sTemp.Append(lpStr);
			return sTemp;
		}

		return *this;
	}

	const CLdStringA& CLdStringA::operator+=(const CLdStringA& src)
	{
		Append(src);
		return *this;
	}

	const CLdStringA& CLdStringA::operator+=(char* lpStr)
	{
		if (lpStr)
		{
			Append(lpStr);
		}

		return *this;
	}

	const CLdStringA& CLdStringA::operator+=(const char ch)
	{
		char str[] = { ch, ('\0') };
		Append(str);
		return *this;
	}

	bool CLdStringA::operator == (char* str) const { return (Compare(str) == 0); };
	bool CLdStringA::operator != (char* str) const { return (Compare(str) != 0); };
	bool CLdStringA::operator <= (char* str) const { return (Compare(str) <= 0); };
	bool CLdStringA::operator <  (char* str) const { return (Compare(str) < 0); };
	bool CLdStringA::operator >= (char* str) const { return (Compare(str) >= 0); };
	bool CLdStringA::operator >  (char* str) const { return (Compare(str) > 0); };

	void CLdStringA::SetAt(int nIndex, char ch)
	{
		if (m_pstr)
			m_pstr[nIndex] = ch;
	}

	int CLdStringA::Compare(char* lpsz) const
	{
		if (m_pstr == lpsz)
			return 0;
		if (!m_pstr)
			return -1;
		else if (!lpsz)
			return 1;
		return strcmp(m_pstr, lpsz);
	}

	int CLdStringA::CompareNoCase(char* lpsz) const
	{
		if (!m_pstr)
			return -1;
		return stricmp(m_pstr, lpsz);
	}

	void CLdStringA::MakeUpper()
	{
		if (!m_pstr)
			return;
		strupr(m_pstr);
	}

	void CLdStringA::MakeLower()
	{
		if (!m_pstr)
			return;
		strlwr(m_pstr);
	}

	CLdStringA CLdStringA::Left(int iLength) const
	{
		if (iLength < 0) iLength = 0;
		if (iLength > GetLength()) iLength = GetLength();
		CLdStringA result;
		result.Assign(m_pstr, iLength);
		return result;
	}

	CLdStringA CLdStringA::Mid(int iPos, int iLength) const
	{
		if (iLength < 0) iLength = GetLength() - iPos;
		if (iPos + iLength > GetLength()) iLength = GetLength() - iPos;
		if (iLength <= 0) return CLdStringA();
		CLdStringA result;
		result.Assign(m_pstr + iPos, iLength);
		return result;
	}

	CLdStringA CLdStringA::Right(int iLength) const
	{
		int iPos = GetLength() - iLength;
		if (iPos < 0) {
			iPos = 0;
			iLength = GetLength();
		}
		CLdStringA result;
		result.Assign(m_pstr + iPos, iLength);
		return result;
	}

	int CLdStringA::Find(char ch, int iPos /*= 0*/) const
	{
		if (!m_pstr)
			return -1;

		if (iPos != 0 && (iPos < 0 || iPos >= GetLength())) return -1;
		char* p = strchr(m_pstr + iPos, ch);
		if (p == NULL) return -1;
		return (int)(p - m_pstr);
	}

	int CLdStringA::Find(char* pstrSub, int iPos /*= 0*/) const
	{
		if (!m_pstr)
			return -1;

		if (iPos != 0 && (iPos < 0 || iPos > GetLength())) return -1;
		char* p = strstr(m_pstr + iPos, pstrSub);
		if (p == NULL) return -1;
		return (int)(p - m_pstr);
	}

	int CLdStringA::ReverseFind(char ch) const
	{
		if (!m_pstr)
			return -1;

		char* p = strrchr(m_pstr, ch);
		if (p == NULL) return -1;
		return (int)(p - m_pstr);
	}

	int CLdStringA::Replace(char* pstrFrom, char* pstrTo)
	{
		if (!m_pstr || !pstrFrom || pstrFrom[0] == '\0')
			return -1;

		CLdStringA sTemp;
		int nCount = 0;
		int iPos = Find(pstrFrom);
		if (iPos < 0) return 0;
		int cchFrom = (int)strlen(pstrFrom);
		int cchTo = 0;
		if (pstrTo)
			cchTo = (int)strlen(pstrTo);
		while (iPos >= 0) {
			sTemp = Left(iPos);
			sTemp += pstrTo;
			sTemp += Mid(iPos + cchFrom);
			Assign(sTemp);
			iPos = Find(pstrFrom, iPos + cchTo);
			nCount++;
		}
		return nCount;
	}

	void CLdStringA::Delete(int nStart, int nLength)
	{
		if (!m_pstr || nStart < 0 || nLength <= 0 || (int)strlen(m_pstr) <= nStart)
			return;
		if (nStart + nLength >= (int)strlen(m_pstr))
		{
			m_pstr[nStart] = '\0';
			return;
		}
		char* p = m_pstr + nStart;
		char* p1 = p + nLength;
		strcpy(p, p1);
	}

	void CLdStringA::Insert(int nStart, char* lpStr)
	{
		if (!m_pstr && nStart == 0)
		{
			Assign(lpStr);
			return;
		}
		if (!m_pstr || nStart < 0 || !lpStr)
			return;
		if (nStart >= (int)strlen(m_pstr))
			*this += lpStr;
		CLdStringA s(lpStr);
		s += m_pstr + nStart;
		m_pstr[nStart] = '\0';
		*this += s;
	}

	int CLdStringA::Format(char* pstrFormat, ...)
	{
		char* szSprintf;
		va_list argList;
		int nLen;
		va_start(argList, pstrFormat);
		nLen = vsnprintf(NULL, 0, pstrFormat, argList);
		szSprintf = (char*)malloc((nLen + 1) * sizeof(char));
		ZeroMemory(szSprintf, (nLen + 1) * sizeof(char));
		int iRet = vsnprintf(szSprintf, nLen + 1, pstrFormat, argList);
		va_end(argList);
		Assign(szSprintf);
		free(szSprintf);
		return iRet;
	}

	int CLdStringA::Try2Int(char* lpStr, int Default)
	{
		if (!lpStr)
			return Default;
		else
		{
			int result;// = _wtoi(pstr);
			if (sscanf(lpStr, "%d", &result) > 0)
				return result;
			else
				return Default;
		}
	}

#pragma endregion   


}