/*
 * Copyright (c) 2019 Jief Luce.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#if !defined(__XSTRINGABSTRACT_H__)
#define __XSTRINGABSTRACT_H__

#include <XToolsConf.h>
#include "XToolsCommon.h"
#include "unicode_conversions.h"

#ifndef DEBUG_ALL
#define DEBUG_XStringAbstract 0
#else
#define DEBUG_XStringAbstract DEBUG_ALL
#endif

#if DEBUG_XStringAbstract == 0
#define DBG_XSTRING(...)
#else
#define DBG_XSTRING(...) DebugLog(DEBUG_XStringAbstract, __VA_ARGS__)
#endif

//#if __WCHAR_MAX__ <= 0xFFFFu
//    #define wchar_cast char16_t
//#else
//    #define wchar_cast char32_t
//#endif






#define asciiToLower(ch) (((ch >= L'A') && (ch <= L'Z')) ? ((ch - L'A') + L'a') : ch)
#define asciiToUpper(ch) (((ch >= L'a') && (ch <= L'z')) ? ((ch - L'a') + L'A') : ch)

template<typename S, typename O>
int XStringAbstract__startWith(const S* src, const O* other, bool ignoreCase)
{
	size_t nb = 0;
	const S* src2 = src;
	const O* other2 = other;

	char32_t src_char32;
	char32_t other_char32;
	other2 = get_char32_from_string(other2, &other_char32);
	if ( !other_char32 ) return true; // startWith with empty string is considered true
	src2 = get_char32_from_string(src2, &src_char32);
	while ( other_char32 ) {
		if ( ignoreCase ) {
			src_char32 = asciiToLower(src_char32);
			other_char32 = asciiToLower(other_char32);
		}
		if ( src_char32 != other_char32 ) return false;
		src2 = get_char32_from_string(src2, &src_char32);
		other2 = get_char32_from_string(other2, &other_char32);
		nb += 1;
	};
	return src_char32 != 0;
}

template<typename S, typename O>
int XStringAbstract__compare(const S* src, const O* other, bool ignoreCase)
{
//	size_t len_s = length_of_utf_string(src);
//	size_t len_other = length_of_utf_string(other);
	size_t nb = 0;
	const S* src2 = src;
	const O* other2 = other;

	char32_t src_char32;
	char32_t other_char32;
	src2 = get_char32_from_string(src2, &src_char32);
	other2 = get_char32_from_string(other2, &other_char32);
	while ( src_char32 ) {
		if ( ignoreCase ) {
			src_char32 = asciiToLower(src_char32);
			other_char32 = asciiToLower(other_char32);
		}
		if ( src_char32 != other_char32 ) break;
		src2 = get_char32_from_string(src2, &src_char32);
		other2 = get_char32_from_string(other2, &other_char32);
		nb += 1;
	};
	if ( src_char32 == other_char32 ) return 0;
	return src_char32 > other_char32 ? 1 : -1;
}

template<typename S, typename O>
int XStringAbstract__ncompare(const S* src, const O* other, size_t n, bool ignoreCase)
{
	if ( n == 0 ) return 0; // string of 0 length are equal.
	const S* src2 = src;
	const O* other2 = other;

	char32_t src_char32;
	char32_t other_char32;
	src2 = get_char32_from_string(src2, &src_char32);
	other2 = get_char32_from_string(other2, &other_char32);
	size_t nb = 1;
	while ( src_char32  &&  nb < n ) {
		if ( ignoreCase ) {
			src_char32 = asciiToLower(src_char32);
			other_char32 = asciiToLower(other_char32);
		}
		if ( src_char32 != other_char32 ) break;
		src2 = get_char32_from_string(src2, &src_char32);
		other2 = get_char32_from_string(other2, &other_char32);
		nb += 1;
	};
	if ( src_char32 == other_char32 ) return 0;
	return src_char32 > other_char32 ? 1 : -1;
}

template<typename O, typename P>
size_t XStringAbstract__indexOf(const O** s, const P* other, size_t offsetRet, bool toLower)
{
	size_t Idx = 0;

	char32_t s_char32;
	char32_t other_char32;

	do
	{
		const O* s2 = *s;
		const P* other2 = other;
		do {
			s2 = get_char32_from_string(s2, &s_char32);
			other2 = get_char32_from_string(other2, &other_char32);
			if ( toLower ) {
				s_char32 = asciiToLower(s_char32);
				other_char32 = asciiToLower(other_char32);
			}
		} while ( s_char32  &&  other_char32  && s_char32 == other_char32 );
		if ( other_char32 == 0 ) return Idx+offsetRet;
		*s = get_char32_from_string(*s, &s_char32);
		Idx++;
	} while (s_char32);
	return MAX_XSIZE;
}

template<typename O, typename P>
size_t XStringAbstract__indexOf(const O* s, size_t Pos, const P* other, bool toLower)
{
	if ( *other == 0 ) return Pos;

	char32_t char32 = 1;
	for ( size_t Idx=0 ; Idx<Pos ; Idx+=1 ) {
		s = get_char32_from_string(s, &char32);
	}
	if ( !char32 ) return MAX_XSIZE;
	return XStringAbstract__indexOf(&s, other, Pos, toLower);
}

template<typename O, typename P>
size_t XStringAbstract__rindexOf(const O* s, size_t Pos, const P* other, bool toLower)
{
	if ( *other == 0 ) return Pos > length_of_utf_string(s) ? length_of_utf_string(s) : Pos;

	size_t index = XStringAbstract__indexOf(&s, other, 0, toLower);
	size_t prev_index = index; // initialize to index in case of index is already == Pos
	
	char32_t char32;
	s = get_char32_from_string(s, &char32);
	while ( char32  && index < Pos ) {
		prev_index = index;
		index = XStringAbstract__indexOf(&s, other, index+1, toLower);
		s = get_char32_from_string(s, &char32);
	};
	if ( index == Pos ) return index;
	if ( prev_index <= Pos ) return prev_index;
	return MAX_XSIZE;
}


template<class T, class ThisXStringClass>
class __String
{
public:
	typedef T char_t;
	typedef ThisXStringClass xs_t;
protected:
 	T *m_data;
	
	// convenience method. Did it this way to avoid #define in header. They can have an impact on other headers
	size_t Xmin(size_t x1, size_t x2) const { if ( x1 < x2 ) return x1; return x2; }
	size_t Xmax(size_t x1, size_t x2) const { if ( x1 > x2 ) return x1; return x2; }

  // Method _data is protected intentionally. It's a const method returning non-const pointer. That's intentional, but dangerous. Do not expose to public.
  // If you need a non-const pointer for low-level access, use dataSized and specify the size
	// pos is counted in logical char (UTF32 char)
	template<typename IntegralType, enable_if(is_integral(IntegralType))>
	T* _data(IntegralType pos) const
	{
		if ( pos<0 ) panic("T* data(int i) -> i < 0");
 		size_t offset = size_of_utf_string_len(m_data, (unsigned_type(IntegralType))pos); // If pos is too big, size_of_utf_string_len returns the end of the string
 		return m_data + offset;
	}
public:

//	T* memoryOffset(size_t i) {
//
//	}
public:
    constexpr __String(const T* s) : m_data((T*)s) {}
public:
//	constexpr __String() : m_data(&nullChar) {  }
	constexpr __String(const __String&) = delete;
	constexpr __String() = delete;

	// no assignement, no destructor

	template<typename IntegralType, enable_if(is_integral(IntegralType))>
	T* data(IntegralType pos) const { return _data(pos); }


	size_t length() const { return length_of_utf_string(m_data); }
//	size_t sizeZZ() const { return size_of_utf_string(m_data); }
	size_t sizeInNativeChars() const { return size_of_utf_string(m_data); }
	size_t sizeInBytes() const { return size_of_utf_string(m_data)*sizeof(T); }
	size_t sizeInBytesIncludingTerminator() const { return (size_of_utf_string(m_data)+1)*sizeof(T); } // usefull for unit tests


	const T* s() const { return m_data; }
	const T* data() const { return m_data; } // todo delete

	/* Empty ? */
	bool isEmpty() const { return m_data == nullptr  ||  *m_data == 0; }
	bool notEmpty() const { return !isEmpty(); }


	//--------------------------------------------------------------------- cast

