#pragma once

/*
从Dui Copy的函数指针对象
*/

#include "classes/LdArray.h"

namespace LeadowLib {

	/*
	通用回掉函数
	*/
	template <typename T>
	interface IGernalCallback
	{
		virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
	};

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
		bool operator() (UINT_PTR param)
		{
			return Invoke(param);
		}
	protected:
		virtual bool Invoke(UINT_PTR param) = 0;
		void* m_pObject;
		void* m_pFn;
	};

	class CStaticMethodDelegate : public CMethodDelegateBase
	{
		typedef bool(*Fn)(UINT_PTR);
	public:
		CStaticMethodDelegate(Fn pFn) : CMethodDelegateBase(nullptr, (void*)pFn) { }
	protected:
		virtual bool Invoke(UINT_PTR param)
		{
			Fn pFn = (Fn)m_pFn;
			return (*pFn)(param);
		}
	};

	template <class T>
	class CObjectMethodDelegate : public CMethodDelegateBase
	{
		typedef bool (T::* Fn)(UINT_PTR);
	public:
		CObjectMethodDelegate(T* pObj, Fn pFn) : CMethodDelegateBase(pObj, *(void**)&pFn) { }
	protected:
		virtual bool Invoke(UINT_PTR param)
		{
			T* pObject = (T*)m_pObject;
			union
			{
				void* ptr;
				Fn fn;
			} func = { m_pFn };
			return (pObject->*func.fn)(param);
		}
	};

	class CMethodDelegate
	{
	public:
		CMethodDelegate()
		{
			m_Delegate = nullptr;
		};
		CMethodDelegate(CMethodDelegate& source)
		{
			m_Delegate = source.m_Delegate;
			//source.m_Delegate = nullptr;
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
		bool operator() (UINT_PTR param)
		{
			if (m_Delegate)
				return (*m_Delegate)(param);
			else
				return false;
		};

		template <class T>
		static CMethodDelegate MakeDelegate(T* pObject, bool (T::* pFn)(UINT_PTR))
		{
			return CMethodDelegate(new CObjectMethodDelegate<T>(pObject, pFn));
		}

		static CStaticMethodDelegate* MakeDelegate(bool(*pFn)(UINT_PTR))
		{
			return new CStaticMethodDelegate(pFn);
		}

	private:
		CMethodDelegateBase* m_Delegate;
	};
}
