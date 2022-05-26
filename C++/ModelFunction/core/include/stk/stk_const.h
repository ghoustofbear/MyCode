#ifndef __STK_CONST_H__
#define __STK_CONST_H__


#ifndef SIP_MESSAGE_MAX_LENGTH
#define SIP_MESSAGE_MAX_LENGTH 4000
#endif

#define CRLF                              "\r\n"
#define CR                                "\r\0"
#define LF                                "\n\0"
#define SP                                " \0"

#define STK_COMM_ACCEPT					  "Accept"
#define STK_COMM_ACCEPT_ENCODING		  "Accept-Encoding"
#define STK_COMM_ACCEPT_LANGUAGE		  "Accept-Language"
#define STK_COMM_AUTHORIZATION			  "Authorization"
#define STK_COMM_CSEQ					  "CSeq"

#define STK_COMM_CONTENT_ENCODING		  "Content-Encoding"
#define STK_COMM_CONTENT_LANGUAGE		  "Content-Language"
#define STK_COMM_CONTENT_LENGTH			  "Content-Length"
#define STK_COMM_CONTENT_TYPE			  "Content-Type"
#define STK_COMM_CONTENT_BASE			  "Content-Base"
#define STK_COMM_DATE					  "Date"
#define STK_COMM_USER_AGENT				  "User-Agent"
#define STK_COMM_ETAG                     "ETag"

#define STK_COMM_SERVER					  "Server"
#define STK_COMM_WWW_AUTHENTICATE		  "Www-Authenticate"
#define STK_COMM_CACHE_CONTROL            "Cache-Control"
#define STK_COMM_PUBLIC                   "Public"
#define STK_COMM_VARY                     "Vary"
#define STK_COMM_RANGE                    "Range"
#define STK_COMM_LAST_MODIFIED            "Last-Modified"
#define STK_COMM_IF_MODIFIED_SINCE        "If-Modified-Since"
#define STK_COMM_EXPIRES				  "Expires"

#define STK_HTTP_HOST                     "Host"
#define STK_HTTP_CONNECTION               "Connection"
#define STK_HTTP_LOCATION                 "Location"

#define STK_RTSP_SESSION                  "Session"
#define STK_RTSP_TRANSPORT                "Transport"
#define STK_RTSP_RTP_MESSAGE              "RTP-MESSAGE"
#define STK_RTSP_SCALE                    "Scale"

#define STK_SIP_ALERT_INFO				  "Alert-Info"
#define STK_SIP_ALLOW					  "Allow"
#define STK_SIP_AUTHENTICATION_INFO		  "Authentication-Info"
#define STK_SIP_CALL_ID					  "Call-Id"
#define STK_SIP_CALL_ID_SHORT			  "i"
#define STK_SIP_CALL_INFO				  "Call-Info"
#define STK_SIP_CONTACT					  "Contact"
#define STK_SIP_CONTACT_SHORT			  "m"
#define STK_SIP_CONTENT_DISPOSITION		  "Content-Disposition"
#define STK_SIP_CONTENT_ENCODING_SHORT	  "e"
#define STK_SIP_CONTENT_LENGTH_SHORT	  "l"
#define STK_SIP_CONTENT_TYPE_SHORT		  "c"
#define STK_SIP_ERROR_INFO				  "Error-info"
#define STK_SIP_FROM					  "From"
#define STK_SIP_FROM_SHORT				  "f"
#define STK_SIP_IN_REPLY_TO				  "In-Reply-To"
#define STK_SIP_MAX_FORWARDS			  "Max-Forwards"
#define STK_SIP_MIME_VERSION			  "Mime-Version"
#define STK_SIP_MIN_EXPIRES				  "Min-Expires"
#define STK_SIP_ORGANIZATION			  "Organization"
#define STK_SIP_PRIORITY				  "Priority"
#define STK_SIP_PROXY_AUTHENTICATE		  "Proxy-Authenticate"
#define STK_SIP_PROXY_AUTHENTICATION_INFO "Proxy-Authentication-info"
#define STK_SIP_PROXY_AUTHORIZATION		  "Proxy-Authorization"
#define STK_SIP_PROXY_REQUIRE			  "Proxy-Require"
#define STK_SIP_RECORD_ROUTE			  "Record-Route"
#define STK_SIP_REPLY_TO				  "Reply-To"
#define STK_SIP_REQUIRE					  "Require"
#define STK_SIP_RETRY_AFTER				  "Retry-After"
#define STK_SIP_ROUTE					  "Route"
#define STK_SIP_SUBJECT					  "Subject"
#define STK_SIP_SUBJECT_SHORT			  "s"
#define STK_SIP_SUPPORTED				  "Supported"
#define STK_SIP_TIMESTAMP				  "Timestamp"
#define STK_SIP_TO						  "To"
#define STK_SIP_TO_SHORT				  "t"
#define STK_SIP_UNSUPPORTED				  "Unsupported"
#define STK_SIP_VIA						  "Via"
#define STK_SIP_VIA_SHORT				  "v"
#define STK_SIP_WARNING					  "Warning"
#define STK_SIP_ACCESSCODE				  "Accesscode"

#endif
