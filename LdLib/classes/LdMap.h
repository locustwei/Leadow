#pragma once

#include "LdArray.h"
#include "LdString.h"

namespace LeadowLib {

	template <typename T1, typename T2>
	class CLdMap
	{
	public:
		CLdMap() :m_ArrayKey(),m_ArrayValue()
		{
		};
		~CLdMap()
		{
			Clear();
		};

		void Clear()
		{
			m_ArrayKey.Clear();
			m_ArrayValue.Clear();
		};

		CLdArray<T1>* GetKeyArrary()
		{
			return &m_ArrayKey; 
		};

		CLdArray<T2>* GetValueArray()
		{
			return &m_ArrayValue;
		};

		T2* Find(T1 key, int* pIndex = NULL)
		{
			int k = m_ArrayKey.IndexOf(key);
			if (k == -1)
				return nullptr;
			else
			{
				if (pIndex)
					*pIndex = k;
				return &m_ArrayValue[k];
			}
		};

		int Put(T1 key, T2 value, bool replease = false)
		{
			int index = m_ArrayKey.IndexOf(key);

			if (index != -1)
			{
				if (!replease)
					return -1;
				m_ArrayValue.Put(index, value);
			}else
			{
				m_ArrayKey.Add(key);
				m_ArrayValue.Add(value);
			}

			return index;
		};

		bool Remove(T1 key)
		{
			int index = m_ArrayKey.IndexOf(key);
			if (index != -1)
			{
				m_ArrayKey.Delete(index);
				m_ArrayValue.Delete(index);
				return true;
			}
			else
				return false;
		};

		int GetCount()
		{
			return m_ArrayKey.GetCount();
		};

		T1* GetAt(int index, T2** pValue = NULL)
		{
			if (index < 0 || index >= m_ArrayKey.GetCount())
				return nullptr;

			if (pValue)
				*pValue = &m_ArrayValue[index];
			return &m_ArrayKey[index];
		};

		T2* operator[](T1 key)
		{
			return Find(key, NULL);
		}

		int IndexOfKey(T1 key)
		{
			return m_ArrayKey.IndexOf(key);
		};

		int IndexOfValue(T2 value)
		{
			return m_ArrayValue.IndexOf(value);
		};

		T1& KeyOf(int index)
		{
			return m_ArrayKey[index];
		};

		T2& ValueOf(int index)
		{
			return m_ArrayValue[index];
		};

	protected:
//		struct MAP_ITEM
//		{
//			T1 key;
//			T2 value;
//		};

		CLdArray<T1> m_ArrayKey;
		CLdArray<T2> m_ArrayValue;
	};

	template <typename T>
	class CLdHashMap
	{
	public:
		static UINT HashKey(TCHAR* Key)
		{
			UINT i = 0;
			SIZE_T len = _tcslen(Key);
			while (len-- > 0) i = (i << 5) + i + Key[len];
			return i;
		};

		static UINT HashKey(const CLdString& Key)
		{
			return HashKey((TCHAR*)Key);
		};

		CLdHashMap(int nSize) : m_nCount(0)
		{
			if (nSize < 16) nSize = 16;
			m_nBuckets = nSize;
			m_aT = new HASHMAP_TITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(HASHMAP_TITEM*));
		}

		~CLdHashMap()
		{
			Resize(0);
		};

		BOOL GetValueAt(int iIndex, T& value)
		{
			if (m_nBuckets == 0 || GetCount() == 0) return false;

			int pos = 0;
			int len = m_nBuckets;
			while (len--) {
				for (HASHMAP_TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext) {
					if (pos++ == iIndex) {
						value = pItem->Data;
						return true;
					}
				}
			}

			return false;
		};

		void RemoveAll()
		{
			this->Resize(m_nBuckets);
		};

