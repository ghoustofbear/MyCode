/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stack utility defines.
 */
#ifndef __STK_UTILITY_H__
#define __STK_UTILITY_H__

#include "stk_exports.h"
#include <stdlib.h>
#include <string>

STK_API void   stk_util_replace_all_lws(char *message);
STK_API int    stk_find_next_crlf(const char * start_of_header, const char ** end_of_header);
STK_API char * stk_next_separator(const char *ch, int separator_osip_to_find, int before_separator);

STK_API char * stk_strnappend(char *dst, const char *src, size_t len);
STK_API char * stk_strappend(char *dst, const char *src);
STK_API char * stk_clrncpy(char *dst, const char *src, size_t len);
STK_API int	   stk_clrspace(char *word);
STK_API int	   stk_clrquote(char *word);
STK_API int    stk_tolower(char *word);

STK_API char * stk_quote_find(const char *qstring);
STK_API int	   stk_quoted_string_set(const char *name, const char *str, std::string& reslut, const char **next);

STK_API void   stk_uri_unescape(char *str);

#endif
