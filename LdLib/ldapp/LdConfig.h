#pragma once

/*
�����ļ���д���ࣨJSON��ʽ����������ģ�������ļ��Ӵ˼̳�
*/

#include "../../Jsonlib/JsonBox.h"
#include "../classes/LdString.h"

namespace LeadowLib {
	class CLdConfig
	{
	public:
		CLdConfig();
		~CLdConfig();

		BOOL LoadConfig();        //���������ļ�
		BOOL SaveConfig();        //д�������ļ�

		void operator = (CLdConfig& source);    //Copy�����ļ������ݲ������ļ�·����
		/*
			STRING,
			INTEGER,
			DOUBLE,
			OBJECT,
			ARRAY,
			BOOLEAN,
			NULL_VALUE,
			UNKNOWN
		*/
		int GetDataType(CLdStringA Path);

		int GetArrayCount(CLdStringA Path);

		BOOL GetBoolean(CLdStringA Path, BOOL def = FALSE, int index = -1);
		double GetDouble(CLdStringA Path, double def = 0.0, int index = -1);
		float GetFloat(CLdStringA Path, float def = 0.0, int index = -1);
		int GetInteger(CLdStringA Path, int def = 0, int index = -1);
		CLdString GetString(CLdStringA Path, TCHAR* def = nullptr, int index = -1);

		VOID AddArrayValue(                                         //���������Ŀ
			CLdStringA Path, 
			JsonBox::Value value
		);
		void AddConfigObject(                                       //�������ֵ
			CLdStringA string,                                      //·��
			JsonBox::Value value,                                   //ֵ
			int index = -1                                          //�����±꣺-1�������顣
		);
		CLdString ToString();
	protected:
		JsonBox::Value m_Config;
		CLdStringA m_ConfigFileName;
	private:
		JsonBox::Value GetConfigObject(                             //��ȡָ��·���µĶ���
			CLdStringA string,                                      //·��
			int index = -1                                          //������������飬���������±�
//			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,  //�����������Ϊnull_value,�򵱶��󲻴���ʱ����һ��
//			PVOID pValue=nullptr                                    //�������,����ֵ
		);
	};
}