//	int ToInt() const;
//	size_t ToUInt() const;


	//--------------------------------------------------------------------- charAt, []

	template<typename IntegralType, enable_if(is_integral(IntegralType))>
	char32_t char32At(IntegralType i) const
	{
		if (i < 0) {
			panic("__String<T>::char32At(size_t i) : i < 0. System halted\n");
		}
		size_t nb = 0;
		const T *p = m_data;
		char32_t char32;
		do {
			p = get_char32_from_string(p, &char32);
			if (!char32) {
				if ( (unsigned_type(IntegralType))i == nb ) return 0; // no panic if we want to access the null terminator
				panic("__String::char32At(size_t i) : i >= length(). System halted\n");
			}
			nb += 1;
		} while (nb <= (unsigned_type(IntegralType))i);
		return char32;
	}
	
	template<typename IntegralType, enable_if(is_integral(IntegralType))>
	char16_t char16At(IntegralType i) const
	{
		char32_t char32 = char32At(i);
		if ( char32 >= 0x10000 ) return 0xFFFD; // � REPLACEMENT CHARACTER used to replace an unknown, unrecognized or unrepresentable character
		return (char16_t)char32;
	}
	
	/* [] */
	template<typename IntegralType, enable_if(is_integral(IntegralType))>
	char32_t operator [](IntegralType i) const { return char32At(i); }


	char32_t lastChar() const { if ( length() > 0 ) return char32At(length()-1); else return 0; }
//	/* copy ctor */
//	__String<T, ThisXStringClass>(const __String<T, ThisXStringClass> &S)	{ Init(0); takeValueFrom(S); }
//	/* ctor */
//	template<typename O, class OtherXStringClass>
//	explicit __String<T, ThisXStringClass>(const __String<O, OtherXStringClass>& S) { Init(0); takeValueFrom(S); }
////	template<typename O>
////	explicit __String<T, ThisXStringClass>(const O* S) { Init(0); takeValueFrom(S); }

	/* Copy Assign */ // Only other XString, no litteral at the moment.
