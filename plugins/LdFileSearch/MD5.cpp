#include "stdafx.h"
#include "md5.h"

#define BUFSIZE 1024 * 1024 / 10

TCHAR* Md5Code2Str(BYTE* Md5Code, TCHAR* pStr)
{
	WCHAR rgbDigits[] = L"0123456789abcdef";
	TCHAR* result = pStr;

	for (DWORD i = 0; i < MD5_CODE_LEN; i++){

#ifdef _DEBUG
		wsprintf(pStr, L"%c%c", rgbDigits[Md5Code[i] >> 4], rgbDigits[Md5Code[i] & 0xf]);
#endif // _DEBUG
		
		pStr += 2;
	}
	return result;
}

DWORD Md5File(const TCHAR* filename, BYTE* Md5Code)
{
	DWORD dwStatus = 0;
	BOOL bResult = FALSE;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	PBYTE rgbFile = new BYTE[BUFSIZE];
	DWORD cbRead = 0;
	BYTE rgbHash[MD5_CODE_LEN];
	DWORD cbHash = 0;

	do 
	{
		hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

		if (INVALID_HANDLE_VALUE == hFile){
			return GetLastError();
		}

		if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)){
			dwStatus = GetLastError();
			break;;
		}

		if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)){
			dwStatus = GetLastError();
			break;
		}

		while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, &cbRead, NULL)){
			if (0 == cbRead){
				break;
			}

			if (!CryptHashData(hHash, rgbFile, cbRead, 0)){
				dwStatus = GetLastError();
				break;
			}
		}

		if (!bResult){
			dwStatus = GetLastError();
			break;
		}

		cbHash = MD5_CODE_LEN;
		if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)){
			CopyMemory(Md5Code, rgbHash, cbHash);
		}else{
			dwStatus = GetLastError();
		}

	} while (FALSE);
	
	delete [] rgbFile;

	if(hHash)
		CryptDestroyHash(hHash);
	if(hProv)
		CryptReleaseContext(hProv, 0);
	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	return dwStatus; 
}  

DWORD Md5Buffer(BYTE* buffer, DWORD buferSize, BYTE* Md5Code)
{
	DWORD dwStatus = 0;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	DWORD cbRead = 0;
	BYTE rgbHash[MD5_CODE_LEN];
	DWORD cbHash = 0;

	do 
	{
		if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)){
			dwStatus = GetLastError();
			break;;
		}

		if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)){
			dwStatus = GetLastError();
			break;
		}

		if (!CryptHashData(hHash, buffer, buferSize, 0)){
			dwStatus = GetLastError();
			break;
		}

		cbHash = MD5_CODE_LEN;
		if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)){
			CopyMemory(Md5Code, rgbHash, cbHash);
		}else{
			dwStatus = GetLastError();
		}

	} while (FALSE);

	if(hHash)
		CryptDestroyHash(hHash);
	if(hProv)
		CryptReleaseContext(hProv, 0);

	return dwStatus; 
}  

DWORD Md5_Init(OUT PMD5_HANDLE* hMd5)
{
	DWORD dwStatus = 0;
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	do 
	{
		if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)){
			dwStatus = GetLastError();
			break;
		}

		if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)){
			dwStatus = GetLastError();
			break;
		}

		*hMd5 = new MD5_HANDLDE;
		(*hMd5)->hHash = hHash;
		(*hMd5)->hProv = hProv;

	} while (FALSE);

	if(dwStatus != 0){
		if(hHash)
			CryptDestroyHash(hHash);
		if(hProv)
			CryptReleaseContext(hProv, 0);
	}
	return dwStatus;
}

DWORD Md5_HashData(PMD5_HANDLE hMd5, BYTE* buffer, DWORD bufferSize, BYTE* Md5Code)
{
	HCRYPTHASH hHash = ((PMD5_HANDLE)hMd5)->hHash;

	if (!CryptHashData(hHash, buffer, bufferSize, 0)){
		return  GetLastError();
	}
	
	if(Md5Code){
		DWORD cbHash = MD5_CODE_LEN;
		if (!CryptGetHashParam(hHash, HP_HASHVAL, Md5Code, &cbHash, 0))
			return GetLastError();
	}

	return 0;
}

DWORD Md5_Finsh(PMD5_HANDLE hMd5, BYTE* Md5Code)
{
	DWORD dwStatus = 0;
	HCRYPTHASH hHash = ((PMD5_HANDLE)hMd5)->hHash;
	HCRYPTPROV hProv = ((PMD5_HANDLE)hMd5)->hProv;

	if(Md5Code){
		DWORD cbHash = MD5_CODE_LEN;
		if (!CryptGetHashParam(hHash, HP_HASHVAL, Md5Code, &cbHash, 0))
			dwStatus = GetLastError();
	}

	if(hHash)
		CryptDestroyHash(hHash);
	if(hProv)
		CryptReleaseContext(hProv, 0);

	return dwStatus;
}