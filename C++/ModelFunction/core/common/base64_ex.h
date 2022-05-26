#include "def.h"

CM_API std::string base64_encode(const unsigned char* Data, int DataByte);

CM_API std::string base64_decode(const char* Data, int DataByte, int& OutByte);
