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
		CDynObject(const TCHAR* szJson);
		~CDynObject();

		BOOL LoadFromFile(const TCHAR* FileName);
		BOOL SaveToFile(const TCHAR* FileName);
		BOOL PrepareStr(const TCHAR* szJson);
		void operator = (CDynObject& source);   

		int GetDataType(const TCHAR* Path); //���Ͳμ�JsonBox::Value::Type
		int GetArrayCount(const TCHAR* Path);
		BOOL GetBoolean(const TCHAR* Path, BOOL def = FALSE, int index = -1);
		double GetDouble(const TCHAR* Path, double def = 0.0, int index = -1);
		float GetFloat(const TCHAR* Path, float def = 0.0, int index = -1);
		int64_t GetInteger(const TCHAR* Path, int def = 0, int index = -1);
		CLdString GetString(const TCHAR* Path, TCHAR* def = nullptr, int index = -1);

		VOID AddArrayValue(                                         //���������Ŀ
			const TCHAR* Path,
			CDynObjectValue value
		);

		VOID AddArrayValue(                                         //���������Ŀ
			const TCHAR* Path,
			CDynObject& value
		);

		VOID AddArrayValue(                                         //���������Ŀ
			const TCHAR* Path,
			wchar_t* value
		);

		void AddObjectAttribute(                                     //�������ֵ
			const TCHAR*,                                           //·��
			CDynObjectValue value,                                   //ֵ
			int index = -1                                          //�����±꣺-1�������顣
		);

		void AddObjectAttribute(                                       //�������ֵ
			const TCHAR* path,                                      //·��
			wchar_t* value,                                       //ֵ
			int index = -1                                          //�����±꣺-1�������顣
		);

		void AddObjectAttribute(                                       //�������ֵ
			const TCHAR* path,                                      //·��
			CDynObject& obj,                                       //ֵ
			int index = -1                                          //�����±꣺-1�������顣
		);

		CLdString ToString();
	protected:
		CDynObjectValue m_Json;
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
