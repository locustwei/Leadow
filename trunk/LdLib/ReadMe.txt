========================================================================
    静态库：LdLib 项目概述
========================================================================

公用对象、函数库。

LdLib.h：导出头文件，使用者引用

PublicRoutimes：杂七杂八的函数。

classes：
	DynObject（动态对象）：使用json数据，实现一个属性可变的对象。
	LdArray（泛型数组）。
	LdList（泛型链表）。
	LdMap
	LdHashMap
	LdSocket（socket 通讯）
	LdString（字符串）
	LdTree
	Thread（线程）
files：文件、文件夹相关对象
	FileInfo：文件、文件夹、文件交换数据流
	FileUtils：文件相关函数
volume：磁盘、卷（盘符）相关
	Fat：Fat文件系统数据结构定义
	ntfs：ntfs文件系统数据结构定义
	VolumeInfo（磁盘卷）：继承FoldeInfo
utils：杂七杂八的对象
	DateTimeUtils：时间相关函数
	DlgGetFileName：选择文件对话框，支持文件、文件夹同时选。
	FormatSettings：系统日期、金额等格式。
	SHFolders：资源管理器IShellFolder函数调用封装
	