		void Resize(int nSize)
		{
			if (m_aT)
			{
				int len = m_nBuckets;
				while (len--) {
					HASHMAP_TITEM* pItem = m_aT[len];
					while (pItem) {
						HASHMAP_TITEM* pKill = pItem;
						pItem = pItem->pNext;
						delete pKill->Key;
						delete pKill;
					}
				}
				delete[] m_aT;
				m_aT = NULL;
			}

			if (nSize < 0) nSize = 0;
			if (nSize > 0) {
				m_aT = new HASHMAP_TITEM*[nSize];
				memset(m_aT, 0, nSize * sizeof(HASHMAP_TITEM*));
			}
			m_nBuckets = nSize;
			m_nCount = 0;
		};

		bool Find(TCHAR* key, T& value, bool optimize = false) const
		{
			if (m_nBuckets == 0 || GetCount() == 0) return false;

			UINT slot = HashKey(key) % m_nBuckets;
			for (HASHMAP_TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
				if (*pItem->Key == key)
				{
					if (optimize && pItem != m_aT[slot]) {
						if (pItem->pNext) {
							pItem->pNext->pPrev = pItem->pPrev;
						}
						pItem->pPrev->pNext = pItem->pNext;
						pItem->pPrev = NULL;
						pItem->pNext = m_aT[slot];
						pItem->pNext->pPrev = pItem;
						//将item移动至链条头部
						m_aT[slot] = pItem;
					}
					value = pItem->Data;
					return true;
				}
			}

			return false;
		};

		LPVOID Set(TCHAR* key, T pData)
		{
			if (m_nBuckets == 0) return pData;

			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for (HASHMAP_TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
				if (*pItem->Key == key) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}

			Insert(key, pData);

			return NULL;
		};

		bool Remove(TCHAR* key)
		{
			if (m_nBuckets == 0 || GetCount() == 0) return false;

			UINT slot = HashKey(key) % m_nBuckets;
			for (HASHMAP_TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
				if (*pItem->Key == key)
				{
					if (m_aT[slot] == pItem)
						m_aT[slot] = pItem->pNext;

					if (pItem->pNext)
						pItem->pNext->pPrev = pItem->pPrev;
					if (pItem->pPrev)
						pItem->pPrev->pNext = pItem->pNext;

					return true;
				}
			}
			return false;
		};

		int GetCount() const
		{
			return m_nCount;
		};

		TCHAR* GetAt(int iIndex) const
		{
			if (m_nBuckets == 0 || GetCount() == 0) return false;

			int pos = 0;
			int len = m_nBuckets;
			while (len--) {
				for (HASHMAP_TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext) {
					if (pos++ == iIndex) {
						return pItem->Key->GetData();
					}
				}
			}

			return NULL;
		};

		TCHAR* operator[] (int nIndex) const
		{
			return GetAt(nIndex);
		};

		T& operator[] (TCHAR* key)
		{
			T result;
			Find(key, result);
			return result;
		};

	protected:

		struct HASHMAP_TITEM
		{
			CLdString* Key;
			T Data;
			struct HASHMAP_TITEM* pPrev;
			struct HASHMAP_TITEM* pNext;
		};

		HASHMAP_TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;

		bool Insert(TCHAR* key, T pData)
		{
			if (m_nBuckets == 0) return false;
			//if (Find(key)) return false;

			// Add first in bucket
			UINT slot = HashKey(key) % m_nBuckets;
			HASHMAP_TITEM* pItem = (HASHMAP_TITEM*)new BYTE[sizeof(HASHMAP_TITEM)];   //尝试引用已删除的函数
			memset(pItem, 0, sizeof(HASHMAP_TITEM));
			pItem->Key = new CLdString(key);
			pItem->Data = pData;
			pItem->pPrev = NULL;
			pItem->pNext = m_aT[slot];
			if (pItem->pNext)
				pItem->pNext->pPrev = pItem;
			m_aT[slot] = pItem;

			m_nCount++;


			return true;
		};

	};
};