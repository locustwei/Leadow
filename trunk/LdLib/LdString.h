#pragma once

class CLdString
{
public:

	CLdString();
	CLdString(const TCHAR ch);
	CLdString(const CLdString& src);
	CLdString(LPCTSTR lpsz);
	CLdString(UINT ccSize);

	~CLdString();
	CLdString ToString();

	void Empty();
	int GetLength() const;
	bool IsEmpty() const;
	TCHAR GetAt(int nIndex) const;
	void Append(LPCTSTR pstr);
	void Assign(LPCTSTR pstr, int nLength = -1);
	TCHAR* GetData() const;
	VOID CopyTo(TCHAR* pStr);

	void SetAt(int nIndex, TCHAR ch);
	operator LPCTSTR() const;
	operator LPTSTR() const;

	TCHAR operator[] (int nIndex) const;
	const CLdString& operator=(const CLdString& src);
	const CLdString& operator=(const TCHAR ch);
	const CLdString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
	const CLdString& CLdString::operator=(LPCSTR lpStr);
	const CLdString& CLdString::operator+=(LPCSTR lpStr);
#else
	const CLdString& CLdString::operator=(LPCWSTR lpwStr);
	const CLdString& CLdString::operator+=(LPCWSTR lpwStr);
#endif
	CLdString operator+(const CLdString& src) const;
	CLdString operator+(LPCTSTR pstr) const;
	const CLdString& operator+=(const CLdString& src);
	const CLdString& operator+=(LPCTSTR pstr);
	const CLdString& operator+=(const TCHAR ch);

	bool operator == (LPCTSTR str) const;
	bool operator != (LPCTSTR str) const;
	bool operator <= (LPCTSTR str) const;
	bool operator <  (LPCTSTR str) const;
	bool operator >= (LPCTSTR str) const;
	bool operator >  (LPCTSTR str) const;

	int Compare(LPCTSTR pstr) const;
	int CompareNoCase(LPCTSTR pstr) const;

	void MakeUpper();
	void MakeLower();

	CLdString Left(int nLength) const;
	CLdString Mid(int iPos, int nLength = -1) const;
	CLdString Right(int nLength) const;

	int Find(TCHAR ch, int iPos = 0) const;
	int Find(LPCTSTR pstr, int iPos = 0) const;
	int ReverseFind(TCHAR ch) const;
	int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);
	void Delete(int nStart, int nLength);
	void Insert(int nStart, LPCTSTR lpStr);

	int __cdecl Format(LPCTSTR pstrFormat, ...);

protected:
	TCHAR* m_pstr;
};

