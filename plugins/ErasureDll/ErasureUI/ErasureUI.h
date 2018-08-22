#pragma once

/*
��̬�⵼���ӿ�ʵ�֣�����̬��ȫ�����úͱ�����(ThisLibrary ������ǰ��̬���ȫ�����ݣ�
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

