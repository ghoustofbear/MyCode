/**
 * (C) Copyright 2013, XXXX Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Feb 26 16:45:16 PST 2013
 *
 * Common debug info define.
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

/* print */
/*		printf("<%20s>\t[%20s]\t", MOD, __FUNCTION__);  \*/ \
#ifdef _WIN32
#ifdef  DEBUG
#undef  DEBUG
#define DEBUG(MOD, MSG) printf
#endif
#else
#ifndef NDEBUG
#define DEBUG(pszModeName, pszFmt, ...) \
	do {\
		fprintf(stderr,"[%10s] \t[%s] "pszFmt,pszModeName,__func__, ##__VA_ARGS__);\
		fflush(stderr);\
	}while(0)
#else   
#define DEBUG(MOD, MSG...)
#endif//NDEBUG
#endif//_WIN32


/* ASSERT */
#ifdef _WIN32
#undef  ASSERT
#define ASSERT(exp) ( (void)((exp)?1:( printf("ASSERT failed!\n"), abort(), 0 )) )
#else
#define ASSERT(exp) ( (void)((exp)?1:( printf("ASSERT failed: func: [%s], line [%d], file: [%s]\n", \
					__func__, __LINE__,__FILE__),abort(), 0)) )
#endif

#endif 