//	__String<T, ThisXStringClass>& operator =(const __String<T, ThisXStringClass>& S) { strcpy(S.s()); return *this; }
//	/* Assign */
//	template<typename O, class OtherXStringClass>
//	ThisXStringClass& operator =(const __String<O, OtherXStringClass>& S)	{ strcpy(S.s()); return *((ThisXStringClass*)this); }
////	template<class O>
////	ThisXStringClass& operator =(const O* S)	{ strcpy(S); return *this; }

	//--------------------------------------------------------------------- indexOf, rindexOf, contains, subString
	
	/* indexOf */
	size_t indexOf(char32_t char32Searched, size_t Pos = 0) const
	{
		char32_t buf[2] = { char32Searched, 0};
		return XStringAbstract__indexOf(m_data, Pos, buf, false);
	}
	template<typename O>
	size_t indexOf(const O* S, size_t Pos = 0) const { return XStringAbstract__indexOf(m_data, Pos, S, false); }
	template<typename O, class OtherXStringClass>
	size_t indexOf(const __String<O, OtherXStringClass>& S, size_t Pos = 0) const { return indexOf(S.s(), Pos); }
	/* IC */
	size_t indexOfIC(char32_t char32Searched, size_t Pos = 0) const
	{
		char32_t buf[2] = { char32Searched, 0};
		return XStringAbstract__indexOf(m_data, Pos, buf, true);
	}
	template<typename O>
	size_t indexOfIC(const O* S, size_t Pos = 0) const { return XStringAbstract__indexOf(m_data, Pos, S, true); }
	template<typename O, class OtherXStringClass>
	size_t indexOfIC(const __String<O, OtherXStringClass>& S, size_t Pos = 0) const { return indexOfIC(S.s(), Pos); }


	/* rindexOf */
	size_t rindexOf(const char32_t char32Searched, size_t Pos = MAX_XSIZE-1) const
	{
		char32_t buf[2] = { char32Searched, 0};
		return XStringAbstract__rindexOf(m_data, Pos, buf, false);
	}
	template<typename O>
	size_t rindexOf(const O* S, size_t Pos = MAX_XSIZE-1) const { return XStringAbstract__rindexOf(m_data, Pos, S, false); }
	template<typename O, class OtherXStringClass>
	size_t rindexOf(const __String<O, OtherXStringClass>& S, size_t Pos = MAX_XSIZE-1) const { return rindexOf(S.s(), Pos); }
	/* IC */
	size_t rindexOfIC(const char32_t char32Searched, size_t Pos = MAX_XSIZE-1) const
	{
		char32_t buf[2] = { char32Searched, 0};
		return XStringAbstract__rindexOf(m_data, Pos, buf, true);
	}
	template<typename O>
	size_t rindexOfIC(const O* S, size_t Pos = MAX_XSIZE-1) const { return XStringAbstract__rindexOf(m_data, Pos, S, true); }
	template<typename O, class OtherXStringClass>
	size_t rindexOfIC(const __String<O, OtherXStringClass>& S, size_t Pos = MAX_XSIZE-1) const { return rindexOf(S.s(), Pos); }

	template<typename O, class OtherXStringClass>
	bool contains(const __String<O, OtherXStringClass>& S) const { return indexOf(S) != MAX_XSIZE; }
	template<typename O>
	bool contains(const O* S) const { return indexOf(S) != MAX_XSIZE; }
	template<typename O, class OtherXStringClass>
	size_t containsIC(const __String<O, OtherXStringClass>& S) const { return indexOfIC(S) != MAX_XSIZE; }
	template<typename O>
	size_t containsIC(const O* S) const { return indexOfIC(S) != MAX_XSIZE; }


	ThisXStringClass subString(size_t pos, size_t count) const
	{
//		if ( pos > length() ) return ThisXStringClass();
//		if ( count > length()-pos ) count = length()-pos;
		
		ThisXStringClass ret;

		const T* src = m_data;
		char32_t char32 = 1;
		while ( char32  && pos > 0 ) {
			src = get_char32_from_string(src, &char32);
			pos -= 1;
		};
		ret.strncat(src, count);
		return ret;
	}

	template<typename O, class OtherXStringClass>
	bool startWith(const __String<O, OtherXStringClass>& otherS) const { return XStringAbstract__startWith(m_data, otherS.m_data, false); }
	template<typename O>
	bool startWith(const O* other) const { return XStringAbstract__startWith(m_data, other, false); }
	template<typename O, class OtherXStringClass>
	bool startWithIC(const __String<O, OtherXStringClass>& otherS) const { return XStringAbstract__startWith(m_data, otherS.m_data, true); }
	template<typename O>
	bool startWithIC(const O* other) const { return XStringAbstract__startWith(m_data, other, true); }

	//---------------------------------------------------------------------

	ThisXStringClass basename() const
	{
		size_t lastSepPos = MAX_XSIZE;
		size_t pos = 0;
		const T *p = m_data;
		char32_t char32;
		p = get_char32_from_string(p, &char32);
		while ( char32 ) {
			if ( char32 == U'/'  ||  char32 == U'\\' ) lastSepPos = pos;
			pos += 1;
			p = get_char32_from_string(p, &char32);
		};
		if ( lastSepPos == MAX_XSIZE ) {
			if ( p == m_data ) return ThisXStringClass().takeValueFrom(".");
		}
		return subString(lastSepPos+1, MAX_XSIZE);
	}
