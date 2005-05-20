/*
	Type definitions and helper macros which aren't part of Standard C++
	This will need to be edited on systems where 'char', 'short' and 'int'
   	have sizes different from 8, 16 and 32 bits.
*/

#ifndef __COMMON_H__
#define __COMMON_H__


// #define DEBUG	/**< Defined when compiling code for debugging */

/*
Basic integer types.
Note 'int' is assumed to be in 2s complement format and at least 32 bits in size
*/
typedef unsigned char  uint8;	/**< An 8 bit unsigned integer */
typedef unsigned short uint16;	/**< An 16 bit unsigned integer */
typedef unsigned int   uint32;	/**< An 32 bit unsigned integer */
typedef signed char    int8;	/**< An 8 bit signed integer (2s complement) */
typedef signed short   int16;	/**< An 16 bit signed integer (2s complement) */
typedef signed int     int32;	/**< An 32 bit signed integer (2s complement) */
typedef unsigned int   uint;	/**< An unsigned integer or at least 32 bits */


#ifndef NULL
#define NULL 0		/**< Used to represent a null pointer type */
#endif


#ifdef _MSC_VER		// Compiling for Microsoft Visual C++

#define DEBUGGER	{ _asm int 3 }			/**< Invoke debugger */
#define IMPORT		__declspec(dllexport)	/**< Mark a function which is to be imported from a DLL */
#define EXPORT		__declspec(dllexport)	/**< Mark a function to be exported from a DLL */
#define ASSERT(c)	{ if(!(c)) DEBUGGER; }	/**< Assert that expression 'c' is true */

#else				// Not compiling for Microsoft Visual C++ ...

#define DEBUGGER							/**< Invoke debugger */
#define IMPORT								/**< Mark a function which is to be imported from a DLL */
#define EXPORT								/**< Mark a function to be exported from a DLL */
//#define ASSERT(c)							/**< Assert that expression 'c' is true */

#endif


#ifdef DEBUG
#define ASSERT_DEBUG(c) ASSERT(c)	/**< Assert that expression 'c' is true (when compiled for debugging)*/
#else
#define ASSERT_DEBUG(c)
#endif


#endif

