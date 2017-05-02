#include "stdafx.h"
#include "ExprotFunctions.h"
#include "..\FileIndexLib\DiskVolume.h"

using namespace  std;

int EntryFunction()
{
	std::vector<CDiskVolume*> volumes;
	ULONG tickt;
	//CWJSLib::EnumDiskVolumes();


	CDiskVolume* volume = new CDiskVolume();

	volume->OpenVolumePath(L"C:");
	if(!volume->LoadDumpFile(L"C:\\")){
	}

	printf("create index.... \n");

	tickt = GetTickCount();
	volume->UpdateMftDump(FALSE);

	printf("used %d seconds  \n", (GetTickCount() - tickt) / 1000);
	//volume->StartThreadUpdateChange();

	tickt = GetTickCount();
	volumes.push_back(volume);

	while(TRUE){
		printf("\n input option; press \"exit\" to quit \n");
		printf("1:search by name eg: abc, *abc, *abc* \n");
		printf("2:search by size \n");
		//printf("3、se \n");
		WCHAR buffer[81] = {0};
		_getws_s(buffer, 80);

		if(_wcsicmp(buffer, L"exit") == 0)
			break;
		ULONGLONG count = 0, found = 0, tmp;
		int m, n;

		switch(StrToInt(buffer)){
		case 1:
			printf("input file name \n");
			_getws_s(buffer, 80);
			tickt = GetTickCount();
			for(ULONG i=0; i<volumes.size(); i++){
				tmp = 0;
				FILE_FILTER filter = {0};
				filter.beginSize = -1;
				filter.endSize = -1;
				filter.pIncludeNames = new PCWSTR[2];
				filter.pIncludeNames[0] = buffer;
				filter.pIncludeNames[1] = NULL;

				volumes[i]->SearchFile(&filter, FALSE);
			}
			break;
		case 2:
			printf("more then  （-1）limit \n");
			_getws_s(buffer, 80);
			m = StrToInt(buffer);
			printf("less then （-1）limit \n");
			_getws_s(buffer, 80);
			n = StrToInt(buffer);

			tickt = GetTickCount();
			for(ULONG i=0; i<volumes.size(); i++){
				tmp = 0;
				FILE_FILTER filter = {0};
				filter.beginSize = m;
				filter.endSize = n;

				volumes[i]->SearchFile(&filter, FALSE);
			}
			break;
		case 3:

			tickt = GetTickCount();
			for(ULONG i=0; i<volumes.size(); i++){
				tmp = 0;
				RECORD_FIELD_CLASS c[] = {RFC_NAME, RFC_NONE};
				ENUM_FILERECORD_PARAM Param = {0};
				Param.sortField = c;
				Param.Filter.onlyFile = TRUE;
				Param.Filter.beginSize = -1;
				Param.Filter.endSize = -1;
				vector<vector<PMFTFILERECORD>*>* result = volumes[i]->FindDuplicateFiles(&Param, FALSE);
				TCHAR directname[MAX_PATH];
				if(result){
					WCHAR fullName[512] = {0};
					printf("找到 %d 组 \n", result->size());
					for(ULONG m=0; m<result->size(); m++){
						vector<PMFTFILERECORD>* item = result->at(m);
						printf("第 %d 组 %d 个------------------------------------  \n", m+1, item->size());
						for(size_t j=0; j<item->size(); j++){
							PMFTFILERECORD p = item->at(j);
							ZeroMemory(directname, MAX_PATH * sizeof(TCHAR));
							volumes[i]->GetFullFileName(p->DirectoryFileReferenceNumber, directname, MAX_PATH);
							printf("%d  %S\\%S \n", p->DataSize, directname, p->Name);
							delete (PUCHAR)p;
						}
						item->clear();
						delete item;
					}
					result->clear();
					delete result;
				}
				//found += volumes[i]->FindDuplicateFiles(&tmp);
				count += tmp;
			}

			break;
		default:
			continue;
		}

	}

	for(ULONG i=0; i<volumes.size(); i++){
		delete volumes[i];
	}
	return 0;
}