//	ThisXStringClass dirname() const
//	{
//		size_t idx = rindexOf('/');
//		if ( idx == MAX_XSIZE ) return ThisXStringClass();
//		return subString(0, idx);
//	}

//	void insert(const __String<T, ThisXStringClass>& Str, size_t pos);
//{
//	if ( pos < size() ) {
//		CheckSize(size()+Str.size());
//		memmove(_data(pos + Str.size()),  data(pos),  (size()-pos)*sizeof(T));
//		memmove(_data(pos), Str.data(), Str.size()*sizeof(T));
//		setLength(size()+Str.size());
//	}else{
//		StrCat(Str);
//	}
//}

//	void ToLower(bool FirstCharIsCap = false);
//	bool IsLetters() const;
//	bool IsLettersNoAccent() const;
//	bool IsDigits() const;
//{
//  const T *p;
//
//	p = data();
//	if ( !*p ) return false;
//	for ( ; *p ; p+=1 ) {
//		if ( *p < '0' ) return false;
//		if ( *p > '9' ) return false;
//	}
//	return true;
//}
//	bool IsDigits(size_t pos, size_t count) const;
//{
//  const T *p;
//  const T *q;
//
//	if ( pos >= size() ) {
//		return false;
//	}
//	if ( pos+count > size() ) {
//		return false;
//	}
//	p = data() + pos;
//	q = p + count;
//	for ( ; p < q ; p+=1 ) {
//		if ( *p < '0' ) return false;
//		if ( *p > '9' ) return false;
//	}
//	return true;
//}

//	void Replace(T c1, T c2)
//	{
//		T* p;
//
//		p = s();
//		while ( *p ) {
//			if ( *p == c1 ) *p = c2;
//			p += 1;
//		}
//	}
//	__String SubStringReplace(T c1, T c2);
//{
//  T* p;
//  __String Result;
//
//	p = s();
//	while ( *p  ) {
//		if ( *p == c1 ) Result += c2;
//		else Result += *p;
//		p++;
//	}
//	return Result;
//}

	//---------------------------------------------------------------------

	template<typename O>
	int strcmp(const O* S) const { return XStringAbstract__compare(m_data, S, false); }
//	int Compare(const char* S) const { return ::Compare<T, char>(m_data, S); }
//	int Compare(const char16_t* S) const { return ::Compare<T, char16_t>(m_data, S); };
//	int Compare(const char32_t* S) const { return ::Compare<T, char32_t>(m_data, S); };
//	int Compare(const wchar_t* S) const { return ::Compare<T, wchar_t>(m_data, S); };
//
	template<typename O, class OtherXStringClass>
	bool equal(const __String<O, OtherXStringClass>& S) const { return XStringAbstract__compare(m_data, S.s(), false) == 0; }
	template<typename O>
	bool equal(const O* S) const { return XStringAbstract__compare(m_data, S, false) == 0; }

	template<typename O, class OtherXStringClass>
	bool equalIC(const __String<O, OtherXStringClass>& S) const { return XStringAbstract__compare(m_data, S.s(), true) == 0; }
	template<typename O>
	bool equalIC(const O* S) const { return XStringAbstract__compare(m_data, S, true) == 0; }

//	bool SubStringEqual(size_t Pos, const T* S) const { return (memcmp(data(Pos), S, wcslen(S)) == 0); }

public:
	// == operator
	template<typename O, class OtherXStringClass>
	bool operator == (const __String<O, OtherXStringClass>& s2) const { return (*this).strcmp(s2.s()) == 0; }
//	template<typename O>
//	bool operator == (const O* s2) const { return (*this).strcmp(s2) == 0; }
//	template<typename O>
//	friend bool operator == (const O* s1, ThisXStringClass& s2) { return s2.strcmp(s1) == 0; }

	template<typename O, class OtherXStringClass>
	bool operator != (const __String<O, OtherXStringClass>& s2) const { return !(*this == s2); }
//	template<typename O>
//	bool operator != (const O* s2) const { return !(*this == s2); }
//	template<typename O>
//	friend bool operator != (const O* s1, const ThisXStringClass& s2) { return s2.strcmp(s1) != 0; }

	template<typename O, class OtherXStringClass>
	bool operator <  (const __String<O, OtherXStringClass>& s2) const { return (*this).strcmp(s2.s()) < 0; }
//	template<typename O>
//	bool operator <  (const O* s2) const { return (*this).strcmp(s2) < 0; }
//	template<typename O>
//	friend bool operator <  (const O* s1, const ThisXStringClass& s2) { return s2.strcmp(s1) > 0; }

	template<typename O, class OtherXStringClass>
	bool operator >  (const __String<O, OtherXStringClass>& s2) const { return (*this).strcmp(s2.s()) > 0; }
//	template<typename O>
//	bool operator >  (const O* s2) const { return  (*this).strcmp(s2) > 0; }
//	template<typename O>
//	friend bool operator >  (const O* s1, const ThisXStringClass& s2) { return s2.strcmp(s1) < 0; }

	template<typename O, class OtherXStringClass>
	bool operator <= (const __String<O, OtherXStringClass>& s2) const { return (*this).strcmp(s2.s()) <= 0; }
