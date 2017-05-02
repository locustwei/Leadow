/*
 * Copyright(C) 2009,yutianzuo (All rights reserved.)
 * 
 * FileDes  : string_x.h
 * Abstract : extension of std::basic_string
 * Version  : 1.0 
 * Author   : yutianzuo
 * Date     : 2009-2-11
 * 
 */
#ifndef __STRING_X_H__
#define __STRING_X_H__
//------------------------------------------------------------------------
#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdarg>

template<typename _Elem, typename _Traits, typename _Alloc>
class stringx;

template<typename T>
struct x_traits;

template<>
struct x_traits <char>
{
	typedef char char_type;
	typedef wchar_t r_char_type;

	static int para_len(const char_type* ptr, va_list argList)
	{
		return ::_vscprintf(ptr, argList);
	}

	static int x_sprintf(char_type* ptr, int len_bytes, const char_type* c_ptr, va_list argList)
	{
		return ::vsprintf_s(ptr, len_bytes, c_ptr, argList);
	}

	typedef stringx<r_char_type, std::char_traits<r_char_type>, std::allocator<r_char_type> > r_derive_type;
	typedef std::basic_string<r_char_type, std::char_traits<r_char_type>, std::allocator<r_char_type> > r_basic_string_type;

	static int w2c_c2w(const char_type* src, int src_len, r_char_type* dst, int dst_len, unsigned long cp = CP_ACP)
	{
		return ::MultiByteToWideChar(cp, 0, src, src_len, dst, dst_len);
	}

	static size_t string_lenth(const char_type* p_string)
	{
		return ::strlen(p_string);
	}
};

template<>
struct x_traits <wchar_t>
{
	typedef wchar_t char_type;
	typedef char r_char_type;

	static int para_len(const char_type* ptr, va_list argList)
	{
		return ::_vscwprintf(ptr, argList);
	}

	static int x_sprintf(char_type* ptr, int len_characters, const char_type* c_ptr, va_list argList)
	{
		return ::vswprintf_s(ptr, len_characters, c_ptr, argList);
	}

	typedef stringx<r_char_type, std::char_traits<r_char_type>, std::allocator<r_char_type> > r_derive_type;
	typedef std::basic_string<r_char_type, std::char_traits<r_char_type>, std::allocator<r_char_type> > r_basic_string_type;

	static int w2c_c2w(const char_type* src, int src_len, r_char_type* dst, int dst_len, unsigned long cp = CP_ACP)
	{
		return ::WideCharToMultiByte(cp, 0, src, src_len, dst, dst_len, NULL, NULL);
	}

	static size_t string_lenth(const char_type* p_string)
	{
		return ::wcslen(p_string);
	}

};


template<typename _Elem, typename _Traits, typename _Alloc>
class stringx : public std::basic_string<_Elem, _Traits, _Alloc>
{
public:
	typedef stringx<_Elem, _Traits, _Alloc> _derive_type;
	typedef stringx _derive_no_type;  //both are ok in vs2005
	typedef std::basic_string<_Elem, _Traits, _Alloc> _basic_type;

	stringx() : _basic_type()
	{
	}


	//added 2011-1-26
	stringx(const typename x_traits<_Elem>::r_char_type* ptr, unsigned long cp = CP_ACP)
	{
		if (!ptr)
		{
			return;
		}
		unsigned long len = x_traits<typename x_traits<_Elem>::r_char_type>::w2c_c2w(ptr, -1, NULL, 0, cp);
		_Elem* out_buf = new (std::nothrow) _Elem[len + 1];
		if (!out_buf)
		{
			return;
		}
		memset(out_buf, 0, (len + 1) * sizeof(_Elem));
		x_traits<typename x_traits<_Elem>::r_char_type>::w2c_c2w(ptr, -1, out_buf, len, cp);
		this->assign(out_buf);
		delete [] out_buf;
	}

	stringx(const _Elem* ptr, size_type count) : _basic_type(ptr, count)
	{
	}

	stringx(const _Elem* ptr, size_type pos, size_type count) : _basic_type(ptr, pos, count)
	{
	}

	stringx(const _basic_type& basic_str) : _basic_type(basic_str)
	{
	}

	stringx(const _Elem* _Ptr) : _basic_type(_Ptr)
	{
	}

	//copy construction
	//also the compiler will generate a default copy constructor for us which is ok for this situation
	//because basic_string has a explicit copy construction while our derive class does not need a explicit copy construtor
	stringx(const _derive_type& strx) : _basic_type(strx.c_str())
	{
	}


	void trim()
	{
		this->erase(0, this->find_first_not_of((_Elem)0x20));
		this->erase(this->find_last_not_of((_Elem)0x20) + 1);
	}

	void trim(_Elem _ch)
	{
		this->erase(0, this->find_first_not_of(_ch));
		this->erase(this->find_last_not_of(_ch) + 1);
	}

	void trim_left()
	{
		this->erase(0, this->find_first_not_of((_Elem)0x20));
	}

