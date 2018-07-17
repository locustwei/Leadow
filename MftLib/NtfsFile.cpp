#include "stdafx.h"
#include "ntfs.h"
#include "NtfsFile.h"

template <class T1, class T2> inline T1* Padd(T1* p, T2 n)
{
	return (T1*)((char *)p + n);
}

CNtfsFile::CNtfsFile(CMftReader* reader):CMftFile(reader)
{
	m_Data = nullptr;
	m_DataAttrCount = 0;
	ZeroMemory((PUCHAR)&m_FileInfo, sizeof(m_FileInfo));
	m_BitmapAttribute = nullptr;
}

CNtfsFile::~CNtfsFile()
{
	Clear();
}

VOID CNtfsFile::Clear()
{
	ZeroMemory((PUCHAR)&m_FileInfo, sizeof(m_FileInfo));
	m_FileRecord = nullptr;

	for (UINT i = 0; i < m_DataAttrCount; i++)
		delete m_Data[i];
	
	free(m_Data);

	if (m_BitmapAttribute)
		delete m_BitmapAttribute;
	m_BitmapAttribute = nullptr;
}

VOID CNtfsFile::LoadAttributes(UINT64 FileReferenceNumber, PNTFS_FILE_RECORD_HEADER FileHeader, bool NameOnly)
{
	m_FileRecord = FileHeader;
	m_FileInfo.FileReferenceNumber = FileReferenceNumber;

	PATTRIBUTE attr = NULL;
	ULONG Offset = FileHeader->AttributesOffset;

	for (attr = PATTRIBUTE(Padd(FileHeader, FileHeader->AttributesOffset)); attr->AttributeType; attr = Padd(attr, attr->Length))
	{
		if (attr->AttributeType == -1) break;
		if (attr->Length == 0) break;
		if (attr->Length % 8 != 0)  //8字节对齐；
			attr->Length = (attr->Length / 8 + 1) * 8;

		if (!attr->Nonresident)
		{
			switch (attr->AttributeType)
			{
			case AttributeStandardInformation:
				LoadStandardAttribute(PSTANDARD_INFORMATION(Padd(attr, PRESIDENT_ATTRIBUTE(attr)->ValueOffset)));
				break;
			case AttributeAttributeList:
				if(!NameOnly)
					LoadListAttribute(PATTRIBUTE_LIST(Padd(attr, PRESIDENT_ATTRIBUTE(attr)->ValueOffset)));
				break;
			case AttributeFileName:
				LoadFilenameAttribute(PFILENAME_ATTRIBUTE(Padd(attr, PRESIDENT_ATTRIBUTE(attr)->ValueOffset)));
				break;
			}
		}

		if(attr->AttributeType == AttributeData && attr->NameLength == 0 )
			m_FileInfo.DataSize += attr->Nonresident ? PNONRESIDENT_ATTRIBUTE(attr)->DataSize : PRESIDENT_ATTRIBUTE(attr)->ValueLength;

		if (!NameOnly)
		{
			switch (attr->AttributeType)
			{
			case AttributeData:
				LoadDataAttribute(attr, Offset);
				break;
			case AttributeBitmap:
				m_BitmapAttribute = LoadFileAttribute(attr, Offset);
				break;
			}
		}
		Offset += attr->Length;
	}

	m_FileInfo.FileAttributes &= 0x0001FFFF;

	if (FileHeader->Flags & 02)
		m_FileInfo.FileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
	else
		m_FileInfo.FileAttributes &= (!FILE_ATTRIBUTE_DIRECTORY);

}

PMFT_FILE_DATA CNtfsFile::GetFileData()
{
	return &m_FileInfo;
}

UINT CNtfsFile::GetDataStreamCount()
{
	return m_DataAttrCount;
}

CNtfsFileAttribute * CNtfsFile::GetDataStream(int id)
{
	if (id < 0 || (UINT)id >= m_DataAttrCount)
		return nullptr;
	return m_Data[id];
}

