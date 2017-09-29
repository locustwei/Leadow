#pragma once

namespace LeadowLib {
	template <typename T >
	__interface ISortCompare
	{
		virtual int ArraySortCompare(T* it1, T* it2) = 0;
	};

	template <typename T >
	__interface IFindCompare
	{
		virtual int ArrayFindCompare(PVOID key, T* it) = 0;
	};

	template <typename T >
	class CLdArray
	{
	public:

#define LDARRAY_MAXSIZE 0x1FFFFFFF

		CLdArray()
		{
			FCount = 0;
			FCapacity = 0;
			FList = NULL;
			m_Sorted = false;
		};

		CLdArray(CLdArray& LSource)
		{
			FCount = 0;
			FCapacity = 0;
			FList = NULL;
			SetCapacity(LSource.FCapacity);
			for (int I = 0; I < LSource.FCount; I++)
				Add(LSource[I]);
			m_Sorted = false;
		}

		virtual ~CLdArray()
		{
			Clear();
		};

		T& Get(int index)
		{
			return FList[index];
		};

		T& operator[] (int index)
		{
			return Get(index);
		};
		operator T*()
		{
			return FList;
		};
		int operator += (T Item)
		{
			return Add(Item);
		}
		int operator -= (T Item)
		{
			return Remove(Item);
		}
		void operator = (CLdArray& LSource)
		{
			Clear();
			SetCapacity(LSource.FCapacity);
			for (int I = 0; I < LSource.FCount; I++)
				Add(LSource[I]);
		};
		void operator &= (CLdArray& LSource)
		{
			for (int I = FCount - 1; I >= 0; I--)
				if (LSource.IndexOf(FList[I]) == -1)
					Delete(I);
		};
		void operator |= (CLdArray& LSource)
		{
			for (int I = 0; I < LSource.FCount; I++)
				if (IndexOf(LSource[I]) == -1)
					Add(LSource[I]);
		};
		void operator -= (CLdArray& LSource)
		{
			for (int I = FCount - 1; I >= 0; I--)
				if (LSource.IndexOf(FList[I]) != -1)
					Delete(I);
		};
		void operator += (CLdArray& LSource)
		{
			SetCapacity(FCapacity + LSource.FCount);
			for (int I = 0; I < LSource.FCount; I++)
				Add(LSource[I]);
		};

		int Add(T Item)
		{
			int Result = FCount;
			if (Result == FCapacity)
				Grow();
			memset(&FList[Result], 0, sizeof(T));
			FList[Result] = Item;
			FCount++;
			m_Sorted = false;
			return Result;
		};
		void Clear()
		{
			SetCount(0);
			SetCapacity(0);
			m_Sorted = false;
		};
		void Delete(int Index)
		{
			if (Index < 0 || Index >= FCount)
				return;
			if (Index < FCount - 1)
				memmove(&FList[Index], &FList[Index + 1], (FCount - Index - 1) * sizeof(T));
			FCount--;
		};
		void Exchange(int Index1, int Index2)
		{
			if (Index1 < 0 || Index1 >= FCount)
				return;
			if (Index2 < 0 || Index2 >= FCount)
				return;
			T Item = FList[Index1];
			FList[Index1] = FList[Index2];
			FList[Index2] = Item;
			m_Sorted = false;
		};
		int IndexOf(T Item)
		{
			int LCount = FCount;
			T* LList = FList;
			for (int Result = 0; Result < LCount; Result++) // new optimizer doesn't use [esp] for Result
				if (LList[Result] == Item)
					return Result;
			return -1;
		};
		void Insert(int Index, T Item)
		{
			if (Index < 0 || Index > FCount)
				return;
			if (FCount == FCapacity)
				Grow();
			if (Index < FCount)
				memmove(&FList[Index + 1], &FList[Index], (FCount - Index) * sizeof(T));
			memset(&FList[Index], 0, sizeof(T));
			FList[Index] = Item;
			FCount++;
		};
		int Remove(T Item)
		{
			int Result = IndexOf(Item);
			if (Result >= 0)
				Delete(Result);
			return Result;
		};
		void SetCount(int NewCount)
		{
			if (NewCount < 0 || NewCount > LDARRAY_MAXSIZE)
				return;
			if (NewCount > FCapacity)
				SetCapacity(NewCount);
			// 		else
			// 			for (int I = FCount - 1; I >= NewCount; I--)
			// 				Delete(I);
			FCount = NewCount;
		};
		int GetCount() const
		{
			return FCount;
		}
		void Sort(ISortCompare<T>* compare)
		{
			qsort_s(FList, GetCount(), sizeof(T*), array_sort_compare, compare);
			m_Sorted = true;
			//qsort_s()
		};

		T* Find(PVOID pKey, IFindCompare<T>* compare)
		{
			UINT count = GetCount();
			if (count == 0)
				return nullptr;

			if (m_Sorted)
				return (T*)bsearch_s(pKey, FList, count, sizeof(T*), array_find_compare, (void*)compare);
			else
			{
				for(UINT i=0; i<count; i++)
				{
					if(compare->ArrayFindCompare(pKey, &FList[i]) == 0)
					{
						return &FList[i];
					}
				}
				return nullptr;
			}
				//return (T*)_lsearch_s(pKey, FList, &count, sizeof(T*), array_find_compare, (void*)compare);
		}

		T Put(int index, T Item)
		{
			T tmp = FList[index];
			FList[index] = Item;
			m_Sorted = false;
			return tmp;
		};
		void SetCapacity(int NewCapacity)
		{
			if (NewCapacity < FCount || NewCapacity > LDARRAY_MAXSIZE)
				return;
			if (NewCapacity != FCapacity)
			{
				FList = (T*)realloc(FList, NewCapacity * sizeof(T));
				if (NewCapacity > FCapacity && FList)
					memset(FList + FCapacity, 0, (NewCapacity - FCapacity) * sizeof(T));
				FCapacity = NewCapacity;
			};
		};
	protected:
		int FCount, FCapacity;
		T* FList;

		void Grow()
		{
			int Delta;
			if (FCapacity > 64)
				Delta = FCapacity / 4;
			else if (FCapacity > 8)
				Delta = 16;
			else
				Delta = 4;
			SetCapacity(FCapacity + Delta);
		};

	private:
		bool m_Sorted;

		static int array_sort_compare(void * context, const void *it1, const void *it2)
		{
			ISortCompare<T>* sc = (ISortCompare<T>*)context;
			return sc->ArraySortCompare((T*)it1, (T*)it2);
		};

		static int array_find_compare(void * context, const void * key, const void * datum)
		{
			IFindCompare<T>* sc = (IFindCompare<T>*)context;
			int result = sc->ArrayFindCompare((PVOID)key, (T*)datum);
			return result;
		};
	};

};