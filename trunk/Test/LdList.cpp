#include "stdafx.h"
#include "LdList.h"

template<class T>
int CLdList<T>::Append(T * p)
{
	struct LIST_ENTRY* pEntry = new struct LIST_ENTRY;
	pEntry->data = p;
	InsertTailList(pEntry);
	return 0;
}

template<class T>
void CLdList<T>::Remove(T * p)
{
	struct LIST_ENTRY* pEntry;

	for (pEntry = m_Head.Flink; pEntry != &m_Head; pEntry = pEntry->Flink)
	{
		if (pEntry->data == p)
		{
			RemoveEntryList(pEntry);
			delete pEntry;
			break;
		}
	}
}

template<class T>
void CLdList<T>::Clear()
{
	while (!IsListEmpty())
	{
		struct LIST_ENTRY* pEntry = RemoveHeadList();
		delete pEntry;
	}
}

template <class T >
void CLdList<T>::InitializeListHead()
{
	m_Head.Flink = m_Head.Blink = &m_Head;
}

template <class T >
bool CLdList<T>::IsListEmpty()
{
	return m_Head.Flink == &m_Head;
}


template <class T >
void CLdList<T>::RemoveEntryList(PLIST_ENTRY Entry)
{
	struct LIST_ENTRY* Blink;
	struct LIST_ENTRY* Flink;

	Flink = Entry->Flink;
	Blink = Entry->Blink;
	Blink->Flink = Flink;
	Flink->Blink = Blink;
}

template<class T>
void CLdList<T>::InsertTailList(PLIST_ENTRY Entry)
{
	PLIST_ENTRY Blink;

	Blink = m_Head.Blink;
	Entry->Flink = ListHead;
	Entry->Blink = Blink;
	Blink->Flink = Entry;
	m_Head.Blink = Entry;
}

template<class T>
void CLdList<T>::AppendTailList(
	PLIST_ENTRY ListHead,
	PLIST_ENTRY ListToAppend
)
{

	PLIST_ENTRY ListEnd = m_Head.Blink;

	ListHead->Blink->Flink = ListToAppend;
	ListHead->Blink = ListToAppend->Blink;
	ListToAppend->Blink->Flink = ListHead;
	ListToAppend->Blink = ListEnd;
	return;
}

template<class T>
PVOID CLdList<T>::RemoveHeadList()
{
	PLIST_ENTRY Flink;
	PLIST_ENTRY Entry;

	Entry = m_Head.Flink;
	Flink = Entry->Flink;
	ListHead->Flink = Flink;
	Flink->Blink = &m_Head;
	return Entry;
}