void CNtfsFile::LoadStandardAttribute(PSTANDARD_INFORMATION attr)
{
	m_FileInfo.CreationTime = attr->ChangeTime;
	m_FileInfo.LastWriteTime = attr->LastWriteTime;
	//m_FileInfo.FileAttributes = attr->FileAttributes;
}

void CNtfsFile::LoadFilenameAttribute(PFILENAME_ATTRIBUTE attr)
{
	if (attr->NameType == 2 && m_FileInfo.NameLength > 0)
		return;
	m_FileInfo.DirectoryFileReferenceNumber = attr->DirectoryFileReferenceNumber & 0x0000FFFFFFFFFFFF;
	m_FileInfo.FileAttributes = attr->FileAttributes;
	//m_FileInfo.DataSize = attr->DataSize;
	m_FileInfo.NameLength = attr->NameLength;
	MoveMemory(m_FileInfo.Name, attr->Name, attr->NameLength * sizeof(TCHAR));
	m_FileInfo.Name[m_FileInfo.NameLength] = '\0';
}

void CNtfsFile::LoadListAttribute(PATTRIBUTE_LIST attrList)
{
	PATTRIBUTE_LIST attr = NULL;


	for (attr = PATTRIBUTE_LIST(Padd(attrList, PRESIDENT_ATTRIBUTE(attrList)->ValueOffset)); !attr->AttributeType; attr = (PATTRIBUTE_LIST)Padd(attr, attr->Length))
	{
		if (attr->AttributeType == -1) break;
		if (attr->Length == 0) break;
		if (attr->Length % 8 != 0)  //8字节对齐；
			attr->Length = (attr->Length / 8 + 1) * 8;
		UINT64 FileNumber = attr->FileReferenceNumber & 0x0000FFFFFFFFFFFF;
		if (FileNumber != m_FileInfo.FileReferenceNumber)
		{
			USHORT id = attr->AttributeNumber;
			CNtfsFile* file = dynamic_cast<CNtfsFile*>(m_Reader->GetFile(FileNumber, false));
			for (UINT i = 0; i < file->m_DataAttrCount; i++)
			{
				if (file->m_Data[i]->id == id)
				{
					bool b = false;
					for(UINT j=0; j<m_DataAttrCount; j++)
						if (wcsicmp(m_Data[j]->Name, file->m_Data[i]->Name)==0)
						{
							m_Data[j]->AddLcnBlocks(file->m_Data[i]->Lcn, file->m_Data[i]->LcnBlockCount);
							//m_Data[j]->Lcn += *file->m_Data[i]->Lcn;
							b = true;
							break;
						}
					if (!b)
					{
						AddDataAttribute(file->m_Data[i]);
						//m_Data.Add(file->m_Data[i]);
					}
					break;
				}
			}
		}
	}
}

void CNtfsFile::LoadDataAttribute(PATTRIBUTE attrData, ULONG Offset)
{
	CNtfsFileAttribute* tmp = LoadFileAttribute(attrData, Offset);

	if (tmp->Name)
	{
		AddDataAttribute(tmp);
	}
	else
	{
		AddDataAttribute(tmp, 0);
	}
}

UINT64 CNtfsFile::Vcn2Lcn(UINT64 vcn, PUINT nCount)
{
	if (m_DataAttrCount == 0)
		return 0;
	
	UINT k = 0;
	for (UINT i = 0; i < m_Data[0]->LcnBlockCount; i++)
	{
		if (k + m_Data[0]->Lcn[i].nCount > vcn)
		{
			if (nCount)
			{
				*nCount = k + m_Data[0]->Lcn[i].nCount - vcn;
			}
			return m_Data[0]->Lcn[i].startLcn + vcn - k;
		}
		k += m_Data[0]->Lcn[i].nCount;
	}
	return 0;
}

