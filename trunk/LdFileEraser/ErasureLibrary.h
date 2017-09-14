#pragma once
/*
文件擦除动态库导出接口
调用API_Init获取这个接口。

*/
class LDLIB_API IErasureLibrary
{
public:
	virtual ~IErasureLibrary() {};
	/*窗口资源ID
	*用于builer窗口*/
	virtual TCHAR* UIResorce() = 0;
	/*
	窗口Build 完成后回传给Library(在动态库中Build窗口，貌似不太稳定)
	*/
	virtual void SetUI(CControlUI*) = 0;
	virtual CControlUI* GetUI() = 0;
	/*
	获取窗口消息处理对象（窗口Build好后，添加消息处理对象）
	*/
	virtual CFramNotifyPump* GetNotifyPump() = 0;
private:
}; 
