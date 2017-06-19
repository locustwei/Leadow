#pragma once

typedef PVOID PListIndex;

template <typename T >
class CLdList
{
public:
	inline CLdList():
		m_Head()
	{
		InitializeListHead();
	};

	inline ~CLdList()
	{
		Clear();
	};

	inline PListIndex Push(T p)
	{
		PLIST_ENTRY pEntry = new struct LIST_ENTRY;
		pEntry->Data = p;
		InsertHeadList(pEntry);
		return (PListIndex)pEntry;
	};

	inline T Pop()
	{
		T Result;
		struct LIST_ENTRY* pEntry = RemoveHeadList();
		if (pEntry)
		{
			Result = pEntry->Data;
			delete pEntry;
		}
		return Result;
	};

	inline PListIndex Append(T p)
	{
		PLIST_ENTRY pEntry = new struct LIST_ENTRY;
		pEntry->Data = p;
		InsertTailList(pEntry);
		return (PListIndex)pEntry;
	};

	inline void Remove(T p)
	{
		struct LIST_ENTRY* pEntry;

		for (pEntry = m_Head.Flink; pEntry != &m_Head; pEntry = pEntry->Flink)
		{
			if (pEntry->Data == p)
			{
				RemoveEntryList(pEntry);
				break;
			}
		}
	};

	inline T Remove(PListIndex index)
	{
		T Result = PLIST_ENTRY(index)->Data;
		RemoveEntryList(pEntry);
		return Result;
	};

	inline void Clear()
	{
		while (!IsListEmpty())
		{
			struct LIST_ENTRY* pEntry = RemoveHeadList();
			delete pEntry;
		}
	};

	inline PListIndex Begin(T* Data)
	{
		if (IsListEmpty())
			return NULL;
		if (Data)
			*Data = m_Head.Blink->Data;
		return (PListIndex)m_Head.Blink;
	}

	inline PListIndex Behind(PListIndex index, T* Data)
	{
		PLIST_ENTRY pEntry = (PLIST_ENTRY)index;
		if (pEntry->Blink == &m_Head)
			return NULL;
		if (Data)
			*Data = pEntry->Blink->Data;
		return (PListIndex)pEntry->Blink;
	}

	inline PListIndex Front(PListIndex index, T* Data)
	{
		PLIST_ENTRY pEntry = index;
		if (pEntry->Flink == &m_Head)
			return NULL;
		if (Data)
			*Data = pEntry->FLink->Data;
		return (PListIndex)pEntry->FLink;
	};

	inline bool IsListEmpty()
	{
		return m_Head.Flink == &m_Head;
	};

	inline int GetCount()
	{
		int result = 0;

		for (PLIST_ENTRY pEntry = m_Head.Flink; pEntry != &m_Head; pEntry = pEntry->Flink)
		{
			result++;
		}

		return result;
	};

	inline T* operator[](PListIndex index)
	{
		for (PLIST_ENTRY pEntry = m_Head.Flink; pEntry != &m_Head; pEntry = pEntry->Flink)
		{
			if(index == pEntry)
				return &pEntry->Data;
		}
		return NULL;
	};

	inline T* operator[](int index)
	{
		int k = 0;
		for (PLIST_ENTRY pEntry = m_Head.Flink; pEntry != &m_Head; pEntry = pEntry->Flink)
		{
			if(k == index)
				return &pEntry->Data;
			k++;
		}
		return NULL;
	};

protected:
	
	typedef struct LIST_ENTRY
	{
		struct LIST_ENTRY *Flink;
		struct LIST_ENTRY *Blink;
		T Data;
	}*PLIST_ENTRY;

private:
	struct LIST_ENTRY m_Head;

	inline void InitializeListHead()
	{
		m_Head.Flink = m_Head.Blink = &m_Head;
	};

	inline void RemoveEntryList(PLIST_ENTRY Entry)
	{
		struct LIST_ENTRY* Blink;
		struct LIST_ENTRY* Flink;

		Flink = Entry->Flink;
		Blink = Entry->Blink;
		Blink->Flink = Flink;
		Flink->Blink = Blink;

		delete Entry;
	};

	inline void InsertHeadList(PLIST_ENTRY Entry)
	{
		PLIST_ENTRY Blink;

		Blink = m_Head.Blink;
		Entry->Flink = &m_Head;
		Entry->Blink = Blink;
		Blink->Flink = Entry;
		m_Head.Blink = Entry;
	};

	inline void InsertTailList(PLIST_ENTRY Entry)
	{
		PLIST_ENTRY Flink;

		Flink = m_Head.Flink;
		Entry->Blink = &m_Head;
		Entry->Flink = Flink;
		Flink->Blink = Entry;
		m_Head.Flink = Entry;
	};

	inline PLIST_ENTRY RemoveHeadList()
	{
		PLIST_ENTRY Blink;
		PLIST_ENTRY Entry;

		Entry = m_Head.Blink;
		Blink = Entry->Blink;
		m_Head.Blink = Blink;
		Blink->Flink = &m_Head;
		return Entry;
	};
};