CNtfsFileAttribute* CNtfsFile::LoadFileAttribute(PATTRIBUTE attrData, ULONG Offset)
{
	CNtfsFileAttribute* tmp = new CNtfsFileAttribute();
	tmp->Offset = Offset;
	if (attrData->NameLength != 0)
	{
		tmp->Name = new TCHAR[attrData->NameLength +1];
		ZeroMemory(tmp->Name, sizeof(WCHAR)*attrData->NameLength + 1);
		wcsncpy(tmp->Name, (TCHAR*)Padd(attrData, attrData->NameOffset), attrData->NameLength);
		//tmp->Name.Assign((TCHAR*)Padd(attrData, attrData->NameOffset), attrData->NameLength);
	}
	else
	{
		//tmp->Name.Empty();
	}
	tmp->Nonresident = attrData->Nonresident;
	tmp->id = attrData->AttributeNumber;

	if (!attrData->Nonresident)
	{
		PRESIDENT_ATTRIBUTE attr = PRESIDENT_ATTRIBUTE(attrData);
		tmp->Size = attr->ValueLength;
		tmp->ValueOffset = attr->ValueOffset;
		tmp->AllocSize = tmp->Size;
	}
	else
	{
		PNONRESIDENT_ATTRIBUTE attr = PNONRESIDENT_ATTRIBUTE(attrData);
		tmp->Size = attr->DataSize;
		PUCHAR runArray = (PUCHAR)Padd(attr, attr->RunArrayOffset);
		int k = 0;
		UINT64 curVcn = attr->LowVcn;
		while (runArray[k] != 0)
		{
			int L = runArray[k] & 0x0F;
			int F = runArray[k] >> 4 & 0x0F;
			if (L == 0 || L > 8)
				break;
			k++;

			UINT Length = 0;
			for (int i = L-1; i >= 0; i--)
			{
				Length = Length << 8;
				Length +=  runArray[k + i]; //长度有没有负数？
			}
			k += L;

			INT64 startVcn = 0;
			if (runArray[k + F - 1] & 0x80)
				startVcn = 0xFFFFFFFFFFFFFFFF;
			for (int i = F-1; i >=0; i--)
			{
				startVcn = startVcn << 8;
				startVcn +=  runArray[k + i];
			}
			curVcn += startVcn;
			k += F;

			tmp->AddLcnBlock(curVcn, Length);
			tmp->AllocSize = attr->AllocatedSize;
		}
	}

	return tmp;
}

void CNtfsFile::AddDataAttribute(CNtfsFileAttribute* DataAttribute, int idx)
{
	m_Data = (CNtfsFileAttribute**)realloc(m_Data, (m_DataAttrCount + 1) * sizeof(CNtfsFileAttribute*));
	if (idx == -1)
		m_Data[m_DataAttrCount] = DataAttribute;
	else
	{
		MoveMemory(m_Data + (idx + 1), m_Data + idx, (m_DataAttrCount - idx) * sizeof(CNtfsFileAttribute*));
		m_Data[idx] = DataAttribute;
	}
	m_DataAttrCount++;
}

CNtfsFileAttribute* CNtfsFile::FindAttribute(ATTRIBUTE_TYPE type)
{
	PATTRIBUTE attr = NULL;
	ULONG Offset = m_FileRecord->AttributesOffset;

	for (attr = PATTRIBUTE(Padd(m_FileRecord, m_FileRecord->AttributesOffset)); !attr->AttributeType; attr = Padd(attr, attr->Length))
	{
		if (attr->AttributeType == -1) break;
		if (attr->Length == 0) break;
		if (attr->Length % 8 != 0)  //8字节对齐；
			attr->Length = (attr->Length / 8 + 1) * 8;

		if (attr->AttributeType == type)
		{
			return LoadFileAttribute(attr, Offset);
		}
		Offset += attr->Length;
	}
	return nullptr;
}

CNtfsFileAttribute* CNtfsFile::GetBitmapAttribute()
{
	return m_BitmapAttribute;
}

PNTFS_FILE_RECORD_HEADER CNtfsFile::GetFileRecord()
{
	return m_FileRecord;
}
