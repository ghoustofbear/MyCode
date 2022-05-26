#include "sys.h"
#include <algorithm>

uint32_t
Sys::rand()
{
    static unsigned long a[2] = {0x14235465, 0x14235465};

    return RAND(a[0], a[1]);
}

string Sys::StrRemoveChar(const string src, char target)
{
    string ret;
    remove_copy(src.begin(), src.end(), back_inserter(ret), target);
    return ret;
}

bool Sys::StrFindReplace(string &src, const string target, const string replacer, int index)
{
    size_t pos = 0, offset = 0, count = 0;
    while (string::npos != (pos = src.find(target, offset)))
    {
        ++count;
        offset = pos + 1;
        if (index == count)
        {
            src.replace(pos, target.length(), replacer);
            return true;
        }
    }
    return false;
}

int64_t Sys::cmpDatatime(datetime_s &value1, datetime_s &value2)
{
    int64_t sub = 0;
    time_t time[2] = { 0 };
    struct tm t[2];
    datetime_s *v[2] = { 0 };
    v[0] = &value1;
    v[1] = &value2;
    for (int i = 0; i < 2; ++i)
    {
        t[i].tm_year = v[i]->year - 1900;
        t[i].tm_mon = v[i]->month - 1;
        t[i].tm_mday = v[i]->day;
        t[i].tm_hour = v[i]->hour;
        t[i].tm_min = v[i]->minute;
        t[i].tm_sec = v[i]->second;
        t[i].tm_isdst = 0;
        time[i] = mktime(&t[i]);
    }

    sub = time[0] - time[1];
    return sub;
}

uint64_t Sys::StrToDecimal(const string src, const int bit){
    string temp = src;
    long long   sum = 0, b = 1;
    transform(temp.begin(), temp.end(), temp.begin(), ::toupper);

    string::reverse_iterator it = temp.rbegin();
    for (; it != temp.rend(); ++it)
    {
        if (*(it) > '9')
        {
            sum += (*(it)-'A' + 10)*b;
            b *= bit;
        }
        else
        {
            sum += (*(it)-'0')*b;
            b *= bit;
        }
    }
    return sum;
}
bool
GnuID::isSame(GnuID &gid)
{
    for (int i = 0; i < 16; i++)
    {
        if (gid.id[i] != id[i])
        {
            return false;
        }

    }

    return true;
}

bool
GnuID::isSet()
{
    for (int i = 0; i < 16; i++)
    {
        if (id[i] != 0)
        {
            return true;
        }
    }

    return false;
}

void
GnuID::clear()
{
    for (int i = 0; i < 16; i++)
    {
        id[i] = 0;
    }

    storeTime = 0;
}


void
GnuID::generate(unsigned char flags)
{
    clear();

    for (int i = 0; i < 16; i++)
    {
        id[i] = sys->rand();
    }

    id[0] = flags;
}

string
GnuID::toStr()
{
    string str;

    for (int i = 0; i < 16; i++)
    {
        char buf[10] = {0};
        snprintf(buf, sizeof(buf), "%d", id[i]);
        /* str += std::to_string(id[i]); */
        str += buf;

    }

    return str;
}

void
GnuID::fromStr(const string str)
{
    clear();

    if (str.size() < 32)
    {
        return;
    }

    char buf[8];

    buf[2] = 0;

    for (int i = 0; i < 16; i++)
    {
        buf[0] = str[i * 2];
        buf[1] = str[i * 2 + 1];
        id[i] = (unsigned char)strtoul(buf, NULL, 16);
    }
}

