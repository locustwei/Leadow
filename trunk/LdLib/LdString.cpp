#include "stdafx.h"
#include "LdString.h"
#include <algorithm>

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

CLdString::CLdString(TCHAR* lpsz)
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

CLdString::operator TCHAR*() const
{
	return m_pstr;
}

//CLdString::operator LPTSTR() const
//{
//	return m_pstr;
//}

void CLdString::Append(TCHAR* pstr)
{
	if (pstr == nullptr)
		return;
	int oLength = GetLength();
	int nNewLength = oLength + (int)_tcslen(pstr);
	m_pstr = reallocstr(m_pstr, (nNewLength + 1));
	_tcscpy(m_pstr+oLength, pstr);
}

void CLdString::Assign(TCHAR* pstr, int cchMax)
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

void CLdString::Trim()
{
	if (!m_pstr)
		return;
	for(int i=GetLength()-1;i>0;i--)
	{
		if (m_pstr[i] == ' ')
		{
			m_pstr[i + 1] = '\0';
			break;
		}
	}
	for(int i=0; i<GetLength(); i++)
	{
		if(m_pstr[i]!=' ')
		{
			Delete(0, i);
			break;
		}
	}
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

const CLdString& CLdString::operator=(TCHAR* lpStr)
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

const CLdString& CLdString::operator=(const TCHAR* lpStr)
{
	return operator=((TCHAR*) lpStr);
}
const CLdString& CLdString::operator=(const TCHAR ch)
{
	Empty();
	Assign((TCHAR*)&ch, 1);
	return *this;
}

CLdString CLdString::operator+(const CLdString& src) const
{
	CLdString sTemp = *this;
	sTemp.Append(src);
	return sTemp;
}

CLdString CLdString::operator+(TCHAR* lpStr) const
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

const CLdString& CLdString::operator+=(TCHAR* lpStr)
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

bool CLdString::operator == (TCHAR* str) const { return (Compare(str) == 0); };
bool CLdString::operator != (TCHAR* str) const { return (Compare(str) != 0); };
bool CLdString::operator <= (TCHAR* str) const { return (Compare(str) <= 0); };
bool CLdString::operator <  (TCHAR* str) const { return (Compare(str) < 0); };
bool CLdString::operator >= (TCHAR* str) const { return (Compare(str) >= 0); };
bool CLdString::operator >  (TCHAR* str) const { return (Compare(str) > 0); };

void CLdString::SetAt(int nIndex, TCHAR ch)
{
	if(m_pstr)
		m_pstr[nIndex] = ch;
}

int CLdString::Compare(TCHAR* lpsz) const
{
	if (m_pstr == lpsz)
		return 0;
	if (!m_pstr)
		return -1;
	else if (!lpsz)
		return 1;
	return _tcscmp(m_pstr, lpsz);
}

int CLdString::CompareNoCase(TCHAR* lpsz) const
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
	TCHAR* p = _tcschr(m_pstr + iPos, ch);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int CLdString::Find(TCHAR* pstrSub, int iPos /*= 0*/) const
{
	if (!m_pstr)
		return -1;

	if (iPos != 0 && (iPos < 0 || iPos > GetLength())) return -1;
	TCHAR* p = _tcsstr(m_pstr + iPos, pstrSub);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int CLdString::ReverseFind(TCHAR ch) const
{
	if (!m_pstr)
		return -1;

	TCHAR* p = _tcsrchr(m_pstr, ch);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int CLdString::Replace(TCHAR* pstrFrom, TCHAR* pstrTo)
{
	if (!m_pstr || !pstrFrom || pstrFrom[0] == '\0')
		return -1;

	CLdString sTemp;
	int nCount = 0;
	int iPos = Find(pstrFrom);
	if (iPos < 0) return 0;
	int cchFrom = (int)_tcslen(pstrFrom);
	int cchTo = 0;
	if(pstrTo)
		cchTo = (int)_tcslen(pstrTo);
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

void CLdString::Insert(int nStart, TCHAR* lpStr)
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

int CLdString::Format(TCHAR* pstrFormat, ...)
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

int CLdString::Try2Int(int Default)
{
	if(IsEmpty())
		return Default;
	else
	try
	{
		return _tstoi(m_pstr);
	}
	catch (...)
	{
		return Default;
	}

}
