#pragma once

namespace LeadowLib {
	class CLdString
	{
	public:

		CLdString();
		CLdString(const TCHAR ch);
		CLdString(const CLdString& src);
		CLdString(TCHAR* lpsz);
		CLdString(UINT ccSize);

		~CLdString();
		CLdString ToString();

		void Empty();
		int GetLength() const;
		void SetLength(UINT cSize);
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;
		void Append(TCHAR* pstr);
		void Assign(TCHAR* pstr, int nLength = -1);
		TCHAR* GetData() const;
		VOID CopyTo(TCHAR* pStr);
		void Trim();

		void SetAt(int nIndex, TCHAR ch);
		operator TCHAR*() const;
		//	operator TCHAR*() const;

		TCHAR operator[] (int nIndex) const;
		const CLdString& operator=(const CLdString& src);
		const CLdString& operator=(const TCHAR ch);
		const CLdString& operator=(TCHAR* lpStr);
		const CLdString& operator=(const TCHAR* lpStr);
		CLdString operator+(const CLdString& src) const;
		CLdString operator+(TCHAR* pstr) const;
		const CLdString& operator+=(const CLdString& src);
		const CLdString& operator+=(TCHAR* pstr);
		const CLdString& operator+=(const TCHAR ch);

		bool operator == (TCHAR* str) const;
		bool operator == (const TCHAR* str) const
		{
			return *this == (TCHAR*)str;
		};
		bool operator != (TCHAR* str) const;
		bool operator <= (TCHAR* str) const;
		bool operator <  (TCHAR* str) const;
		bool operator >= (TCHAR* str) const;
		bool operator >  (TCHAR* str) const;

		int Compare(TCHAR* pstr) const;
		int CompareNoCase(TCHAR* pstr) const;

		void MakeUpper();
		void MakeLower();

		CLdString Left(int nLength) const;
		CLdString Mid(int iPos, int nLength = -1) const;
		CLdString Right(int nLength) const;

		int Find(TCHAR ch, int iPos = 0) const;
		int Find(TCHAR* pstr, int iPos = 0) const;
		int ReverseFind(TCHAR ch) const;
		int Replace(TCHAR* pstrFrom, TCHAR* pstrTo);
		void Delete(int nStart, int nLength);
		void Insert(int nStart, TCHAR* lpStr);

		int __cdecl Format(TCHAR* pstrFormat, ...);

		int Try2Int(int Default = 0);
	protected:
		TCHAR* m_pstr;
	};

};