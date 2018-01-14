#pragma once

#define MD5_CODE_LEN  16

typedef struct _MD5_HANDLDE{
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
}MD5_HANDLDE, *PMD5_HANDLE;

PWSTR Md5Code2Str(BYTE* Md5Code, PWSTR pStr);
DWORD Md5File(LPCWSTR filename, BYTE* Md5Code);
DWORD Md5Buffer(BYTE* buffer, DWORD buferSize, BYTE* Md5Code);
DWORD Md5_Init(OUT PMD5_HANDLE* hMd5);
DWORD Md5_HashData(PMD5_HANDLE hMd5, BYTE* buffer, DWORD bufferSize, BYTE* Md5Code = NULL);
DWORD Md5_Finsh(PMD5_HANDLE hMd5, BYTE* Md5Code);