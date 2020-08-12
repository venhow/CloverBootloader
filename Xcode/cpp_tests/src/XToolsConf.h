#ifndef __XTOOLSCONF_H__
#define __XTOOLSCONF_H__

#include <posix.h>


#define xsize size_t
//#define xisize INTN
#ifdef _MSC_VER
#define MAX_XSIZE SIZE_MAX
#else
#define MAX_XSIZE SIZE_T_MAX
#endif
//#define MAX_XISIZE MAX_INTN

#define XStringGrowByDefault 10
#define XArrayGrowByDefault 8
//extern xsize XBufferGrowByDefault;

/* For convience, operator [] is define with int parameter.
 * Defining __XTOOLS_CHECK_OVERFLOW__ make a check that the parameter is >= 0
 * TODO : make new XString using __XTOOLS_CHECK_OVERFLOW__
 */
#define __XTOOLS_CHECK_OVERFLOW__

#define Xrealloc(ptr, newsize, oldsize) realloc(ptr, newsize)



#endif
