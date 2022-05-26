#ifndef  _BASE64_H_
#define  _BASE64_H_

#include <string>
using namespace std;

class ZBase64
{
public:

	string Encode(const unsigned char* Data,int DataByte);

	string Decode(const char* Data,int DataByte,int& OutByte);
};

#endif
