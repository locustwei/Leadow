#pragma once

namespace LeadowLib 
{
	class CLdStringA
	{
	public:

		CLdStringA();
		CLdStringA(const char ch);
		CLdStringA(const CLdStringA& src);
		CLdStringA(char* lpsz);
		CLdStringA(const char* lpsz)
		{
			m_pstr = NULL;
			Assign((char*)lpsz);
		};
		CLdStringA(wchar_t* lpsz);
		CLdStringA(const wchar_t* lpsz)
		{
			m_pstr = nullptr;
			Assign((wchar_t*)lpsz);
		};
		CLdStringA(UINT ccSize);

		~CLdStringA();
		CLdStringA ToString();

		void Empty();
		int GetLength() const;
		void SetLength(UINT cSize);
		bool IsEmpty() const;
		char GetAt(int nIndex) const;
		void Append(char* pstr);
		void Assign(char* pstr, int nLength = -1);
		void Assign(wchar_t* pstr, int nLength = -1);
		char* GetData() const;
		VOID CopyTo(char* pStr);
		void Trim();

		void SetAt(int nIndex, char ch);
		operator char*() const;
		//	operator char*() const;

		char operator[] (int nIndex) const;
		const CLdStringA& operator=(const CLdStringA& src);

		const CLdStringA& operator=(const char ch);
		const CLdStringA& operator=(char* lpStr);
		const CLdStringA& operator=(const char* lpStr);
		const CLdStringA& operator=(wchar_t* lpStr);
		const CLdStringA& operator=(const wchar_t* lpStr);

		CLdStringA operator+(const CLdStringA& src) const;
		CLdStringA operator+(char* pstr) const;
		const CLdStringA& operator+=(const CLdStringA& src);
		const CLdStringA& operator+=(char* pstr);
		const CLdStringA& operator+=(const char ch);

		bool operator == (char* str) const;
		bool operator == (const char* str) const
		{
			return *this == (char*)str;
		};
		bool operator != (char* str) const;
		bool operator <= (char* str) const;
		bool operator <  (char* str) const;
		bool operator >= (char* str) const;
		bool operator >  (char* str) const;

		int Compare(char* pstr) const;
		int CompareNoCase(char* pstr) const;

		void MakeUpper();
		void MakeLower();

		CLdStringA Left(int nLength) const;
		CLdStringA Mid(int iPos, int nLength = -1) const;
		CLdStringA Right(int nLength) const;

		int Find(char ch, int iPos = 0) const;
		int Find(char* pstr, int iPos = 0) const;
		int ReverseFind(char ch) const;
		int Replace(char* pstrFrom, char* pstrTo);
		void Delete(int nStart, int nLength);
		void Insert(int nStart, char* lpStr);

		int __cdecl Format(char* pstrFormat, ...);

		static int Try2Int(char* lpStr, int Default = 0);
	protected:
		char* m_pstr;
	};

	class CLdStringW
	{
	public:

		CLdStringW();
		CLdStringW(const WCHAR ch);
		CLdStringW(const CLdStringW& src);
		CLdStringW(WCHAR* lpsz);
		CLdStringW(const WCHAR* lpsz)
		{
			m_pstr = nullptr;
			Assign((WCHAR*)lpsz);
		};
		CLdStringW(char* lpsz);
		CLdStringW(const char* lpsz)
		{
			m_pstr = nullptr;
			Assign((char*)lpsz);
		};

		CLdStringW(UINT ccSize);

		~CLdStringW();
		CLdStringW ToString();

		void Empty();
		int GetLength() const;
		void SetLength(UINT cSize);
		bool IsEmpty() const;
		WCHAR GetAt(int nIndex) const;
		void Append(WCHAR* pstr);
		void Assign(WCHAR* pstr, int nLength = -1);
		void Assign(char* pstr, int nLength = -1);
		WCHAR* GetData() const;
		VOID CopyTo(WCHAR* pStr);
		void Trim();

		void SetAt(int nIndex, WCHAR ch);
		operator WCHAR*() const;
		//	operator WCHAR*() const;

		WCHAR operator[] (int nIndex) const;
		const CLdStringW& operator=(const CLdStringW& src);
		const CLdStringW& operator=(const WCHAR ch);
		const CLdStringW& operator=(WCHAR* lpStr);
		const CLdStringW& operator=(const WCHAR* lpStr);
		const CLdStringW& operator=(char* lpStr);
		const CLdStringW& operator=(const char* lpStr);

		CLdStringW operator+(const CLdStringW& src) const;
		CLdStringW operator+(WCHAR* pstr) const;
		const CLdStringW& operator+=(const CLdStringW& src);
		const CLdStringW& operator+=(WCHAR* pstr);
		const CLdStringW& operator+=(const WCHAR ch);

		bool operator == (WCHAR* str) const;
		bool operator == (const WCHAR* str) const
		{
			return *this == (WCHAR*)str;
		};
		bool operator != (WCHAR* str) const;
		bool operator <= (WCHAR* str) const;
		bool operator <  (WCHAR* str) const;
		bool operator >= (WCHAR* str) const;
		bool operator >  (WCHAR* str) const;

		int Compare(WCHAR* pstr) const;
		int CompareNoCase(WCHAR* pstr) const;

		void MakeUpper();
		void MakeLower();

		CLdStringW Left(int nLength) const;
		CLdStringW Mid(int iPos, int nLength = -1) const;
		CLdStringW Right(int nLength) const;

		int Find(WCHAR ch, int iPos = 0) const;
		int Find(WCHAR* pstr, int iPos = 0) const;
		int ReverseFind(WCHAR ch) const;
		int Replace(WCHAR* pstrFrom, WCHAR* pstrTo);
		void Delete(int nStart, int nLength);
		void Insert(int nStart, WCHAR* lpStr);

		int __cdecl Format(WCHAR* pstrFormat, ...);

		static int Try2Int(WCHAR* lpStr, int Default = 0);
	protected:
		WCHAR* m_pstr;
	};

#ifdef UNICODE
#define CLdString CLdStringW
#else
#define CLdString CLdStringA
#endif
};