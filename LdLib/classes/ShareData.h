#pragma once
#include "Thread.h"
#include "../LdDelegate.h"

namespace LeadowLib
{
	/*
	内存映像文件实现两个进程间进行数据通信。
	第一个创建进程称为Master，第二个称为Client。

	*/

#pragma warning(disable: 4200)

	class CShareData: IThreadRunable
	{

	public:
		CShareData(TCHAR* pName, WORD nSize = 1024);
		~CShareData();
		CLdString& GetName();
		//写数据
		virtual DWORD Write(PVOID pData, UINT nLength);
		//读取数据（在没有读取数据线程时适用）
		//		virtual DWORD Read(PVOID pData, WORD nLength); 
		//virtual DWORD Read(PBYTE* pData, WORD* nLength);
		//线程等待读取数据
		virtual DWORD StartReadThread(
			CMethodDelegate ReadCallback, //读到数据回掉
			bool FreeOnTerminate = false);        //停止读取时删除自己
												  //停止读取数据线程
		void StopReadThread();

		void SetWaitTimeOut(UINT nTime);
		//		static CShareData* Create(TCHAR* pName, UINT nSize);

	private:
		typedef struct SHAREDATA_DATA
		{
			WORD id;       //标记是谁写入的
			WORD nSize;
			BYTE Data[0];
		}*PSHAREDATA_DATA;

		HANDLE m_hFile;            //内存映像文件
		PSHAREDATA_DATA m_pFileHeader;   //映像文件内存地址
		UINT m_Size;               //文件大小
		HANDLE m_hSemaphore;       //信号量，同步读写内存映像文件
		HANDLE m_hReadEvent;       //等待数据写入事件。
		//HANDLE m_hWriteEvent;      //发送数据写入。

		CLdString m_Name;          //内存映像文件名
		bool m_TermateReadThread;  //结束读取线程。
//		bool m_IsMaster;           //是否为Master
		UINT m_nTimeOut;           //WaitFor写入、读取等待超时
		bool m_FreeOnTerminate;    //读取线程结束时删除自己
		CMethodDelegate m_ReadCallback;
		WORD m_ThisID;            //同一共享文件，使用者编号
	protected: //IThreadRunable
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	};

}