//	template<typename O>
//	bool operator <= (const O* s2) const { return  (*this).strcmp(s2) <= 0; }
//	template<typename O>
//	friend bool operator <= (const O* s1, const ThisXStringClass& s2) { return s2.strcmp(s1) >= 0; }

	template<typename O, class OtherXStringClass>
	bool operator >= (const __String<O, OtherXStringClass>& s2) const { return (*this).strcmp(s2.s()) >= 0; }
//	template<typename O>
//	bool operator >= (const O* s2) const { return  (*this).strcmp(s2) >= 0; }
//	template<typename O>
//	friend bool operator >= (const O* s1, const ThisXStringClass& s2) { return s2.strcmp(s1) <= 0; }

};


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

template<class T, class ThisXStringClass>
class LString : public __String<T, ThisXStringClass>
{
public:
protected:
	constexpr LString(const T* s) : __String<T, ThisXStringClass>(s) {};
	constexpr LString() = delete;
	constexpr LString(const LString& L) : __String<T, ThisXStringClass>(L.m_data) {};

	// no assignement, no destructor

};

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


#define m_data __String<T, ThisXStringClass>::m_data

template<class T, class ThisXStringClass>
class XStringAbstract : public __String<T, ThisXStringClass>
{
	static T nullChar;

  protected:
  	size_t m_allocatedSize; // Must include null terminator. Real memory allocated is only m_allocatedSize (not m_allocatedSize+1)
	
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	// Init , Alloc
	//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
	
	/*
	 * nNewSize must include null terminator.
	 */
	void Alloc(size_t nNewSize)
	{
			if ( m_allocatedSize == 0 ) m_data = (T*)malloc( nNewSize*sizeof(T) );
			else m_data = (T*)Xrealloc(m_data, nNewSize*sizeof(T), m_allocatedSize*sizeof(T));
			if ( !m_data ) {
				panic("XStringAbstract::Alloc(%zu) : Xrealloc(%" PRIuPTR ", %lu, %zd) returned NULL. System halted\n", nNewSize, uintptr_t(m_data), nNewSize*sizeof(T), m_allocatedSize*sizeof(T));
			}
			m_allocatedSize = nNewSize;
	}

//  public:
  /*
   * Make sure this string has allocated size of at least nNewSize+1.
   */
	T *CheckSize(size_t nNewSize, size_t nGrowBy = XStringGrowByDefault) // nNewSize is in number of chars, NOT bytes
	{
		//DBG_XSTRING("CheckSize: m_size=%d, nNewSize=%d\n", m_size, nNewSize);
		if ( m_allocatedSize < nNewSize+1 )
		{
			nNewSize += nGrowBy;
			if ( m_allocatedSize == 0 ) { //if ( *m_data ) {
				size_t len = __String<T, ThisXStringClass>::length();
				if ( nNewSize < len ) nNewSize = len;
				T* m_dataSav = m_data;
				m_data = NULL;
				Alloc(nNewSize+1);
				utf_string_from_utf_string(m_data, m_allocatedSize, m_dataSav);
			}else{
				Alloc(nNewSize+1);
			}
		}
		return m_data;
	}
//	void setSize(size_t newSize) // nNewSize is in number of chars, NOT bytes
//	{
//		//DBG_XSTRING("setLength(%d)\n", len);
//		CheckSize(newSize);
//		//	if ( len >= size() ) {
//		//		DBG_XSTRING("__String<T>::setLength(size_t len) : len >= size() (%d != %d). System halted\n", len, size());
//		//		panic();
//		//	}
//		m_data[newSize] = 0; // we may rewrite a 0 in nullChar, if no memory were allocated. That's ok.
//	}


public:

	/* default ctor */
	XStringAbstract() : __String<T, ThisXStringClass>(&nullChar), m_allocatedSize(0) {}
	
	/* copy ctor */
	XStringAbstract(const XStringAbstract& S) : __String<T, ThisXStringClass>(&nullChar), m_allocatedSize(0)
	{
		if ( S.m_data  &&  !S.m_allocatedSize ) {
			m_data = S.m_data;
		}else{
			takeValueFrom(S);
		}
	}

	~XStringAbstract()
	{
		//DBG_XSTRING("Destructor :%ls\n", data());
		if ( m_allocatedSize > 0 ) free((void*)m_data);
	}

	/* ctor */
	template<class OtherLStringClass>
	explicit XStringAbstract(const LString<T, OtherLStringClass>& S) : __String<T, ThisXStringClass>(S.s()), m_allocatedSize(0) {}
	
	template<typename O, class OtherXStringClass>
	explicit XStringAbstract<T, ThisXStringClass>(const XStringAbstract<O, OtherXStringClass>& S) : __String<T, ThisXStringClass>(&nullChar), m_allocatedSize(0) { takeValueFrom(S); }
	template<typename O, class OtherXStringClass>
	explicit XStringAbstract<T, ThisXStringClass>(const LString<O, OtherXStringClass>& S) : __String<T, ThisXStringClass>(&nullChar), m_allocatedSize(0) { takeValueFrom(S); }
// TEMPORARILY DISABLED
//	template<typename O>
//	explicit __String<T, ThisXStringClass>(const O* S) { Init(0); takeValueFrom(S); }
//
	/* Copy Assign */ // Only other XString, no litteral at the moment.
	XStringAbstract& operator=(const XStringAbstract &S)  { takeValueFrom(S); return *this; }
	/* Assign */
	#pragma GCC diagnostic ignored "-Weffc++"
	template<typename O, class OtherXStringClass>
	ThisXStringClass& operator =(const __String<O, OtherXStringClass>& S)	{ strcpy(S.s()); return *((ThisXStringClass*)this); }
  #pragma GCC diagnostic warning "-Weffc++"
// TEMPORARILY DISABLED
//	template<class O>
//	ThisXStringClass& operator =(const O* S)	{ strcpy(S); return *this; }

protected:
	ThisXStringClass& takeValueFromLiteral(const T* s)
	{
		if ( m_allocatedSize > 0 ) panic("XStringAbstract::takeValueFromLiteral -> m_allocatedSize > 0");
		m_data = (T*)s;
		return *((ThisXStringClass*)this);
	}

public:
	
