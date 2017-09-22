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

		int GetDataType(CLdStringA Path);

		int GetArrayCount(CLdStringA Path);
		BOOL GetArrayBoolean(CLdStringA Path, int index, BOOL def = FALSE);
		double GetArrayDouble(CLdStringA Path, int index, double def=0.0);
		float GetArrayFloat(CLdStringA Path, int index, float def=0.0);
		int GetArrayInteger(CLdStringA Path, int index, int def=0);
		CLdString GetArrayString(CLdStringA Path, int index, TCHAR* def=nullptr);

		VOID SetArrayBoolean(CLdStringA Path, int index, BOOL Value);
		VOID SetArrayDouble(CLdStringA Path, int index, double Value);
		VOID SetArrayFloat(CLdStringA Path, int index, float Value);
		VOID SetArrayInteger(CLdStringA Path, int index, int Value);
		VOID SetArrayString(CLdStringA Path, int index, TCHAR* Value);


		BOOL GetBoolean(CLdStringA Path, BOOL def = FALSE);
		double GetDouble(CLdStringA Path, double def = 0.0);
		float GetFloat(CLdStringA Path, float def = 0.0);
		int GetInteger(CLdStringA Path, int def = 0);
		CLdString GetString(CLdStringA Path, TCHAR* def = nullptr);

		VOID SetBoolean(CLdStringA Path, BOOL Value);
		VOID SetDouble(CLdStringA Path, double Value);
		VOID SetFloat(CLdStringA Path, float Value);
		VOID SetInteger(CLdStringA Path, int Value);
		VOID SetString(CLdStringA Path, TCHAR* Value);
	protected:
		CLdStringA m_ConfigFileName;
	private:
		JsonBox::Value m_Config;
		JsonBox::Value GetConfigObject(                             //��ȡָ��·���µĶ���
			CLdStringA string,                                      //·��
			int index = 0,                                          //������������飬���������±�
			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,  //�����������Ϊnull_value,�򵱶��󲻴���ʱ����һ��
			PVOID pValue=nullptr                                    //�������,����ֵ
		);
	};
}
