#pragma once

/*
回掉函数。

*/

namespace LeadowLib {

	/*
	回掉接口
	*/
	template <typename T>
	interface IGernalCallback
	{
		virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
	};

	/*
	函数代理
	*/
	class CLdMethodDelegate
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
		//静态函数代理
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
		//对象函数代理
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
		CLdMethodDelegate()
		{
			m_Delegate = nullptr;
		};
		//赋值后原MethodDelegate失效，否则解构时出错
		CLdMethodDelegate(CLdMethodDelegate& source)
		{
			m_Delegate = source.m_Delegate;
			source.m_Delegate = nullptr;
		};

		CLdMethodDelegate(CMethodDelegateBase* d)
		{
			m_Delegate = d;
		};
		~CLdMethodDelegate()
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
		//赋值后原MethodDelegate失效，否则解构时出错
		void operator = (CLdMethodDelegate d)
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
		static CLdMethodDelegate MakeDelegate(T* pObject, INT_PTR (T::* pFn)(PVOID, UINT_PTR))
		{
			return CLdMethodDelegate(new CObjectMethodDelegate<T>(pObject, pFn));
		}

		static CLdMethodDelegate MakeDelegate(INT_PTR(*pFn)(PVOID, UINT_PTR))
		{
			return CLdMethodDelegate(new CStaticMethodDelegate(pFn));
		}
		//空代理
		static CLdMethodDelegate MakeDelegate()
		{
			return CLdMethodDelegate(nullptr);
		}
	private:
		CMethodDelegateBase* m_Delegate;
	};
}