	size_t allocatedSize() const { return m_allocatedSize; }
	
	void setEmpty()
	{
		if ( m_allocatedSize <= 0 ) m_data = &nullChar;
		else m_data[0] = 0;
	}

	
	template<typename IntegralType, enable_if(is_integral(IntegralType))>
	T* dataSized(IntegralType size)
	{
		if ( size<0 ) panic("T* dataSized() -> i < 0");
		if ( (unsigned_type(IntegralType))size > MAX_XSIZE ) panic("T* dataSized() -> i > MAX_XSIZE");
		CheckSize((size_t)size);
		return __String<T, ThisXStringClass>::_data(0);
	}
//
//	// Pos is counted in logical char but size is counted in physical char (char, char16_t, char32_t or wchar_t)
//	template<typename IntegralType1, typename IntegralType2, enable_if(is_integral(IntegralType1) && is_integral(IntegralType2))>
//	T* dataSized(IntegralType1 pos, IntegralType2 size)
//	{
//		if ( pos<0 ) panic("T* dataSized(xisize i, size_t sizeMin, size_t nGrowBy) -> i < 0");
//		if ( size<0 ) panic("T* dataSized(xisize i, size_t sizeMin, size_t nGrowBy) -> i < 0");
// 		size_t offset = size_of_utf_string_len(m_data, (typename _xtools__make_unsigned<IntegralType1>::type)pos); // If pos is too big, size_of_utf_string_len returns the end of the string
//		CheckSize(offset + (typename _xtools__make_unsigned<IntegralType2>::type)size);
//		return _data(pos);
//	}


	T* forgetDataWithoutFreeing()
	{
		T* ret = m_data;
		m_data = &nullChar;
		m_allocatedSize = 0;
		return ret;
	}

	
	//--------------------------------------------------------------------- strcat, strcpy, operator =

	/* strcpy char */
	template<typename O, enable_if(is_char(O))>
	void strcpy(O otherChar)
	{
		if ( otherChar != 0) {
			size_t newSize = utf_size_of_utf_string_len(m_data, &otherChar, 1);
			CheckSize(newSize, 0);
			utf_string_from_utf_string_len(m_data, m_allocatedSize, &otherChar, 1);
			m_data[newSize] = 0;
		}else{
			setEmpty();
		}
	}
	/* strcpy */
	template<typename O>
	void strcpy(const O* other)
	{
		if ( other && *other ) {
			size_t newSize = utf_size_of_utf_string(m_data, other);
			CheckSize(newSize, 0);
			utf_string_from_utf_string(m_data, m_allocatedSize, other);
			m_data[newSize] = 0;
		}else{
			setEmpty();
		}
	}
	/* strncpy */
	template<typename O>
	void strncpy(const O* other, size_t other_len)
	{
		if ( other && *other && other_len > 0 ) {
			size_t newSize = utf_size_of_utf_string_len(m_data, other, other_len);
			CheckSize(newSize, 0);
			utf_string_from_utf_string_len(m_data, m_allocatedSize, other, other_len);
			m_data[newSize] = 0;
		}else{
			setEmpty();
		}
	}

	/* strcat char */
	template<typename O, enable_if(is_char(O))>
	void strcat(O otherChar)
	{
		if ( otherChar ) {
			size_t currentSize = size_of_utf_string(m_data);
			size_t newSize = currentSize + utf_size_of_utf_string_len(m_data, &otherChar, 1);
			CheckSize(newSize, 0);
			utf_string_from_utf_string_len(m_data+currentSize, m_allocatedSize, &otherChar, 1);
			m_data[newSize] = 0;
		}else{
			// nothing to do
		}
	}
  /* strcat char* */
  template<typename O>
  void strcat(const O* other)
  {
    if ( other && *other ) {
      size_t currentSize = size_of_utf_string(m_data); // size is number of T, not in bytes
      size_t newSize = currentSize + utf_size_of_utf_string(m_data, other); // size is number of T, not in bytes
      CheckSize(newSize, 0);
      utf_string_from_utf_string(m_data+currentSize, m_allocatedSize-currentSize, other);
      m_data[newSize] = 0;
    }else{
      // nothing to do
    }
  }
	/* strcat __String */
	template<typename OtherCharType, class OtherXStringClass>
	void strcat(const __String<OtherCharType, OtherXStringClass>& other)
	{
		size_t currentSize = size_of_utf_string(m_data); // size is number of T, not in bytes
		size_t newSize = currentSize + utf_size_of_utf_string(m_data, other.s()); // size is number of T, not in bytes
		CheckSize(newSize, 0);
		utf_string_from_utf_string(m_data+currentSize, m_allocatedSize-currentSize, other.s());
		m_data[newSize] = 0;
	}
	/* strncat */
	template<typename O>
	void strncat(const O* other, size_t other_len)
	{
		if ( other && *other && other_len > 0 ) {
			size_t currentSize = size_of_utf_string(m_data);
			size_t newSize = currentSize + utf_size_of_utf_string_len(m_data, other, other_len);
			CheckSize(newSize, 0);
			utf_string_from_utf_string_len(m_data+currentSize, m_allocatedSize, other, other_len);
			m_data[newSize] = 0;
		}else{
			// nothing to do
		}
	}
    