	void trim_left(_Elem _ch)
	{
		this->erase(0, this->find_first_not_of(_ch));
	}

	void trim_right()
	{
		this->erase(this->find_last_not_of((_Elem)0x20) + 1);
	}

	void trim_right(_Elem _ch)
	{
		this->erase(this->find_last_not_of(_ch) + 1);
	}


	//operator= can not be use directly in the derive class 
	//but complier will generate operator=(const _derive_type&) if we didn't explicit give one
	
	_derive_type& operator=(const _Elem* _Ptr)
	{
		this->assign(_Ptr);
		return *this;
	}

	_derive_type& operator=(_Elem _ch)
	{
		this->assign(1, _ch);
		return *this;
	}

	_derive_type& operator=(const _basic_type& basic_str)
	{
		this->assign(basic_str.c_str());
		return *this;
	}

	//this will be automatically implemented by the compiler if we don't give one explicitly
	//if we don't give this operator=(const _derive_type& strx), it will also work well
	//because the one the compiler generated will automatically invoke operator=(const _basic_type&)
	_derive_type& operator=(const _derive_type& strx)
	{
		this->assign(strx.c_str());
		return *this;
	}


	int compare_no_case(const _Elem* ptr) const
	{
		if (!ptr)
		{
			return 1;
		}
		_derive_type strx_tempr(ptr);
		_derive_type strx_templ(*this);
		strx_tempr.to_lower();
		strx_templ.to_lower();
		return strx_templ.compare(strx_tempr.c_str());
	}

	int compare_no_case(const _Elem* ptr, std::size_t characters, std::size_t pos = 0) const
	{
		if (!ptr || 0 == characters || pos >= this->size() || pos + characters > this->size())
		{
			return 1;
		}
		_derive_type strx_tempr(ptr);
		_derive_type strx_templ(this->c_str(), pos, characters);
		strx_tempr.to_lower();
		strx_templ.to_lower();
		return strx_templ.compare(strx_tempr.c_str());
	}

	void to_lower()
	{
		std::transform(this->begin(), this->end(), this->begin(), _derive_type::_help_to_lower);
	}

	void to_upper()
	{
		std::transform(this->begin(), this->end(), this->begin(), _derive_type::_help_to_upper);
	}

	void format(const _Elem* ptr, ...)
	{
		if (!ptr)
		{
			return;
		}
		va_list argList;
		va_start(argList, ptr);
		int len_characters = x_traits<_Elem>::para_len(ptr, argList);
		this->resize(len_characters + 1);
		x_traits<_Elem>::x_sprintf(&this->at(0), len_characters + 1, ptr, argList);
		va_end(argList);
		this->erase(len_characters, 1);
	}

	void convert_code(typename x_traits<_Elem>::r_basic_string_type& out_basic_string, unsigned long cp = CP_ACP) const
	{
		if (0 == this->size())
		{
			return;
		}
		unsigned long len = x_traits<_Elem>::w2c_c2w(this->c_str(), -1, NULL, 0, cp);
		out_basic_string.resize(len - 1);
		x_traits<_Elem>::w2c_c2w(this->c_str(), -1, &out_basic_string.at(0), len - 1, cp);
	}


	void convert_code(typename x_traits<_Elem>::r_derive_type& out_derive_string, unsigned long cp = CP_ACP) const
	{
		if (0 == this->size())
		{
			return;
		}
		unsigned long len = x_traits<_Elem>::w2c_c2w(this->c_str(), -1, NULL, 0, cp);
		out_derive_string.resize(len - 1);
		x_traits<_Elem>::w2c_c2w(this->c_str(), -1, &out_derive_string.at(0), len - 1, cp);
	}


	_basic_type mid(int first, int count) const
	{
		if (0 == this->size())
		{
			return *this;
		}
		if (0 > first)
		{
			first = 0;
		}
		if (0 > count)
		{
			count = 0;
		}
		if (first >= (int)this->size())
		{
			first = 0;
		}
		if (count > (int)this->size())
		{
			count = 0;
		}
		if (count > (int)(this->size() - first))
		{
			count = (int)(this->size() - first);
		}

		if (first == 0 && count == (int)this->size())
		{
			return *this;
		}

		return _basic_type(&this->at(first), count);
	}

	_basic_type left(int count) const
	{
		if (0 == this->size())
		{
			return *this;
		}
		if (0 > count)
		{
			count = 0;
		}
		if (count >= (int)this->size())
		{
			return *this;
		}
		return _basic_type(&this->at(0), count);
	}

	_basic_type right(int count) const
	{
		if (0 == this->size())
		{
			return *this;
		}
		if (0 > count)
		{
			count = 0;
		}
		if (count >= (int)this->size())
		{
			return *this;
		}
		return _basic_type(&this->at(this->size() - count), count);
	}

