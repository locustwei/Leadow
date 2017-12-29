#pragma once
#include "Thread.h"

namespace LeadowLib
{
	/*
	内存映像文件实现两个进程间进行数据通信。
	第一个创建进程称为Master，第二个称为Client。

	*/

#pragma warning(disable: 4200)

	class CShareData: IThreadRunable
	{
	private:
		struct MAP_DATA
		{
			BYTE nSign;     //1,标记数据已读取
			WORD nSize;
			BYTE Data[0];
		};
		HANDLE m_hFile;            //内存映像文件
		MAP_DATA* m_pFileHeader;   //映像文件内存地址
		UINT m_Size;               //文件大小
		HANDLE m_hSemaphore;       //信号量，同步读写内存映像文件
		HANDLE m_hWaitEvent;       //等待数据写入事件。
		HANDLE m_hSetEvent;        //发送数据写入。

		CLdString m_Name;          //内存映像文件名
		bool m_TermateReadThread;  //结束读取线程。
		bool m_IsMaster;           //是否为Master
		UINT m_nTimeOut;
	protected:
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	public:
		CShareData(TCHAR* pName, WORD nSize);
		~CShareData();
		//写数据
		virtual DWORD Write(PVOID pData, UINT nLength);
		//读取数据（在没有读取数据线程时适用）
		virtual DWORD Read(PVOID pData, UINT nLength);  
		//线程等待读取数据
		virtual DWORD StartReadThread(IGernalCallback<PVOID>* ReadCallback);
		//停止读取数据线程
		void StopReadThread();

		void SetWaitTimeOut(UINT nTime);
//		static CShareData* Create(TCHAR* pName, UINT nSize);
	};

}
