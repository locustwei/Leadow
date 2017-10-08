#pragma once

/*
�����ļ���д���ࣨJSON��ʽ����������ģ�������ļ��Ӵ˼̳�
*/

#include "LdString.h"
#include "../../Jsonlib/Value.h"
namespace LeadowLib {

	#define  CDynObjectValue JsonBox::Value

	class CDynObject
	{
	public:
		CDynObject();
		~CDynObject();

		BOOL LoadFromFile(TCHAR* FileName);
		BOOL SaveToFile(TCHAR* FileName);
		BOOL PrepareStr(TCHAR* szJson);
		void operator = (CDynObject& source);   

		int GetDataType(CLdStringA Path); //���Ͳμ�JsonBox::Value::Type
		int GetArrayCount(CLdStringA Path);
		BOOL GetBoolean(CLdStringA Path, BOOL def = FALSE, int index = -1);
		double GetDouble(CLdStringA Path, double def = 0.0, int index = -1);
		float GetFloat(CLdStringA Path, float def = 0.0, int index = -1);
		int GetInteger(CLdStringA Path, int def = 0, int index = -1);
		CLdString GetString(CLdStringA Path, TCHAR* def = nullptr, int index = -1);

		VOID AddArrayValue(                                         //���������Ŀ
			CLdStringA Path, 
			CDynObjectValue value
		);
		void AddConfigObject(                                       //�������ֵ
			CLdStringA string,                                      //·��
			CDynObjectValue value,                                   //ֵ
			int index = -1                                          //�����±꣺-1�������顣
		);
		CLdString ToString();
	protected:
		CDynObjectValue m_Config;
//		CLdStringA m_ConfigFileName;

	private:
		CDynObjectValue GetConfigObject(                             //��ȡָ��·���µĶ���
			CLdStringA string,                                      //·��
			int index = -1                                          //������������飬���������±�
//			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,  //�����������Ϊnull_value,�򵱶��󲻴���ʱ����һ��
//			PVOID pValue=nullptr                                    //�������,����ֵ
		);
	};
}
