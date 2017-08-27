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
#include "../Jsonlib/JsonBox.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	return (int) 0;
}

/*
class CImpl: 
	public IGernalCallback<PSH_HEAD_INFO>
	,public ISortCompare<CLdString*>
{
public:
	CImpl()
	{
		Count = 0;
	};

	int Count;

	int ArraySortCompare(CLdString** it1, CLdString** it2) override
	{
		int k = _tcsicmp((*it1)->GetData(), (*it2)->GetData());
		if (k == 0)
			printf("%d\n", Count++);
		return k;
	};

	BOOL GernalCallback_Callback(_SH_HEAD_INFO* pData, UINT_PTR Param) override
	{
		CLdArray<CLdString>* columes = (CLdArray<CLdString>*)Param;
		columes->Add(pData->szName);
		//printf("%S\n", pData->szName);

		return true;
	};
};*/

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");
	CoInitialize(nullptr);

	JsonBox::Object o;
	o["myName"] = JsonBox::Value(123);
	o["myOtherMember"] = JsonBox::Value("asld\\kfn");
	o["hahaha"] = JsonBox::Value(true);
	o["adamo"] = JsonBox::Value(129.09);
	o["child"] = JsonBox::Value(o);
	JsonBox::Array a;
	a.push_back(JsonBox::Value("I'm a string..."));
	a.push_back(JsonBox::Value(123));
	o["array"] = JsonBox::Value(a);

	std::cout << o << std::endl;
	JsonBox::Value v(o);
	v.writeToFile("file.json");

	printf("\npress any key exit");
	getchar();
	return 0;
}
