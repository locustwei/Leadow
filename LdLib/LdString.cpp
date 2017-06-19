#include "stdafx.h"
#include "LdString.h"

#define reallocstr(pStr, ccSize) (TCHAR*)realloc(pStr, ccSize * sizeof(TCHAR))

CLdString::CLdString()
{
	m_pstr = NULL;
}


CLdString::~CLdString()
{
	if (m_pstr)
		free(m_pstr);
	m_pstr = NULL;
}

CLdString::CLdString(const TCHAR ch)
{
	m_pstr = reallocstr(NULL, 2);
	m_pstr[0] = ch;
	m_pstr[1] = 0;
}

CLdString::CLdString(LPCTSTR lpsz)
{
	m_pstr = NULL;
	Assign(lpsz);
}

CLdString::CLdString(UINT ccSize)
{
	m_pstr = reallocstr(NULL, ccSize);
	ZeroMemory(m_pstr, ccSize * sizeof(TCHAR));
}

CLdString::CLdString(const CLdString& src)
{
	m_pstr = NULL;
	Assign(src.m_pstr);
}

CLdString CLdString::ToString()
{
	return m_pstr;
}

int CLdString::GetLength() const
{
	if (!m_pstr)
		return 0;
	else
		return (int)_tcslen(m_pstr);
}

CLdString::operator LPCTSTR() const
{
	return m_pstr;
}

CLdString::operator LPTSTR() const
{
	return m_pstr;
}

void CLdString::Append(LPCTSTR pstr)
{
	int nNewLength = GetLength() + (int)_tcslen(pstr);
	m_pstr = reallocstr(m_pstr, (nNewLength + 1));
	_tcscat(m_pstr, pstr);
}

void CLdString::Assign(LPCTSTR pstr, int cchMax)
{
	if (pstr == NULL)
	{
		free(m_pstr);
		m_pstr = NULL;
		return;
	}
	cchMax = (cchMax < 0 ? (int)_tcslen(pstr) : cchMax);
	if (cchMax == 0)
		return;
	if (cchMax > GetLength()) {
		m_pstr = reallocstr(m_pstr, (cchMax + 1));
	}
	_tcsncpy(m_pstr, pstr, cchMax);
	m_pstr[cchMax] = _T('\0');
}

bool CLdString::IsEmpty() const
{
	return m_pstr == NULL || m_pstr[0] == _T('\0');
}

void CLdString::Empty()
{
	if (m_pstr)
		m_pstr[0] = 0;
}

TCHAR* CLdString::GetData() const
{
	return m_pstr;
}

VOID CLdString::CopyTo(TCHAR * pStr)
{
	if(GetLength() > 0)
		_tcscpy(pStr, m_pstr);
}

TCHAR CLdString::GetAt(int nIndex) const
{
	if (!m_pstr)
		return '\0';
	return m_pstr[nIndex];
}

TCHAR CLdString::operator[] (int nIndex) const
{
	if (!m_pstr)
		return '\0';
	return m_pstr[nIndex];
}

const CLdString& CLdString::operator=(const CLdString& src)
{
	Assign(src);
	return *this;
}

const CLdString& CLdString::operator=(LPCTSTR lpStr)
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

#ifdef _UNICODE

const CLdString& CLdString::operator=(LPCSTR lpStr)
{
	if (lpStr)
	{
		int cchStr = (int)strlen(lpStr) + 1;
		LPWSTR pwstr = reallocstr(NULL, cchStr);
		if (pwstr != NULL) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr);
		Assign(pwstr);
	}
	else
	{
		Empty();
	}
	return *this;
}

const CLdString& CLdString::operator+=(LPCSTR lpStr)
{
	if (lpStr)
	{
		int cchStr = (int)strlen(lpStr) + 1;
		LPWSTR pwstr = reallocstr(NULL, cchStr);
		if (pwstr != NULL) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr);
		Append(pwstr);
	}

	return *this;
}

#else

const CLdString& CLdString::operator=(LPCWSTR lpwStr)
{
	if (lpwStr)
	{
		int cchStr = ((int)wcslen(lpwStr) * 2) + 1;
		LPSTR pstr = (LPSTR)_alloca(cchStr);
		if (pstr != NULL) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
		Assign(pstr);
	}
	else
	{
		Empty();
	}

	return *this;
}

const CLdString& CLdString::operator+=(LPCWSTR lpwStr)
{
	if (lpwStr)
	{
		int cchStr = ((int)wcslen(lpwStr) * 2) + 1;
		LPSTR pstr = (LPSTR)_alloca(cchStr);
		if (pstr != NULL) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
		Append(pstr);
	}

	return *this;
}

#endif // _UNICODE

const CLdString& CLdString::operator=(const TCHAR ch)
{
	Empty();
	Assign(&ch, 1);
	return *this;
}

CLdString CLdString::operator+(const CLdString& src) const
{
	CLdString sTemp = *this;
	sTemp.Append(src);
	return sTemp;
}

