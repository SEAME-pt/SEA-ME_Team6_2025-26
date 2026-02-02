/********************************************************************************
*
* Copyright (c) 2018 STMicroelectronics - All Rights Reserved
*
* License terms: STMicroelectronics Proprietary in accordance with licensing
* terms at www.st.com/sla0081
*
* STMicroelectronics confidential
* Reproduction and Communication of this document is strictly prohibited unless
* specifically authorized in writing by STMicroelectronics.
*
*******************************************************************************/


#ifndef HTGD_PLATFORM_H_
#define HTGD_PLATFORM_H_

#include <stdint.h>
#include <stddef.h> /* this is for NULL */
#include <stdbool.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
//#include "lock_dev_libs.h"

//#include "stm32xxx_hal.h"

#define ROMABLE_DATA
//typedef float float_t;

#ifndef NULL
#error "review  NULL definition or add required include "
#endif

#if !defined(STDINT_H) &&  !defined(_GCC_STDINT_H) && !defined(__STDINT_DECLS) && !defined(_STDINT) && !defined(_STDINT_H)

#pragma message("Please review  type definition of STDINT define for your platform and add to list above ")

 /*
  *  target platform do not provide stdint or use a different #define than above
  *  to avoid seeing the message below addapt the #define list above or implement
  *  all type and delete these pragma
  */

typedef unsigned int uint32_t;
typedef int int32_t;

typedef unsigned short uint16_t;
typedef short int16_t;

typedef unsigned char uint8_t;

typedef signed char int8_t;

#endif /* _STDINT_H */

#if defined(DEBUG)
extern int HTGDVerbose;


//extern int uart_printf(const char *msg, ...);

#define HTGD_LOG(verbose, fmt, ... ) if(verbose <= HTGDVerbose) printf("HTGDLOG " fmt "\n", ##__VA_ARGS__);
//#define HTGD_LOG(verbose, fmt, ... ) if(verbose <= HTGDVerbose) printf("HTGDLOG " fmt "\n");
//#define HTGD_LOG(verbose, fmt, ... ) if(verbose == HTGD_VERBOSE_DEBUG) printf("HTGDLOG " fmt "\n", ##__VA_ARGS__);
//#define HTGD_LOG(verbose, fmt, ... ) (void)0
#else
#define HTGD_LOG(verbose, fmt, ... ) (void)0
//#define HTGD_LOG(verbose, fmt, ... ) printf("HTGDLOG " fmt "\n", ##__VA_ARGS__);
#endif


#define HTGD_VERBOSE_NOLOG 0
#define HTGD_VERBOSE_PROFILING 1 	// Used for profiling info
#define HTGD_VERBOSE_ERROR 1 		// Error messages on fail
#define HTGD_VERBOSE_INFO  2 		// Top level information
#define HTGD_VERBOSE_DEBUG 3 		// Full information about execution

#define HTGD_PROFILE(fmt, ...) HTGD_LOG(HTGD_VERBOSE_PROFILING, "PRO " fmt, ##__VA_ARGS__)
#define HTGD_ERROR(fmt, ...) HTGD_LOG(HTGD_VERBOSE_ERROR, "ERR " fmt, ##__VA_ARGS__)
#define HTGD_INFO(fmt, ...)  HTGD_LOG(HTGD_VERBOSE_INFO , "INF " fmt, ##__VA_ARGS__)
#define HTGD_DEBUG(fmt, ...) HTGD_LOG(HTGD_VERBOSE_DEBUG, "DBG " fmt, ##__VA_ARGS__)

	
#endif /* HTGD_PLATFORM_H_ */