	int split_string(const _Elem* divide_crt, std::vector<_derive_type>& vec_out_splited_string) const
	{
		if (!divide_crt || 0 == x_traits<_Elem>::string_lenth(divide_crt))
		{
			return -2;
		}
		size_type posb = 0;
		size_type pose = 0;
		size_type pos_first = 0;
		int counter = 0;
		while (npos != (posb = this->find(divide_crt, posb)))
		{
			posb += (_derive_type(divide_crt)).size();
			++counter;
		}
		if (0 == counter)
		{
			return -1;
		}
		pos_first = this->find(divide_crt);
		if (0 != pos_first)
		{
			vec_out_splited_string.push_back(this->substr(0, pos_first));
		}
		posb = 0;
		while (npos != (posb = this->find(divide_crt, posb)))
		{
			posb += (_derive_type(divide_crt)).size();
			pose = this->find(divide_crt, posb);
			if (npos == pose)
			{
				_derive_type str_temp = this->substr(posb);
				if (!str_temp.empty())
				{
					vec_out_splited_string.push_back(str_temp);
				}				
			}
			else
			{
				_derive_type str_temp = this->substr(posb, pose - posb);
				if (!str_temp.empty())
				{
					vec_out_splited_string.push_back(str_temp);
				}				
			}
		}
		return 0;
	}

	void reverse_string()
	{
		if (1 >= this->size())
		{
			return;
		}
		for (size_t i = 0; i < (size_t)this->size() / 2; ++i)
		{
			std::swap(this->at(i), this->at(this->size() - 1 - i));
		}
	}

	_derive_type spe_substr(const _Elem* p_begin, const _Elem* p_end, std::size_t pos_current = 0) const
	{
		if (!p_begin || !p_end || 0 == x_traits<_Elem>::string_lenth(p_begin) || 0 == x_traits<_Elem>::string_lenth(p_end))
		{
			_derive_type temp;
			return temp;
		}
		size_type pos = 0;
		size_type pose = 0;
		pos = this->find(p_begin, pos_current);
		if (npos != pos)
		{
			pos += x_traits<_Elem>::string_lenth(p_begin);
		}
		else
		{
			return _derive_type();
		}
		pose = this->find(p_end, pos);
		if (npos == pose || pose <= pos)
		{
			return _derive_type();
		}
		return this->substr(pos, pose - pos);
	}

	void replace_all(_Elem ch_old, _Elem ch_new)
	{
		if (0 == this->size() || ch_old == ch_new)
		{
			return;
		}
		for (size_type i = 0; i < this->size(); ++i)
		{
			if (ch_old == this->at(i))
			{
				this->at(i) = ch_new;
			}
		}
	}

	void replace_all(const _Elem* p_old, const _Elem* p_new)
	{
		if (0 == this->size() || !p_old || 0 == x_traits<_Elem>::string_lenth(p_old))
		{
			return;
		}
		size_type pos = 0;
		while (npos != (pos = this->find(p_old, pos)))
		{
			this->replace(pos, x_traits<_Elem>::string_lenth(p_old), p_new);
			pos += x_traits<_Elem>::string_lenth(p_new);
		}
	}

	void replace_spe(_Elem ch_old, _Elem ch_new, std::size_t pos_start)
	{
		if (0 == this->size() || (pos_start >= this->size() - 1))
		{
			return;
		}
		for (size_type i = pos_start; i < this->size(); ++ i)
		{
			if (ch_old == this->at(i))
			{
				this->at(i) = ch_new;
			}
		}
	}

	void replace_spe(const _Elem* p_old, const _Elem* p_new, std::size_t pos_start)
	{
		if (0 == this->size() || !p_old || !p_new ||0 == x_traits<_Elem>::string_lenth(p_old) || pos_start >= (this->size() - 1))
		{
			return;
		}
		size_type pos = pos_start;
		while (npos != (pos = this->find(p_old, pos)))
		{
			this->replace(pos, x_traits<_Elem>::string_lenth(p_old), p_new);
			pos += x_traits<_Elem>::string_lenth(p_new);
		}
	}

	operator const _Elem* () const
	{
		return this->c_str();
	}

	_Elem& operator [](int n_pos)
	{
		return _basic_type::operator[](n_pos);
	}

	const _Elem& operator [](int n_pos) const
	{
		return _basic_type::operator[](n_pos);
	}

private:
	static _Elem _help_to_lower(_Elem ch)
	{
		if (ch >= (_Elem)65 && ch <= (_Elem)90) //to lower
		{
			ch += (_Elem)32;
		}
		return ch;
	}

	static _Elem _help_to_upper(_Elem ch)
	{
		if (ch >= (_Elem)97 && ch <= (_Elem)122) //to upper
		{
			ch -= (_Elem)32;
		}
		return ch;
	}

	enum{is_char_or_wchar = (sizeof(_Elem) == sizeof(char))};

	
};




typedef stringx<char, std::char_traits<char>, std::allocator<char> > stringxa;

typedef stringx<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > stringxw;





//------------------------------------------------------------------------
#endif
