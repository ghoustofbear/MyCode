#include "gb28181_common.h"

using namespace std;

static int g_ssrc = 1;

string GenerateSSRC(const string &addrcode)
{
    char str[32] = { 0 };
    sprintf(str,  "%04d", g_ssrc++);
    if (g_ssrc > 9999)
    {
        g_ssrc = 1;
    }

    string ssrc = "ABBBBBCCCC";
    ssrc.replace(0, 0, "0");
    ssrc.replace(1, 5, addrcode.substr(3,7));
    ssrc.replace(6, 9, str);
    return ssrc;
}