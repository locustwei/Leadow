#pragma once

#include "LdArray.h"
#include "LdString.h"

template <typename T1, typename T2>

class CLdMap
{
public:
	CLdMap() :m_Array()
	{
	};
	~CLdMap()
	{
		Clear();
	};

	void Clear()
	{
		m_Array.Clear();
	};

	T2* Find(T1 key, int* pIndex = NULL) 	
	{
		for (int i = 0; i < m_Array.GetCount(); i++)
		{
			if (((MAP_ITEM*)m_Array)[i].key == key)
			{
				if (pIndex)
					*pIndex = i;
				return &((MAP_ITEM*)m_Array)[i].value;
			}
		}
		if (pIndex)
			*pIndex = -1;
		return NULL;
	};

	int Put(T1 key, T2 value, bool replease = false)
	{
		int index = -1;
		Find(key, &index);
		if (index != -1)
		{
			if (!replease)
				return -1;
		}

		MAP_ITEM Item = { key, value };
		if (index != -1)
		{
			m_Array.Put(index, Item);
			return index;
		}
		else
			return m_Array.Add(Item);
	};

	bool Remove(T1 key)
	{
		int index;
		if (Find(key, &index))
		{
			m_Array.Delete(index);
			return true
		}
		else
		{
			return false;
		}

	};

	int GetCount()
	{
		return m_Array.GetCount();
	};

	T1 GetAt(int index, T2* pValue = NULL)
	{
		MAP_ITEM Item = m_Array[index];
		if (pValue)
			*pValue = Item.value;
		return Item.key;
	};

	T2* operator[](T1 key)
	{
		return Find(key, NULL);
	}

	int IndexOfKey(T1 key)
	{
		for (int i = 0; i < m_Array.GetCount(); i++)
		{
			if (((MAP_ITEM*)m_Array)[i].key == key)
			{
				return i;
			}
		}
		return -1;
	}; 

	int IndexOfValue(T2 value)
	{
		for (int i = 0; i < m_Array.GetCount(); i++)
		{
			if (((MAP_ITEM*)m_Array)[i].value == value)
			{
				return i;
			}
		}
		return -1;
	};

protected:
	struct MAP_ITEM
	{
		T1 key;
		T2 value;
	};

	CLdArray<MAP_ITEM> m_Array;
};

template <typename T>
class CLdHashMap 
{
public:

	struct HASHMAP_TITEM
	{
		CLdString Key;
		T Data;
		struct HASHMAP_TITEM* pPrev;
		struct HASHMAP_TITEM* pNext;
	};

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);
		while (len-- > 0) i = (i << 5) + i + Key[len];
		return i;
	};

	static UINT HashKey(const CLdString& Key)
	{
		return HashKey((LPCTSTR)Key);
	};

	CLdHashMap(int nSize) : m_nCount(0)
	{
		if (nSize < 16) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new HASHMAP_TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(HASHMAP_TITEM*));
	};

	~CLdHashMap()
	{
		Resize(0);
	};

	void RemoveAll()
	{
		this->Resize(m_nBuckets);
	};

	void Resize(int nSize)
	{
		if (m_aT) {
			int len = m_nBuckets;
			while (len--) {
				HASHMAP_TITEM* pItem = m_aT[len];
				while (pItem) {
					HASHMAP_TITEM* pKill = pItem;
					pItem = pItem->pNext;
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

	T* Find(LPCTSTR key, bool optimize = false) const
	{
		if (m_nBuckets == 0 || GetSize() == 0) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for (HASHMAP_TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
			if (pItem->Key == key) {
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
				return &pItem->Data;
			}
		}

		return NULL;
	};

	bool Insert(LPCTSTR key, T pData)
	{
		if (m_nBuckets == 0) return false;
		if (Find(key)) return false;

		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		HASHMAP_TITEM* pItem = (HASHMAP_TITEM*)new BYTE[sizeof(HASHMAP_TITEM)];   //尝试引用已删除的函数
		memset(pItem, 0, sizeof(HASHMAP_TITEM));
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	};

	LPVOID Set(LPCTSTR key, T pData)
	{
		if (m_nBuckets == 0) return pData;

		if (GetSize()>0) {
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for (HASHMAP_TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) {
				if (pItem->Key == key) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	};

	bool Remove(LPCTSTR key)
	{
		if (m_nBuckets == 0 || GetSize() == 0) return false;

		UINT slot = HashKey(key) % m_nBuckets;
		HASHMAP_TITEM** ppItem = &m_aT[slot];
		while (*ppItem) {
			if ((*ppItem)->Key == key) {
				HASHMAP_TITEM* pKill = *ppItem;
				*ppItem = (*ppItem)->pNext;
				if (*ppItem)
					(*ppItem)->pPrev = pKill->pPrev;
				delete pKill;
				m_nCount--;
				return true;
			}
			ppItem = &((*ppItem)->pNext);
		}

		return false;
	};

	int GetSize() const
	{
		return m_nCount;
	};

	LPCTSTR GetAt(int iIndex) const
	{
		if (m_nBuckets == 0 || GetSize() == 0) return false;

		int pos = 0;
		int len = m_nBuckets;
		while (len--) {
			for (HASHMAP_TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext) {
				if (pos++ == iIndex) {
					return pItem->Key.GetData();
				}
			}
		}

		return NULL;
	};

	LPCTSTR operator[] (int nIndex) const
	{
		return GetAt(nIndex);
	};

	T* operator[] (LPCTSTR key)
	{
		return Find(key);
	};
protected:
	HASHMAP_TITEM** m_aT;
	int m_nBuckets;
	int m_nCount;
};