  /* insert char* */
  template<typename O>
  ThisXStringClass&  insertAtPos(const O* other, size_t other_len, size_t pos)
  {
    if ( !other || !*other ) return *((ThisXStringClass*)this);
    
    size_t currentLength = __String<T, ThisXStringClass>::length();
    if ( pos >= currentLength ) {
      strncat(other, other_len);
      return *((ThisXStringClass*)this);
    }

    size_t currentSize = size_of_utf_string(m_data);
    size_t otherSize = utf_size_of_utf_string_len(m_data, other, other_len);
    CheckSize(currentSize+otherSize, 0);
    size_t start = size_of_utf_string_len(m_data, pos); // size is number of T, not in bytes
    memmove( m_data + start + otherSize, m_data + start, (currentSize-start+1)*sizeof(T)); // memmove handles overlapping memory move
    utf_stringnn_from_utf_string(m_data+start, otherSize, other);
//    m_data[newSize] = 0;
    return *((ThisXStringClass*)this);
  }
  
  /* insert char* */
  template<typename O>
  ThisXStringClass&  insertAtPos(const O* other, size_t pos)
  {
    if ( !other || !*other ) return *((ThisXStringClass*)this);
    
    size_t currentLength = __String<T, ThisXStringClass>::length();
    if ( pos >= currentLength ) {
      strcat(other);
      return *((ThisXStringClass*)this);
    }

    size_t currentSize = size_of_utf_string(m_data);
    size_t otherSize = utf_size_of_utf_string(m_data, other);
    CheckSize(currentSize+otherSize, 0);
    size_t start = size_of_utf_string_len(m_data, pos); // size is number of T, not in bytes
    memmove( m_data + start + otherSize, m_data + start, (currentSize-start+1)*sizeof(T)); // memmove handles overlapping memory move
    utf_stringnn_from_utf_string(m_data+start, otherSize, other);
//    m_data[newSize] = 0;
    return *((ThisXStringClass*)this);
  }
  /* insert char */
  template<typename O, enable_if(is_char(O))>
  void insertAtPos(O otherChar, size_t pos)
  {
    insertAtPos(&otherChar, 1, pos);
  }

  ThisXStringClass& deleteCharsAtPos(size_t pos, size_t count=1)
  {
    size_t currentLength = __String<T, ThisXStringClass>::length();
    if ( pos >= currentLength ) return *((ThisXStringClass*)this);

    size_t currentSize = size_of_utf_string(m_data); // size is number of T, not in bytes
    CheckSize(currentSize, 0); // Although we only delete, we have to CheckSize in case this string point to a litteral.

    size_t start = size_of_utf_string_len(m_data, pos); // size is number of T, not in bytes

//    if ( pos+count >= currentLength ) count = currentLength - pos;
    if ( pos+count >= currentLength ) {
      m_data[start] = 0;
    }else{
      size_t end = start + size_of_utf_string_len(m_data+start, count); // size is number of T, not in bytes
      memmove( m_data + start, m_data + end, (currentSize-end+1)*sizeof(T)); // memmove handles overlapping memory move
    }
    // Handle length change when implementing caching length feature.
    return *((ThisXStringClass*)this);
  }
  
  void trim()
  {
    T* start = 0;
    size_t count = 0;
    T* s = m_data;
    while ( *s && unsigned_type(T)(*s) <= 32 ) s++;
    start = s;
    while ( *s && unsigned_type(T)(*s) > 32 ) s++;
    count = uintptr_t(s - start);
    CheckSize(count); // We have to CheckSize in case this string point to a litteral.
    memmove(m_data, start, count*sizeof(T));
    m_data[count] = 0;
  }

  void lowerAscii()
  {
    size_t currentSize = size_of_utf_string(m_data); // size is number of T, not in bytes
    CheckSize(currentSize, 0); // We have to CheckSize in case this string point to a litteral.
    T* s = m_data;
    while ( *s ) {
      *s = asciiToLower(*s);
      s++;
    }
  }

  void upperAscii()
  {
    size_t currentSize = size_of_utf_string(m_data); // size is number of T, not in bytes
    CheckSize(currentSize, 0); // We have to CheckSize in case this string point to a litteral.
    T* s = m_data;
    while ( *s ) {
      *s = asciiToUpper(*s);
      s++;
    }
  }


  ThisXStringClass& stealValueFrom(T* S) {
    if ( m_allocatedSize > 0 ) free((void*)m_data);
    m_data = S;
    m_allocatedSize = utf_size_of_utf_string(m_data, S) + 1;
    return *((ThisXStringClass*)this);
  }

