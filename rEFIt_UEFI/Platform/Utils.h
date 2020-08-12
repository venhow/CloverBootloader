//
//All rights reserved. This program and the accompanying materials
//are licensed and made available under the terms and conditions of the BSD License
//which accompanies this distribution. The full text of the license may be found at
//http://opensource.org/licenses/bsd-license.php
//
//THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//Module Name:
//
//  Utils
//
//

#ifndef _UTILS_H_
#define _UTILS_H_

#include "../cpp_foundation/XString.h"

//Unicode
#define IS_COMMA(a)                ((a) == L',')
#define IS_HYPHEN(a)               ((a) == L'-')
#define IS_DOT(a)                  ((a) == L'.')
#define IS_LEFT_PARENTH(a)         ((a) == L'(')
#define IS_RIGHT_PARENTH(a)        ((a) == L')')
#define IS_SLASH(a)                ((a) == L'/')
#define IS_NULL(a)                 ((a) == L'\0')
//Ascii
#define IS_DIGIT(a)            (((a) >= '0') && ((a) <= '9'))
#define IS_HEX(a)            ((((a) >= 'a') && ((a) <= 'f')) || (((a) >= 'A') && ((a) <= 'F')))
#define IS_UPPER(a)          (((a) >= 'A') && ((a) <= 'Z'))
#define IS_ALFA(x) (((x >= 'a') && (x <='z')) || ((x >= 'A') && (x <='Z')))
#define IS_ASCII(x) ((x>=0x20) && (x<=0x7F))
#define IS_PUNCT(x) ((x == '.') || (x == '-'))


////VOID        LowCase (IN OUT CHAR8 *Str);
UINT32      hex2bin(IN CHAR8 *hex, OUT UINT8 *bin, UINT32 len);
BOOLEAN     IsHexDigit (CHAR8 c);
UINT8       hexstrtouint8 (CHAR8* buf); //one or two hex letters to one byte
XString8    Bytes2HexStr(UINT8 *data, UINTN len);


#endif
