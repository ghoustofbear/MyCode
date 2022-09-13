#include<iostream>
#include"inifile.h"
using namespace inifile;
int main()
{
	IniFile inifile ;
	std::string filepath = ".\\test.ini";
	inifile.Load(filepath);
	std::string ret ;
	//string db_name = inifile.getValue("COMMON", "DB", ret);
	inifile.GetStringValue("COMMON","PASSWD", &ret);
	std::cout << "ret=" << ret << std::endl;
}//