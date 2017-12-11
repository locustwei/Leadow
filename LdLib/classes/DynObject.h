#pragma once

/*
����JSONʵ�ֵġ���̬���󡱣�����һЩ��ȷ����ʽ������ͬһ����
�磺������Ϣ�������в�����
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

		VOID AddArrayValue(                                         //���������Ŀ
			CLdStringA Path,
			wchar_t* value
		);

		void AddObjectAttribute(                                       //�������ֵ
			CLdStringA path,                                      //·��
			CDynObjectValue value,                                   //ֵ
			int index = -1                                          //�����±꣺-1�������顣
		);

		void AddObjectAttribute(                                       //�������ֵ
			CLdStringA path,                                      //·��
			wchar_t* value,                                   //ֵ
			int index = -1                                          //�����±꣺-1�������顣
		);

		CLdString ToString();
	protected:
		CDynObjectValue m_Config;
//		CLdStringA m_ConfigFileName;

	private:
		CDynObjectValue GetDynObject(                             //��ȡָ��·���µĶ���
			CLdStringA string,                                      //·��
			int index = -1                                          //������������飬���������±�
//			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,  //�����������Ϊnull_value,�򵱶��󲻴���ʱ����һ��
//			PVOID pValue=nullptr                                    //�������,����ֵ
		);
	};
}
