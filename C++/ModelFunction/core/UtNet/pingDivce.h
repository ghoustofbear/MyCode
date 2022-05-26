#ifndef _PING_DEVICE_
#define _PING_DEVICE_

#ifdef _WIN32

#include <WINSOCK2.H>
#include "proto_def.h"
//#pragma comment(lib, "WS2_32.lib")

/* The IP header */ 
typedef struct iphdr { 
	DWORD   h_len:4;        // length of the header 
	DWORD   version:4;      // Version of IP 
	BYTE    tos;            // Type of service 
	WORD	total_len;      // total length of the packet 
	WORD	ident;          // unique identifier 
	WORD	frag_and_flags; // flags 
	BYTE	ttl; 
	BYTE	proto;          // protocol (TCP, UDP etc) 
	WORD	checksum;       // IP checksum 	
	DWORD   sourceIP; 
	DWORD   destIP; 
	
}IpHeader; 


//  ICMP header / 
typedef struct icmphdr { 
	BYTE   i_type; 
	BYTE   i_code;                /* type sub code */ 
	WORD   i_cksum; 
	WORD   i_id; 
	WORD   i_seq;           	/* This is not the std header, but we reserve space for time */ 
	ULONG  timestamp; 
}IcmpHeader; 

#define ICMP_ECHO 8 
#define ICMP_ECHOREPLY 0 
#define ICMP_MIN 8                      // minimum 8 byte icmp packet (just header) 

#define STATUS_FAILED      FALSE 
#define DEF_PACKET_SIZE    32 
#define DEF_PACKET_NUMBER  4            /* 发送数据报的个数 */ 
#define MAX_PACKET         1024 


#define xmalloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s)) 
#define xfree(p) HeapFree (GetProcessHeap(),0,(p)) 

VOID   fill_icmp_data(PCHAR, S32); 
WORD   checksum(PWORD, S32); 
socklen_t    decode_resp(PCHAR, socklen_t, SA_IN *); 

