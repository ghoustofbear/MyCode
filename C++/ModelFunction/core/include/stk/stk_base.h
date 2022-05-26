/**
 * @file      stk_base.h
 * @copyright Copyright (c) 2017, UT Technology Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-09 13:56:15
 *
 * @note
 *  stk_base.h defines
 */
#ifndef __STK_BASE_H__
#define __STK_BASE_H__
#include "stk_exports.h"
#include <stdint.h>
#include <stdlib.h>
#include <string>
using namespace std;

#ifdef _WIN32
#pragma warning( disable: 4251 )
#endif

template <class T>
class _TNode
{
public:
    _TNode()
    {
        data = NULL;
        next = NULL;
    }

    ~_TNode() {}

public:
    T      * data;
    _TNode * next;
};

template <class T>
class _TList
{
public:
    _TList();
    ~_TList();

    inline int32_t size() const
    {
        return nb_elt;
    }

    bool           eol(int32_t pos) const;
    int32_t        add(T *data, int32_t pos);
    T            * get(int32_t pos) const;
    int32_t        remove(int32_t pos);
    void           clear();

private:
    int32_t        nb_elt;
    _TNode<T> *    head;
};


template <class T>
_TList<T>::_TList()
{
    nb_elt = 0;
    head = NULL;
}

template <class T>
_TList<T>::~_TList()
{
    clear();
}

template <class T>
void _TList<T>::clear()
{
    int32_t pos = 0;

    while (!eol(pos))
    {
        T* data = get(pos);
        remove(pos);
        delete data;
    }

    nb_elt = 0;
}

template <class T>
bool _TList<T>::eol(int32_t pos) const
{
    if (pos < nb_elt)
    {
        return false;                                      /* not end of list */
    }
    else
    {
        return true;                                       /* end of list */
    }
}

template <class T>
int32_t _TList<T>::add(T *data, int32_t pos)
{
    _TNode<T> *ntmp;

    if (nb_elt == 0)
    {
        head = new _TNode<T>;

        if (head == NULL)
        {
            return -1;
        }

        head->data = data;
        head->next = NULL;
        nb_elt++;
        return 0;
    }

    if (pos == -1 || pos >= nb_elt)
    {
        /* insert at the end  */
        pos = nb_elt;
    }

    ntmp = head;              /* exist because nb_elt>0  */

    if (pos == 0)                 /* pos = 0 insert before first elt  */
    {
        head = new _TNode<T>;

        if (head == NULL)
        {
            /* leave the list unchanged */
            head = ntmp;
            return -1;
        }

        head->data = data;
        head->next = ntmp;
        nb_elt++;
        return 0;
    }

    int32_t i = 0;

    while (pos > i + 1)
    {
        i++;
        /* when pos>i next node exist  */
        ntmp = (_TNode<T> *) ntmp->next;
    }

    /* if pos==nb_elt next node does not exist  */
    if (pos == nb_elt)
    {
        ntmp->next = new _TNode<T>;

        if (ntmp->next == NULL)
        {
            return -1;    /* leave the list unchanged */
        }

        ntmp = (_TNode<T> *) ntmp->next;
        ntmp->data = data;
        ntmp->next = NULL;
        nb_elt++;
        return 0;
    }

    /* here pos==i so next node is where we want to insert new node */
    {
        _TNode<T> *nextnode = (_TNode<T> *) ntmp->next;

        ntmp->next = new _TNode<T>;

        if (ntmp->next == NULL)
        {
            /* leave the list unchanged */
            ntmp->next = nextnode;
            return 0;
        }

        ntmp = (_TNode<T> *) ntmp->next;
        ntmp->data = data;
        ntmp->next = nextnode;
        nb_elt++;
    }
    return 0;
}

template <class T>
T* _TList<T>::get(int32_t pos) const
{
    _TNode<T> *ntmp;

    if (pos < 0 || pos >= nb_elt)
        /* element does not exist */
    {
        return NULL;
    }


    ntmp = head;              /* exist because nb_elt>0 */

    int32_t i = 0;

    while (pos > i)
    {
        i++;
        ntmp = (_TNode<T> *) ntmp->next;
    }

    return ntmp->data;
}

template <class T>
int32_t _TList<T>::remove(int32_t pos)
{
    _TNode<T> *ntmp;
    int32_t i = 0;

    if (pos < 0 || pos >= nb_elt)
        /* element does not exist */
    {
        return -1;
    }

    ntmp = head;              /* exist because nb_elt>0 */

    if (pos == 0)
    {
        /* special case  */
        head = (_TNode<T> *) ntmp->next;
        nb_elt--;
        delete ntmp;
        return 0;
    }

    while (pos > i + 1)
    {
        i++;
        ntmp = (_TNode<T> *) ntmp->next;
    }

    /* delete the node */
    {
        _TNode<T> *remnode;

        remnode = (_TNode<T> *) ntmp->next;
        ntmp->next = ((_TNode<T> *) ntmp->next)->next;
        delete remnode;
        nb_elt--;
    }
    return 0;
}

enum
{
    STK_FD_NAME_UNKNOW = 0,

