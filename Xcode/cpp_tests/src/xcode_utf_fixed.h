//
//  wfunction.hpp
//  cpp_tests
//
//  Created by jief on 15.03.20.
//  Copyright © 2020 JF Knudsen. All rights reserved.
//

#ifndef __xcode_utf16_h__
#define __xcode_utf16_h__


/*
 * all the functions w... seems to expect utf32 even when compiled with short-wchar
 */

size_t   wcslen_fixed(const wchar_t *s);
int      wcsncmp_fixed(const wchar_t *s1, const wchar_t * s2, size_t n);
const wchar_t* wcsstr_fixed(const wchar_t* s1, const wchar_t* s2);


#endif /* wfunction_hpp */