BOOL PingFunc(PCSTR strDes)
{
	WSADATA wsaData; 
	SOCKET	sockRaw;
	SA_IN   dest,from; 
	S32		bread,datasize,times; 
	S32		fromlen = sizeof(from); 
	S32		timeout = 1000; 
	S32		statistic = 0;  /* 用于统计结果 */  
	PCHAR   dest_ip; 
	PCHAR   icmp_data; 
	PCHAR   recvbuf; 
	DWORD   addr=0; 
	WORD    seq_no = 0; 
	
	if (WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
	{
		return STATUS_FAILED;
	} 

	sockRaw = WSASocket(AF_INET,SOCK_RAW,IPPROTO_ICMP,NULL, 0,WSA_FLAG_OVERLAPPED); 
// 
//注：为了使用发送接收超时设置(即设置SO_RCVTIMEO, SO_SNDTIMEO)， 
//    必须将标志位设为WSA_FLAG_OVERLAPPED ! 
// 

	if (sockRaw == INVALID_SOCKET) 
	{ 
		return STATUS_FAILED; 
	} 

	bread = setsockopt( sockRaw, SOL_SOCKET, SO_RCVTIMEO, (PCHAR)&timeout, sizeof(timeout) ); 

	if(bread == SOCKET_ERROR)
	{
		return STATUS_FAILED;
	} 

	bread = setsockopt(sockRaw,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout, sizeof(timeout)); 

	if(bread == SOCKET_ERROR) 
	{ 
		return STATUS_FAILED;
	} 
	memset(&dest,0,sizeof(dest)); 

	dest.sin_addr.s_addr = inet_addr(strDes); 
    dest.sin_family = AF_INET; 
    dest_ip = inet_ntoa(dest.sin_addr); 

	times = DEF_PACKET_NUMBER; 	
	datasize = DEF_PACKET_SIZE; 	
    datasize += sizeof(IcmpHeader); 
	
    icmp_data = (char*)xmalloc(MAX_PACKET); 
    recvbuf   = (char*)xmalloc(MAX_PACKET); 
	
	if (!icmp_data) 
	{ 
		return STATUS_FAILED;
	} 	
	
	memset(icmp_data,0,MAX_PACKET); 
	fill_icmp_data(icmp_data,datasize); 
		
	for(S32 i = 0; i < times ; ++i)
	{ 
		S32 bwrote; 
		
		((IcmpHeader*)icmp_data)->i_cksum   = 0; 
		((IcmpHeader*)icmp_data)->timestamp = GetTickCount(); 
		
		((IcmpHeader*)icmp_data)->i_seq   = seq_no++; 
		((IcmpHeader*)icmp_data)->i_cksum = checksum((USHORT*)icmp_data,datasize); 
		
		bwrote = sendto(sockRaw,icmp_data,datasize,0,(struct sockaddr*)&dest,sizeof(dest)); 

		if (bwrote == SOCKET_ERROR)
		{ 
			return STATUS_FAILED;
		} 
		if (bwrote < datasize ) 
		{ 
			return STATUS_FAILED;
		} 

		bread = recvfrom(sockRaw,recvbuf,MAX_PACKET,0,(struct sockaddr*)&from,&fromlen); 
		
		if (bread == SOCKET_ERROR)
		{ 
			return STATUS_FAILED;
		} 
		if(-1 != decode_resp(recvbuf,bread,&from)) 
			statistic++;                                   /* 成功接收的数目++ */ 
		Sleep(50); 
	} 
	
	if (times == statistic)
	{
		WSACleanup(); 
		return TRUE; 
	}

	WSACleanup();
	return STATUS_FAILED;
}

/* 
The response is an IP packet. We must decode the IP header to locate 
the ICMP data 
*/ 
socklen_t decode_resp(PCHAR buf, socklen_t bytes,SA_IN* from) { 
	
	IpHeader *iphdr; 
	IcmpHeader *icmphdr; 
	unsigned short iphdrlen; 
	
	iphdr = (IpHeader *)buf; 
	
	iphdrlen = (iphdr->h_len) * 4 ; // number of 32-bit words *4 = bytes 
	
	if (bytes < iphdrlen + ICMP_MIN) 
	{ 
		return STATUS_FAILED;
	} 
	
	icmphdr = (IcmpHeader*)(buf + iphdrlen); 
	
	if (icmphdr->i_type != ICMP_ECHOREPLY)
	{ 
		return STATUS_FAILED; 
	} 
	if (icmphdr->i_id != (USHORT)GetCurrentProcessId())
	{ 
		return STATUS_FAILED; 
	} 
	return TRUE; 	
} 


WORD checksum(PWORD buffer, S32 size)
{ 
	ULONG cksum=0; 
	
	while(size >1) { 
		cksum+=*buffer++; 
		size -=sizeof(WORD); 
	} 
	
	if(size) { 
		cksum += *(PWORD)buffer; 
	} 
	
	cksum = (cksum >> 16) + (cksum & 0xffff); 
	cksum += (cksum >>16); 
	return (WORD)(~cksum); 
} 
/* 
Helper function to fill in various stuff in our ICMP request. 
*/ 
VOID fill_icmp_data(PCHAR icmp_data, S32 datasize)
{ 
	
	IcmpHeader *icmp_hdr; 
	PCHAR datapart; 
	
	icmp_hdr = (IcmpHeader*)icmp_data; 
	
	icmp_hdr->i_type = ICMP_ECHO; 
	icmp_hdr->i_code = 0; 
	icmp_hdr->i_id = (USHORT)GetCurrentProcessId(); 
	icmp_hdr->i_cksum = 0; 
	icmp_hdr->i_seq = 0; 
	
	datapart = icmp_data + sizeof(IcmpHeader); 
	memset(datapart,'E', datasize - sizeof(IcmpHeader)); 
} 

#endif
#endif