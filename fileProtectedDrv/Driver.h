#pragma once

typedef struct _WFS_GLOBAL_DATA {
	PDRIVER_OBJECT DriverObject;
	PDEVICE_OBJECT CtrlDevice;
	PFLT_FILTER  FilterHandle;          //����Handle��ע��ʱ���ɣ�ж��ʱ�ͷ�
	ULONG WinVer;
} WFS_GLOBAL_DATA;

extern WFS_GLOBAL_DATA Globals;
extern ULONG WIN_VER;


extern HANDLE g_WiseProcessId;