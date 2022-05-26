#include "matrix.h"

#define ENABLE_MATRIX_DEBUG
#ifdef  ENABLE_MATRIX_DEBUG 
#ifdef _WIN32
#define MATRIX_DEBUG             printf
#else
#define MATRIX_DEBUG(MSG...)     DEBUG("MATRIX", MSG)
#endif /* end _WIN32 */
#else
#define MATRIX_DEBUG(MSG...)
#endif /* end ENABLE_MATRIX_DEBUG */

Matrix::Matrix(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_HIK;
    m_port = port;
    m_domainType = domainType;
}


Matrix::~Matrix()
{
}
