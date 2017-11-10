========================================================================
    WIN32 应用程序：RdErasure 项目概述
========================================================================

文件、磁盘擦除动态库。
导出函数：
	API_Init：获取文件、磁盘擦除函数接口IErasureLibrary
	API_UnInit：释放动态库占用资源。

ErasureImpl：文件、磁盘擦除函数接口IErasureLibrary实现。

Erasure：擦除功能实现
	Erasure：擦除功能实现核心对象。
	ErasureMethod：13种数据擦除方法定义，Erasure使用。
	ErasureThread：多线程调用Erasure同步数据擦除的线程管理。
	VolumeEx：继承VolumeInfo，磁盘擦除时附加擦除过程信息。