	/* takeValueFrom */
	template<typename O, class OtherXStringClass>
	ThisXStringClass& takeValueFrom(const __String<O, OtherXStringClass>& S) { strcpy(S.s()); return *((ThisXStringClass*)this); }
  template<typename O>
  ThisXStringClass& takeValueFrom(const O* S) { strcpy(S); return *((ThisXStringClass*)this); }
  template<typename O, enable_if(is_char(O))>
  ThisXStringClass& takeValueFrom(const O C) { strcpy(C); return *((ThisXStringClass*)this); }
	template<typename O, class OtherXStringClass>
	ThisXStringClass& takeValueFrom(const __String<O, OtherXStringClass>& S, size_t len) { strncpy(S.data(0), len); return *((ThisXStringClass*)this);	}
	template<typename O>
	ThisXStringClass& takeValueFrom(const O* S, size_t len) {	strncpy(S, len); return *((ThisXStringClass*)this); }
	

	/* += */
	template<typename O, class OtherXStringClass>
	ThisXStringClass& operator += (const __String<O, OtherXStringClass>& S) { strcat(S.s()); return *((ThisXStringClass*)this); }
	template<typename O, enable_if(is_char(O))>
	ThisXStringClass& operator += (O S) { strcat(S); return *((ThisXStringClass*)this); }
	template<typename O>
	ThisXStringClass& operator += (const O* S) { strcat(S); return *((ThisXStringClass*)this); }

	
};

template<class T, class ThisXStringClass>
T XStringAbstract<T, ThisXStringClass>::nullChar = 0;


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

///* __String + char32_t */
//template<typename CharType1, class XStringClass1>
//XStringClass1 operator + (const __String<CharType1, XStringClass1>& p1, char32_t p2) { XStringClass1 s; s.takeValueFrom(p1); s.strcat(p2); return s; }
//
///* __String + __String */
//template<typename CharType1, class XStringClass1, typename CharType2, class XStringClass2>
//XStringClass1 operator + (const __String<CharType1, XStringClass1>& p1, const __String<CharType2, XStringClass2>& p2) { XStringClass1 s; s.takeValueFrom(p1); s.strcat(p2); return s; }
//
///* char* + __String */
//template<typename CharType1, typename CharType2, class XStringClass2>
//XStringClass2 operator + (const CharType1* p1, const __String<CharType2, XStringClass2>& p2) { XStringClass2 s; s.takeValueFrom(p1); s.strcat(p2); return s; }
//
///* __String + char* */
//template<typename T1, class XStringClass1, typename CharType2>
//XStringClass1 operator + (const __String<T1, XStringClass1>& p1, const CharType2* p2) { XStringClass1 s; s.takeValueFrom(p1); s.strcat(p2); return s; }


template <typename Base> _xtools__true_type is_base_of_test_func( Base* );
template <typename Base> _xtools__false_type is_base_of_test_func( void* );
template <typename B, typename D>
auto test_pre_is_base_of(int) -> decltype(is_base_of_test_func<B>(static_cast<D*>(nullptr)));


template< class, class = _xtools__void_t<>, class = _xtools__void_t<> >
struct __string_type { typedef void type; };
template< typename T >
struct __string_type<T, _xtools__void_t<typename T::xs_t>, _xtools__void_t<typename T::char_t>> { typedef __String<typename T::char_t, typename T::xs_t> type; };

#define is___String_t(x) decltype(test_pre_is_base_of<typename __string_type<x>::type , x>(0))
#define is___String(x) is___String_t(x)::value


template< class, class = _xtools__void_t<>, class = _xtools__void_t<> >
struct __lstring_type { typedef void type; };
template< typename T >
struct __lstring_type<T, _xtools__void_t<typename T::xs_t>, _xtools__void_t<typename T::char_t>> { typedef LString<typename T::char_t, typename T::xs_t> type; };

#define is___LString_t(x) decltype(test_pre_is_base_of<    typename __lstring_type<x>::type , x>(0))
#define is___LString(x) is___LString_t(x)::value

/* __string_class_or<T1, T2>::type is T1 is T1 is a subclass of __String. If T1 is not a subclass of __String, returns T2 if it's a subclass of __String */
template <typename T1, typename T2, typename Tdummy=void>
struct __string_class_or;
template <typename T1, typename T2>
struct __string_class_or<T1, T2, enable_if_t(!is___String(T1) && !is___String(T2))> { /*typedef double type;*/ };
template <typename T1, typename T2>
struct __string_class_or<T1, T2, enable_if_t(is___String(T1))> { typedef typename T1::xs_t type; };
template <typename T1, typename T2>
struct __string_class_or<T1, T2, enable_if_t(!is___String(T1) && is___String(T2))> { typedef typename T2::xs_t type; };

//------------------------------------------------------- + operator

template<typename T1, typename T2, enable_if( is___String(T1) || is___String(T2) )>
typename __string_class_or<T1, T2>::type operator + (T1 p1, T2 p2) { typename __string_class_or<T1, T2>::type s; s.takeValueFrom(p1); s.strcat(p2); return s; }



//-------------------------------------------------------

#undef DBG_XSTRING
#undef asciiToLower
#undef m_data


#endif // __XSTRINGABSTRACT_H__
