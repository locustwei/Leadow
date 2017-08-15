#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <Commdlg.h>
#include <shellapi.h>
#include <io.h>
#include <fcntl.h>


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	return (int) 0;
}

static ULONG HashKey(PWCH Key, int nLength)
{
	ULONG i = 0;
	while (nLength-- > 0)
		i = (i << 5 ) + towlower(Key[nLength]) + i;
	return i;

//	ULONG crc = 0;
//	TCHAR b;
//	int i = 0;
//
//	for (; i < nLength; i++)
//	{
//		b = towlower(Key[i]);
//		crc = crc ^ (b << (i % 7) * 4) + b;
////		if (crc & 0x80000000)
////			crc = (crc << 1) ^ 0x04C11DB7;
////		else 
////			crc = crc << 1;
//	}
//			
//	return  crc;
};

int PtFuncCompare(void const* item1, void const* item2)
{
	int m = HashKey(*(TCHAR**)item1, _tcslen(*(TCHAR**)item1)) % 100;
	int n = HashKey(*(TCHAR**)item2, _tcslen(*(TCHAR**)item2)) % 100;
	return m - n;
}

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");

	DWORD cb;
	TCHAR* strs[] = {
		L"QianYingTips.exe",
		L"mininews.exe",
		L"QMTencentNews.exe",
		L"MangoMini.exe",
		L"minisite.exe",
		L"QyFragment.exe",
		L"BFpop.exe",
		L"xmptipwnd.1.0.0.99.exe",
		L"2345PicMiniPage.exe",
		L"sesvc.exe",
		L"kminisite.exe",
		L"newsclient.exe",
		L"kktip.exe",
		
		L"532AF15",
		L"5GThFgHJPbcIB3.HsnW7",
		L"7Dd6VD3z.veU9d",
		L"addins",
		L"appcompat",
		L"AppPatch",
		L"AppReadiness",
		L"bcastdvr",
		L"bfsvc.exe",
		L"Boot",
		L"Branding",
		L"CbsTemp",
		L"comsetup.log",
		L"CSC",
		L"Cursors",
		L"CYp9C.SIz86",
		L"debug",
		L"desktop.ini",
		L"diagerr.xml",
		L"diagnostics",
		L"diagwrn.xml",
		L"DigitalLocker",
		L"DPINST.LOG",
		L"DtcInstall.log",
		L"en-US",
		L"Enterprise.xml",
		L"explorer.exe",
		L"GameBarPresenceWriter",
		L"Globalization",
		L"Help",
		L"HelpPane.exe",
		L"hh.exe",
		L"HoloShell",
		L"IME",
		L"ImmersiveControlPanel",
		L"INF",
		L"InfusedApps",
		L"InputMethod",
		L"KMS10",
		L"L2Schemas",
		L"LiveKernelReports",
		L"Logs",
		L"mib.bin",
		L"Microsoft.NET",
		L"Migration",
		L"Minidump",
		L"MiracastView",
		L"ModemLogs",
		L"notepad.exe",
		L"ntbtlog.txt",
		L"OCR",
		L"OfflineWebPages",
		L"oobe3",
		L"Panther",
		L"PCHEALTH",
		L"Performance",
		L"PFRO.log",
		L"PLA",
		L"PolicyDefinitions",
		L"Prefetch",
		L"PrintDialog",
		L"Provisioning",
		L"regedit.exe",
		L"Registration",
		L"Reimage.ini",
		L"RemotePackages",
		L"rescache",
		L"Resources",
		L"RtlExUpd.dll",
		L"SchCache",
		L"schemas",
		L"security",
		L"ServiceProfiles",
		L"servicing",
		L"Setup",
		L"setuperr.log",
		L"ShellExperiences",
		L"ShellNew",
		L"SKB",
		L"SoftwareDistribution",
		L"Speech",
		L"Speech_OneCore",
		L"splwow64.exe",
		L"symbols",
		L"System",
		L"system.ini",
		L"System32",
		L"SystemApps",
		L"SystemResources",
		L"SysWOW64",
		L"TAPI",
		L"Tasks",
		L"Temp",
		L"ThPUKXv2PUUq73s.7rWy6",
		L"tracing",
		L"twain_32",
		L"twain_32.dll",
		L"veRIi21HNSJ5j.7sJsJ",
		L"Vss",
		L"Web",
		L"win.ini",
		L"WindowsUpdate.log",
		L"winhlp32.exe",
		L"WinSxS",
		L"WiseFs64.sys",
		L"WiseHDInfo64.dll",
		L"WiseRegNotify.sys",
		L"WiseTDIFw64.sys",
		L"WiseUnlock64.sys",
		L"WMSysPr9.prx",
		L"write.exe",
		L"xinstaller.1.3.0.22.dll",
		L"xinstaller.1.3.0.22.exe"
		
	};

	qsort(&strs, 126, sizeof(TCHAR*), PtFuncCompare);
	ULONG k = 0, k1, k2 = 0;
	for (int i = 0; i < 126; i++)
	{
		k1 = HashKey(strs[i], _tcslen(strs[i])) % 100;
		if(k1 != k)
		{
			//printf("------------------------------------------------\n");
			k = k1;
			k2++;
		}
		printf("%d  %S\n", k1, strs[i]);
	}

	printf("%d \n", k2);

	printf("\npress any key exit");
	getchar();
	return 0;
}
