#pragma once

/*
�ص�������

*/

namespace LeadowLib {

	/*
	�ص��ӿ�
	*/
	template <typename T>
	interface IGernalCallback
	{
		virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
	};

	/*
	��������
	*/
	class CMethodDelegate
	{
	private:
		class CMethodDelegateBase
		{
		public:
			CMethodDelegateBase(void* pObject, void* pFn)
			{
				m_pObject = pObject;
				m_pFn = pFn;
			}
			virtual ~CMethodDelegateBase()
			{

			};
			INT_PTR operator() (PVOID pData, UINT_PTR param)
			{
				return Invoke(pData, param);
			}
		protected:
			virtual INT_PTR Invoke(PVOID pData, UINT_PTR param) = 0;
			void* m_pObject;
			void* m_pFn;
		};
		//��̬��������
		class CStaticMethodDelegate : public CMethodDelegateBase
		{
			typedef INT_PTR(*Fn)(PVOID, UINT_PTR);
		public:
			CStaticMethodDelegate(Fn pFn) : CMethodDelegateBase(nullptr, (void*)pFn) { }
		protected:
			virtual INT_PTR Invoke(PVOID pData, UINT_PTR param)
			{
				Fn pFn = (Fn)m_pFn;
				return (*pFn)(pData, param);
			}
		};
		//����������
		template <class T>
		class CObjectMethodDelegate : public CMethodDelegateBase
		{
			typedef INT_PTR (T::* Fn)(PVOID, UINT_PTR);
		public:
			CObjectMethodDelegate(T* pObj, Fn pFn) : CMethodDelegateBase(pObj, *(void**)&pFn) { }
		protected:
			virtual INT_PTR Invoke(PVOID pData, UINT_PTR param)
			{
				T* pObject = (T*)m_pObject;
				union
				{
					void* ptr;
					Fn fn;
				} func = { m_pFn };
				return (pObject->*func.fn)(pData, param);
			}
		};

	public:
		CMethodDelegate()
		{
			m_Delegate = nullptr;
		};
		//��ֵ��ԭMethodDelegateʧЧ������⹹ʱ����
		CMethodDelegate(CMethodDelegate& source)
		{
			m_Delegate = source.m_Delegate;
			source.m_Delegate = nullptr;
		};

		CMethodDelegate(CMethodDelegateBase* d)
		{
			m_Delegate = d;
		};
		~CMethodDelegate()
		{
			if (m_Delegate)
				delete m_Delegate;
		};

		void operator = (CMethodDelegateBase* d)
		{
			if (m_Delegate)
				delete m_Delegate;

			m_Delegate = d;
		};
		//��ֵ��ԭMethodDelegateʧЧ������⹹ʱ����
		void operator = (CMethodDelegate d)
		{
			if (m_Delegate)
				delete m_Delegate;

			m_Delegate = d.m_Delegate;
			d.m_Delegate = nullptr;
		}

		INT_PTR operator() (PVOID pData, UINT_PTR param)
		{
			return Invoke(pData, param);
		};
		INT_PTR Invoke(PVOID pData, UINT_PTR param)
		{
			if (m_Delegate)
				return (*m_Delegate)(pData, param);
			else
				return 0;
		}

		bool IsNull()
		{
			return m_Delegate == nullptr;
		}

		template <class T>
		static CMethodDelegate MakeDelegate(T* pObject, INT_PTR (T::* pFn)(PVOID, UINT_PTR))
		{
			return CMethodDelegate(new CObjectMethodDelegate<T>(pObject, pFn));
		}

		static CMethodDelegate MakeDelegate(INT_PTR(*pFn)(PVOID, UINT_PTR))
		{
			return CMethodDelegate(new CStaticMethodDelegate(pFn));
		}
		//�մ���
		static CMethodDelegate MakeDelegate()
		{
			return CMethodDelegate(nullptr);
		}
	private:
		CMethodDelegateBase* m_Delegate;
	};
}