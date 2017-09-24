#pragma once

/*
��̬�⵼���ӿ�ʵ�֣�����̬��ȫ�����úͱ�����(ThisLibrary ������ǰ��̬���ȫ�����ݣ�
*/

#include "ErasureLibrary.h"
#include "ui/ErasureMainWnd.h"
#include "eraser/ErasureConfig.h"

class CErasureImpl
	:public IErasureLibrary
{
private:
	friend BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/);
public:
	CErasureImpl();
	~CErasureImpl();
	CErasureConfig* GetConfig();
	HMODULE GetModuleHandle();
protected:
	CFramNotifyPump* GetNotifyPump() override;
	TCHAR* UIResorce() override;
	void SetUI(CControlUI* pCtrl) override;
	CControlUI* GetUI() override;

	DWORD EraseFile(CLdConfig Param, IEraserThreadCallback* callback) override;
private:
	CErasureMainWnd* m_MainWnd;
	CControlUI* m_Ctrl;
	CErasureConfig* m_Config;
	HMODULE m_hModule;
};

extern CErasureImpl* ThisLibrary;
