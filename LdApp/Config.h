#pragma once

/*
产品配置文件管理单元（读取、写入JSON文件格式），配置文件路径在%appdata%下。
CAppConfig 产品配置文件
CErasureConfig 文件擦除配置文件
*/

class CAppConfig
	:public CDynObject
{
public:
	CAppConfig();
	~CAppConfig();
private:
	
};
