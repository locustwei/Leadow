// dllmain.h : 模块类的声明。

class CLdShellExtendModule : public ATL::CAtlDllModuleT< CLdShellExtendModule >
{
public :
	DECLARE_LIBID(LIBID_LdShellExtendLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_LDSHELLEXTEND, "{2E75A25B-D17C-4544-A3A8-8F329A1F1191}")
};

extern class CLdShellExtendModule _AtlModule;
