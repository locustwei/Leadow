#pragma once
#include "MftReader.h"

/*!
 * CNtfsDataStreamAttribute
 *
 * Ntfs 数据流属性 
 *
 * author asa-pc
 * date 六月 2018
 */
class CNtfsFileAttribute
{
	friend class CNtfsFile;

public:
	CNtfsFileAttribute() 
	{
		id = 0;
		Size = 0;
		ValueOffset = 0;
		LcnBlockCount = 0;
		Name = nullptr;
		Lcn = nullptr;
	};
	~CNtfsFileAttribute() 
	{
		if (Name)
			delete[] Name;
		if(Lcn)
			free(Lcn);
	};


	bool IsNonresident()
	{
		return Nonresident;
	}
	UINT64 GetSize()
	{
		return Size;
	}
	UINT64 GetAllocSize()
	{
		return AllocSize;
	}
	ULONG GetOffset()
	{
		return Offset;
	}
	USHORT GetValueOffset()
	{
		return ValueOffset;
	}
	int GetBlockCount()
	{
		return LcnBlockCount;
	}

	PLCN_BLOCK GetLcnBlock(UINT idx)
	{
		return &Lcn[idx];
	}
private:

	TCHAR* Name;      //流名称（一个ntfs文件最多有一个未命名的数据流）
	bool Nonresident;
	USHORT id;           //流id（ntfs有可能把一个流当成一个文件记录另外存放，此时用id关联）
	UINT64 Size;         //数据流大小
	UINT64 AllocSize;
	//CLdArray<LCN_BLOCK> Lcn;  //占用的逻辑族号
	PLCN_BLOCK Lcn;
	UINT LcnBlockCount;
	ULONG Offset;             //属性相对于文件记录的偏移位置
	USHORT ValueOffset;       //属性数据相对于属性头的便宜位置

	void AddLcnBlock(UINT64 strartLcn, UINT count)
	{
		LcnBlockCount++;
		Lcn = (LCN_BLOCK*)realloc(Lcn, LcnBlockCount * sizeof(LCN_BLOCK));
		Lcn[LcnBlockCount - 1].startLcn = strartLcn;
		Lcn[LcnBlockCount - 1].nCount = count;
	}

	void AddLcnBlocks(LCN_BLOCK* Lcns, UINT length)
	{
		Lcn = (LCN_BLOCK*)realloc(Lcn, (LcnBlockCount + length) * sizeof(LCN_BLOCK));
		MoveMemory(Lcn + LcnBlockCount, Lcns, length * sizeof(LCN_BLOCK));
		LcnBlockCount += length;
	}

};

/*!
 * CNtfsFile;
 *
 * NTFS 文件记录，包含文件常规属性，数据流属性
 *
 * author asa-pc
 * date 六月 2018
 */
class CNtfsFile : public CMftFile
{
public:
	CNtfsFile(CMftReader* reader);
	~CNtfsFile() override;
	VOID Clear();

	VOID LoadAttributes(UINT64 FileReferenceNumber, PNTFS_FILE_RECORD_HEADER FileHeader, bool NameOnly);

	int GetDataStreamCount();
	CNtfsFileAttribute* GetDataStream(int id = 0);
	UINT64 Vcn2Lcn(UINT64, PUINT);
	//CNtfsFileAttribute* FindAttribute(ATTRIBUTE_TYPE type);
	CNtfsFileAttribute* GetBitmapAttribute();
	PNTFS_FILE_RECORD_HEADER GetFileRecord();
private:
	int m_DataAttrCount;
	CNtfsFileAttribute** m_Data;        //数据流
	PNTFS_FILE_RECORD_HEADER m_FileRecord;
	CNtfsFileAttribute* m_BitmapAttribute;
	void LoadStandardAttribute(PSTANDARD_INFORMATION attr);
	void LoadFilenameAttribute(PFILENAME_ATTRIBUTE attr);
	void LoadListAttribute(PATTRIBUTE_LIST attrList);
	void LoadDataAttribute(PATTRIBUTE attrData, ULONG Offset);
	CNtfsFileAttribute* LoadFileAttribute(PATTRIBUTE attrData, ULONG Offset);
	void AddDataAttribute(CNtfsFileAttribute* DataAttribute, int idx = -1);
};
