#include "initializer.h"
#include "mempool.h"
#include "log4x.h"

#ifdef _WIN32
#include "wsys.h"
#else
#include "usys.h"
#endif


Sys     * sys     = NULL;
Mempool * mempool = NULL;

Initializer:: Initializer()
{
    m_start = false;

#if 0
#if defined(_WIN32) && defined(_DEBUG)
    AllocConsole();

    FILE* fp = NULL;
    freopen_s(&fp, "CONOUT$", "w", stdout);
#endif
#endif

#ifdef _WIN32
    sys = new WSys();
#else
    sys = new USys();
#endif

    /* NOTE: The creation of threads in the static variables constructor of the dynamic library will block */
    /* log4x::ilog4x::instance()->config("log.ini"); */
    /* log4x::ilog4x::instance()->start(); */
}

Initializer::~Initializer()
{
    if (mempool)
    {
        delete mempool;
        mempool = NULL;
    }

    if (sys)
    {
        delete sys;
        sys = NULL;
    }

#if 0
#if defined(_WIN32) && defined(_DEBUG)
    FreeConsole();
#endif
#endif
}

int
Initializer::start()
{
    AutoLock locker(m_mutex);
    if (m_start)
    {
        LOGI_COM("Common has started!");
        return 0;
    }

    int result = 0;
    char* evnPath = getenv("LOG4X_CONFIG_DIR");
    char cfgPath[128] = {0};
    if (evnPath == NULL)
    {
        strcpy(cfgPath, "log.ini");
    }
    else
    {
        strcpy(cfgPath, evnPath);
        strcat(cfgPath, "log.ini");
    }

    LOGE_COM("LOG4X_CONFIG_DIR:%s\n", cfgPath);

    log4x::ilog4x::instance()->config(cfgPath);
    result += log4x::ilog4x::instance()->start();

#if _DEBUG
    DEBUG_LEVEL_COM(LOG_LEVEL_DEBUG);
#else
    DEBUG_LEVEL_COM(LOG_LEVEL_INFO);
#endif

    if (mempool)
    {
        result += mempool->start();
    }

    if (0 == result)
    {
        m_start = true;

        LOGI_COM("common started");
        
        return 0;
    }

    LOGF_COM("Common start failed!");

    return -1;
}

void
Initializer::stop()
{
    AutoLock locker(m_mutex);
    if (mempool)
    {
        mempool->stop();
    }

    LOGI_COM("common stop");

    log4x::ilog4x::instance()->stop();
    m_start = false;
}



icommon* icommon::m_self = NULL;

icommon *
icommon::instance()
{
    //static Initializer icom;
    //return &icom;

    if (m_self == NULL)
    {
        m_self = new Initializer;
    }
    return m_self;
}

bool
icommon::inited()
{
    Initializer *initializer = (Initializer *)instance();
    AutoLock locker(initializer->m_mutex);

    return initializer->m_start;
}
