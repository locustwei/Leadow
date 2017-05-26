#pragma once

#include "stdafx.h"

template <typename T >
class CLdArray
{
public:
	CLdArray()
	{
		FCount = 0;
		FCapacity = 0;
		FList = NULL;
	};

	~CLdArray()
	{
		Clear();
	};

	T operator[] (int Index)
	{
		return Get(Index);
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
		FList[Result] = Item;
		FCount++;
		return Result;
	};
	void Clear()
	{
		SetCount(0);
		SetCapacity(0);
	};
	T Delete(int Index)
	{
		if (Index < 0 || Index >= FCount)
			return NULL;
		T Temp = Get(Index);
		FCount--;
		memmove(&FList[Index], &FList[Index + 1], (FCount - Index) * sizeof(T));
		return Temp;
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
		if (NewCount < 0 || NewCount > 0xFFFFFF)
			return;
		if (NewCount > FCapacity)
			SetCapacity(NewCount);
		if (NewCount > FCount)
			memset(&FList[FCount], (NewCount - FCount) * sizeof(T), 0);
		else
			for (int I = FCount - 1; I >= NewCount; I--)
				Delete(I);
		FCount = NewCount;
	};
	int GetCount()
	{
		return FCount;
	}
	void Sort()
	{
		//qsort_s()
	};
	

protected:
	T Get(int Index)
	{
		if (FList && Index >= 0 && Index < FCount)
			return FList[Index];
		else
			return NULL;
	};

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

	void SetCapacity(int NewCapacity)
	{
		if (NewCapacity < FCount || NewCapacity > 0xFFFFFF)
			return;
		if (NewCapacity != FCapacity)
		{
			FList = (T*)realloc(FList, NewCapacity * sizeof(T));
			FCapacity = NewCapacity;
		};
	};

private:
	int FCount, FCapacity;
	T* FList;
};

 

