#pragma once

template <class T >
class CLdList
{
public:
	CLdList()
	{
		InitializeListHead();
	};
	~CLdList()
	{
	};

	int Append(T* p);
	void Remove(T* p);
	void Clear();

protected:
	
	typedef struct LIST_ENTRY
	{
		struct LIST_ENTRY *Flink;
		struct LIST_ENTRY *Blink;
		T* Data;
	}*PLIST_ENTRY;

	struct LIST_ENTRY m_Head;
private:
	void InitializeListHead();
	bool IsListEmpty();
	void RemoveEntryList(PLIST_ENTRY Entry);
	void InsertTailList(PLIST_ENTRY Entry);
	void AppendTailList(PLIST_ENTRY ListHead, PLIST_ENTRY ListToAppend);
	PVOID RemoveHeadList();
};

