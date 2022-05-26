/**
 * (C) Copyright 2013, XXXX Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Tue Feb 26 16:45:16 PST 2013
 *
 * inifile.h
 */
#ifndef _INIFILE
#define _INIFILE
#include "stream.h"

class CM_API IniFile 
{
public:
	bool	openReadOnly(const char *);
	bool	openWriteReplace(const char *);
	bool	openWriteAppend(const char *);
	void	close();

	bool	readNext();

	bool	isName(const char *);
	bool    isNameLike(const char *str);
	char *	getName();
	int		getIntValue();
	char *	getStrValue();
	bool	getBoolValue();

	void	writeSection(const char *);
	void	writeIntValue(const char *, int);
	void	writeStrValue(const char *, const char *);
	void	writeBoolValue(const char *, int);
	void	writeLine(const char *);


	FileStream	fStream;
	char	currLine[256];
	char	*nameStr,*valueStr;
};

#endif
