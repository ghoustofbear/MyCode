#ifndef ___EXCEPTION__
#define ___EXCEPTION__

#ifdef _WIN32
#define __FUNCTION__ "__FUNCTION__"
#endif

#define SHOW_NOT_IMPLE_FUNC printf("FUNC = %s not impletion!\n",  __FUNCTION__)
#include <string>
using namespace std;
class Exception
{
public:
    explicit Exception() 
    {
    }

    Exception(const char* info) 
    {
        errinfo = info;
    }
    
    ~Exception() {}
        
public:     
    const string what() const
    {   
        return errinfo;
    }

private:                       
    string errinfo;            
};
                               
// ----------------------------------
class GeneralException
{
public:
    GeneralException(const char *m, int e = 0) 
	{
		strcpy(msg,m);
		err=e;
	}
    char msg[128];
	int err;
};

// -------------------------------------
class StreamException : public GeneralException
{
public:
	StreamException(const char *m) : GeneralException(m) {}
	StreamException(const char *m,int e) : GeneralException(m,e) {}

};

#endif      