CLdString CLdString::operator+(LPCTSTR lpStr) const
{
	if (lpStr)
	{
		CLdString sTemp = *this;
		sTemp.Append(lpStr);
		return sTemp;
	}

	return *this;
}

const CLdString& CLdString::operator+=(const CLdString& src)
{
	Append(src);
	return *this;
}

const CLdString& CLdString::operator+=(LPCTSTR lpStr)
{
	if (lpStr)
	{
		Append(lpStr);
	}

	return *this;
}

const CLdString& CLdString::operator+=(const TCHAR ch)
{
	TCHAR str[] = { ch, _T('\0') };
	Append(str);
	return *this;
}

bool CLdString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
bool CLdString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
bool CLdString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
bool CLdString::operator <  (LPCTSTR str) const { return (Compare(str) < 0); };
bool CLdString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
bool CLdString::operator >  (LPCTSTR str) const { return (Compare(str) > 0); };

void CLdString::SetAt(int nIndex, TCHAR ch)
{
	if(m_pstr)
		m_pstr[nIndex] = ch;
}

int CLdString::Compare(LPCTSTR lpsz) const
{
	if (m_pstr == lpsz)
		return 0;
	if (!m_pstr)
		return -1;
	else if (!lpsz)
		return 1;
	return _tcscmp(m_pstr, lpsz);
}

int CLdString::CompareNoCase(LPCTSTR lpsz) const
{
	if (!m_pstr)
		return -1;
	return _tcsicmp(m_pstr, lpsz);
}

void CLdString::MakeUpper()
{
	if (!m_pstr)
		return;
	_tcsupr(m_pstr);
}

void CLdString::MakeLower()
{
	if (!m_pstr)
		return;
	_tcslwr(m_pstr);
}

CLdString CLdString::Left(int iLength) const
{
	if (iLength < 0) iLength = 0;
	if (iLength > GetLength()) iLength = GetLength();
	CLdString result;
	result.Assign(m_pstr, iLength);
	return result;
}

CLdString CLdString::Mid(int iPos, int iLength) const
{
	if (iLength < 0) iLength = GetLength() - iPos;
	if (iPos + iLength > GetLength()) iLength = GetLength() - iPos;
	if (iLength <= 0) return CLdString();
	CLdString result;
	result.Assign(m_pstr + iPos, iLength);
	return result;
}

CLdString CLdString::Right(int iLength) const
{
	int iPos = GetLength() - iLength;
	if (iPos < 0) {
		iPos = 0;
		iLength = GetLength();
	}
	CLdString result;
	result.Assign(m_pstr + iPos, iLength);
	return result;
}

int CLdString::Find(TCHAR ch, int iPos /*= 0*/) const
{
	if (!m_pstr)
		return -1;

	if (iPos != 0 && (iPos < 0 || iPos >= GetLength())) return -1;
	LPCTSTR p = _tcschr(m_pstr + iPos, ch);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int CLdString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
{
	if (!m_pstr)
		return -1;

	if (iPos != 0 && (iPos < 0 || iPos > GetLength())) return -1;
	LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int CLdString::ReverseFind(TCHAR ch) const
{
	if (!m_pstr)
		return -1;

	LPCTSTR p = _tcsrchr(m_pstr, ch);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int CLdString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
{
	if (!m_pstr)
		return -1;

	CLdString sTemp;
	int nCount = 0;
	int iPos = Find(pstrFrom);
	if (iPos < 0) return 0;
	int cchFrom = (int)_tcslen(pstrFrom);
	int cchTo = (int)_tcslen(pstrTo);
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

void CLdString::Delete(int nStart, int nLength)
{
	if (!m_pstr || nStart < 0 || nLength <= 0 || _tcslen(m_pstr) <= nStart)
		return;
	if (nStart + nLength >= _tcslen(m_pstr))
	{
		m_pstr[nStart] = '\0';
		return;
	}
	TCHAR* p = m_pstr + nStart;
	TCHAR* p1 = p + nLength;
	_tcscpy(p, p1);
}

void CLdString::Insert(int nStart, LPCTSTR lpStr)
{
	if (!m_pstr || nStart < 0 || !lpStr)
		return;
	if (nStart >= _tcslen(m_pstr))
		*this += lpStr;
	CLdString s(lpStr);
	s += m_pstr + nStart;
	m_pstr[nStart] = '\0';
	*this += s;
}

int CLdString::Format(LPCTSTR pstrFormat, ...)
{
	LPTSTR szSprintf = NULL;
	va_list argList;
	int nLen;
	va_start(argList, pstrFormat);
	nLen = _vsntprintf(NULL, 0, pstrFormat, argList);
	szSprintf = (TCHAR*)malloc((nLen + 1) * sizeof(TCHAR));
	ZeroMemory(szSprintf, (nLen + 1) * sizeof(TCHAR));
	int iRet = _vsntprintf(szSprintf, nLen + 1, pstrFormat, argList);
	va_end(argList);
	Assign(szSprintf);
	free(szSprintf);
	return iRet;
}
