#pragma once

template <typename T >
class CTreeNodes;

template <typename T >
class CTreeNode
{
	friend class CTreeNodes<T>;

public:
	CTreeNode(T Data)
	{
		m_Data = Data;
		m_Items = nullptr;
	};

	~CTreeNode();
	operator T()
	{
		return m_Data;
	};
	T& Get()
	{
		return m_Data;
	}
	void Set(T Data)
	{
		m_Data = Data;
	}

	int GetCount();;

	CTreeNodes<T>* GetItems();

	CTreeNode<T>* GetItem(int index);

private:
	T m_Data;
	CTreeNodes<T>* m_Items;
};

template <typename T >
class CTreeNodes: 
	public CLdArray<CTreeNode<T>*>,
	IFindCompare<CTreeNode<T>*>,
	ISortCompare<CTreeNode<T>*>
{
public:
	UINT_PTR Tag;

	CTreeNodes() : CLdArray<CTreeNode<T>*>(),
		Tag(0)
	{
	};

	~CTreeNodes()
	{
		Clear();
	};

	T& GetValue(int index)
	{
		CTreeNode<T>* it = CLdArray::Get(index);
		return it->Get();
	};

	T& operator[] (int index)
	{
		return Get(index);
	};

	int operator += (T Item)
	{
		return Add(Item);
	}

	int operator -= (T Item)
	{
		return Remove(Item);
	}

	int Add(T Item)
	{
		CTreeNode<T>* it = new CTreeNode<T>(Item);
		return CLdArray::Add(it);
	};

	void Clear()
	{
		for (int i = 0; i<GetCount(); i++)
		{
			delete Get(i);
		}
		CLdArray::Clear();
	};

	void Delete(int Index)
	{
		delete Get(i);
		CLdArray::Delete(Index);
	};

	int IndexOf(T Item)
	{
		for (int i = 0; i<GetCount(); i++)
		{
			if(Item == Get(i)->m_Data)
				return i;
		}
		return -1;
	};

	void Insert(int Index, T Item)
	{
		CTreeNode<T>* it = new CTreeNode<T>(Item);
		CLdArray::Insert(Index, it);
	};
	int Remove(T Item)
	{
		int Result = IndexOf(Item);
		if (Result >= 0)
			Delete(Result);
		return Result;
	};

	void Sort(ISortCompare<T>* compare)
	{
		m_SortCompare = compare;
		CLdArray::Sort(this);
	};

	T* Find(PVOID pKey, IFindCompare<T>* compare, bool child = false)
	{
		m_FindCompare = compare;
		CTreeNode<T>** it = CLdArray::Find(pKey, this);
		if (it)
			return &(*it)->m_Data;
		else if(child)
		{
			for(int i=0;i<GetCount();i++)
			{
				CTreeNode<T>* item = CLdArray::Get(i);
				if (item && item->m_Items)
				{
					T* data = item->m_Items->Find(pKey, compare, child);
					if(data)
						return data;
				}
			}
		}
		return nullptr;
	}

	T Put(int index, T Item)
	{
		CTreeNode<T>* it = new CTreeNode<T>(Item);
		it = CLdArray::Put(index, it);
		return it->Get();
	};

protected:

	int ArraySortCompare(CTreeNode<T>** it1, CTreeNode<T>** it2) override
	{
		return m_SortCompare->ArraySortCompare(&(*it1)->m_Data, &(*it2)->m_Data);
	};
	int ArrayFindCompare(PVOID key, CTreeNode<T>** it) override
	{
		return m_FindCompare->ArrayFindCompare(key, &(*it)->m_Data);
	};
private:

	ISortCompare<T>* m_SortCompare;
	IFindCompare<T>* m_FindCompare;
};


template <typename T >
inline CTreeNode<T>::~CTreeNode()
{
	if (m_Items)
	{
		delete m_Items;
		m_Items = nullptr;
	}
}

template <typename T >
inline int CTreeNode<T>::GetCount()
{
	if (!m_Items)
		return 0;
	else
		return m_Items->GetCount();
}

template <typename T >
inline CTreeNodes<T>* CTreeNode<T>::GetItems()
{
	if (!m_Items)
		m_Items = new CTreeNodes<T>();
	return m_Items;
}

template <typename T >
inline CTreeNode<T>* CTreeNode<T>::GetItem(int index)
{
	if (!m_Items)
		return nullptr;
	else
	{
		CTreeNode* node;
		if (m_Items->GetValueAt(index, node))
			return node;
		else
			return nullptr;
	}
}