    STK_FD_NAME_ACCEPT,
    STK_FD_NAME_ACCEPT_ENCODING,
    STK_FD_NAME_ACCEPT_LANGUAGE,
    STK_FD_NAME_AUTHORIZATION,
    STK_FD_NAME_CSEQ = 5,

    STK_FD_NAME_CONTENT_ENCODING,
    STK_FD_NAME_CONTENT_LANGUAGE,
    STK_FD_NAME_CONTENT_LENGTH,
    STK_FD_NAME_CONTENT_TYPE,
    STK_FD_NAME_CONTENT_BASE = 10,

    STK_FD_NAME_DATE,
    STK_FD_NAME_USER_AGENT,
    STK_FD_NAME_SERVER,
    STK_FD_NAME_WWW_AUTHENTICATE,
    STK_FD_NAME_HOST = 15,

    STK_FD_NAME_CONNECTION,
    STK_FD_NAME_LOCATION,
    STK_FD_NAME_PUBLIC,
    STK_FD_NAME_RANGE,
    STK_FD_NAME_VARY = 20,

    STK_FD_NAME_SESSION,
    STK_FD_NAME_TRANSPORT,
    STK_FD_NAME_RTP_MESSAGE,
    STK_FD_NAME_SCALE,
    STK_FD_NAME_CACHE_CONTROL,
    STK_FD_NAME_BODY,

};

class MsgParam
{

public:
    MsgParam()
    {
        m_name = m_value = "";
    }
    MsgParam(const char* szName, const char* szValue)
    {
        m_name  = (szName)  ? szName  : "";
        m_value = (szValue) ? szValue : "";
    }
    ~MsgParam()
    {
        m_name = m_value = "";
    }

public:
    string		       m_name;    	                       /**< parameter name */
    string		       m_value;       	                   /**< parameter value */
};

class MsgParamList
{
public:
    MsgParamList();
    ~MsgParamList();

    MsgParamList     & operator= (const MsgParamList &rSrc);

    MsgParam         * get_byname(const char *strName) const;

    void               clear();
    int32_t            size() const;

    int32_t            parse_params(string szParams);
    int32_t            to_str(string & strBuffer, bool bAllowValueNull = true);
    int32_t            add(const string & szName, const string & szValue);
    int32_t            to_str_escape_uri_param(string &str);
    int32_t            to_str_escape_header_param(string &str);

private:
    void	           uri_escape_uri_param(const char * szSrc, string& strDest) const;
    void	           uri_escape_header_param(const char * szSrc, string& strDest) const;

private:
    _TList<MsgParam>   m_ParamList;
};

class STK_API MsgField
{
public:
    MsgField()
    {
        m_uSessionID = 0;
        m_uTypeID    = 0;
    };

    MsgField(uint32_t uSession)
    {
        m_uSessionID = uSession;
        m_uTypeID = 0;
    };

    MsgField(uint32_t uSession, uint32_t uTypeID)
    {
        m_uSessionID = uSession;
        m_uTypeID    = uTypeID;
    };

    virtual ~MsgField() {}


    virtual void 	clear() = 0;
    virtual bool	isvalid() const = 0;
    virtual int32_t to_str(string &strField) = 0;
    virtual int32_t	parse(const char * szBuf, int32_t nBufLen) = 0;


    const  char   * GetTypeName();

    inline void		SetType(uint32_t uType)
    {
        m_uTypeID    = uType;
    };
    inline void		SetSession(uint32_t uSession)
    {
        m_uSessionID = uSession;
    };

    inline uint32_t	GetType()
    {
        return m_uTypeID;
    };
    inline uint32_t	GetSession()
    {
        return m_uSessionID;
    };


    MsgField & operator= (const MsgField &src);

private:
    uint32_t		m_uTypeID;
    uint32_t		m_uSessionID;
};

template <class T> struct _override_t {};
class STK_API MsgFieldList
{
public:
    MsgFieldList();
    ~MsgFieldList();

    inline bool eol(int32_t pos) const
    {
        return m_listField.eol(pos);
    }

    MsgField  * get(int32_t pos);

    void        clear();
    int32_t     size() const
    {
        return m_listField.size();
    }

    int32_t     push(MsgField *field);
    int32_t     add(MsgField *field);
    int32_t     remove(int32_t pos);
    int32_t     to_str(string &str);


    /*
     * VC6 下不支持函数模板显式实例化, 参考
     * http://jeffreyloo.blog.163.com/blog/static/121761670201061714726369/
     */
    template <class T>
    int32_t assign(const MsgFieldList &li, _override_t<T>)
    {
        clear();

        int32_t pos = 0;

        while (!li.m_listField.eol(pos))
        {
            T *header;
            T *header2;

            header = (T *) li.m_listField.get(pos);
            header2 = new T(*header);

            if (NULL == header2)
            {
                return -1;
            }

            m_listField.add(header2, -1);
            pos++;
        }

        return 0;
    }
private:
    MsgFieldList & operator = (const MsgFieldList &src);

private:
    _TList<MsgField> m_listField;
};


#endif
