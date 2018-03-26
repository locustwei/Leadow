========================================================================
    WIN32 应用程序：ErasureImpl 项目概述
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

文件、磁盘擦除界面库。
	ErasureMainWnd: 界面框架
	ShFileView：文件信息展示界面，使用Windows提供IShellFolder获取文件、文件夹信息。	
	ErasureFileUI（文件擦除页面）：继承ShFileView。
		用户添加文件、文件夹，显示文件相关属性，擦除状态。
	ErasureRecycleUI（回收站擦除页面）：继承ErasureFileUI。
		读取各磁盘回收站中的文件作为待擦除文件。
	磁盘空闲区擦除页面：继承ShFileView。
	ErasureVolumeUI(磁盘空闲空间擦除)：继承ShFileView。
	ErasureOptions（擦除设置）
头文件：
	EraserUI.h 导出头文件，给引用者使用。
