#include <stdlib.h>
#include "inifile.h"
// -----------------------------------
char *trimstr(char *s1)
{
	while (*s1)
	{
		if ((*s1 == ' ') || (*s1 == '\t'))
			s1++;
		else
			break;

	}

	char *s = s1;

	s1 = s1+strlen(s1);

	while (*--s1)
		if ((*s1 != ' ') && (*s1 != '\t'))
			break;

	s1[1] = 0;

	return s;
}

// -----------------------------------
char *stristr(const char *s1, const char *s2)
{
	while (*s1)
	{
		if (TOUPPER(*s1) == TOUPPER(*s2))
		{
			const char *c1 = s1;
			const char *c2 = s2;

			while (*c1 && *c2)
			{
				if (TOUPPER(*c1) != TOUPPER(*c2))
					break;
				c1++;
				c2++;
			}
			if (*c2==0)
				return (char *)s1;
		}

		s1++;
	}
	return NULL;
}

void	openReadOnly(const char *);
void	openWriteReplace(const char *);
// -----------------------------------------
bool IniFile::openReadOnly(const char *fn)
{
	try 
	{
		fStream.openReadOnly(fn);
	}catch(StreamException &)
	{
		return false;
	}
	return true;
}
// -----------------------------------------
bool IniFile::openWriteReplace(const char *fn)
{
	try 
	{
		fStream.openWriteReplace(fn);
#ifndef _WIN32
		fStream.writeCRLF = false;
#endif

	}catch(StreamException &)
	{
		return false;
	}
	return true;
}

// -----------------------------------------
bool IniFile::openWriteAppend(const char *fn)
{
	try 
	{
		fStream.openWriteAppend(fn);
#ifndef _WIN32
		fStream.writeCRLF = false;
#endif

	}catch(StreamException &)
	{
		return false;
	}
	return true;
}

// -----------------------------------------
void IniFile::close()
{
	fStream.close();
}


// -----------------------------------------
bool	IniFile::readNext()
{
	if (fStream.eof())
		return false;

	try
	{
		fStream.readLine(currLine, 256);
	}catch(StreamException &)
	{
		return false;
	}


	// find end of value name and null terminate
	char *nend = strstr(currLine, "=");

	if (nend)
	{
		*nend = 0;
		valueStr = trimstr(nend+1);
		nameStr = trimstr(currLine);
	}else
	{
		valueStr = NULL;
		nameStr  = currLine;
	}

	return true;
}
// -----------------------------------------
bool IniFile::isName(const char *str)
{
	return stricmp(getName(),str)==0;
}

// -----------------------------------------
bool IniFile::isNameLike(const char *str)
{
	return strnicmp(getName(), str, strlen(str) - 1)==0;
}

// -----------------------------------------
char *	IniFile::getName()
{
	return nameStr;
}
// -----------------------------------------
int		IniFile::getIntValue()
{
	if (valueStr)
		return atoi(valueStr);
	else
		return 0;
}
// -----------------------------------------
char *	IniFile::getStrValue()
{
	if (valueStr)
		return valueStr;
	else
		return NULL;
}
// -----------------------------------------
bool	IniFile::getBoolValue()
{
	if (!valueStr)
		return false;


	if ( (stricmp(valueStr,"yes")==0) ||
			(stricmp(valueStr,"y")==0) ||
			(stricmp(valueStr,"1")==0) )
		return true;

	return false;
}

// -----------------------------------------
void	IniFile::writeIntValue(const char *name, int iv)
{
	sprintf(currLine,"%s = %d",name,iv);
	fStream.writeLine(currLine);
}
// -----------------------------------------
void	IniFile::writeStrValue(const char *name, const char *sv)
{
	sprintf(currLine,"%s = %s",name,sv);
	fStream.writeLine(currLine);
}
// -----------------------------------------
void	IniFile::writeSection(const char *name)
{
	fStream.writeLine("");
	sprintf(currLine,"[%s]",name);
	fStream.writeLine(currLine);
}
// -----------------------------------------
void	IniFile::writeBoolValue(const char *name, int v)
{
	sprintf(currLine,"%s = %s",name,(v!=0)?"Yes":"No");
	fStream.writeLine(currLine);
}
// -----------------------------------------
void	IniFile::writeLine(const char *str)
{
	fStream.writeLine(str);
}
