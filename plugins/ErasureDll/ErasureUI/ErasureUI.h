#pragma once

/*
动态库导出接口实现，并动态库全局设置和变量。(ThisLibrary 变量当前动态库的全局数据）
*/

#include <LdCommunication.h>
#include <LdPlugin.h>

class CErasureUI
	: public IPluginInterface
{
public:
	CErasureUI();
	~CErasureUI() override;
	HMODULE GetModuleHandle();
protected: //IPluginInterface
	CFramNotifyPump* CreateUI() override;
	DWORD InitCommunicate() override;
//protected: //ICommunicationListen
//	bool OnCreate() override;
//	void OnTerminate(DWORD exitcode) override;
//	void OnCommand(WORD id, TCHAR* ProcessName, PVOID data, WORD nSize) override;
private:
	HMODULE m_hModule;
};

