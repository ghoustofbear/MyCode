#include <iostream>
#include <csignal>
#include <ctime>
#include "Server.h"

Server server;
pthread_t msg1[MAX_CLIENT];
int num_message = 0;
int time_send   = 2700;

void close_app(int s) {
	server.closed();
	exit(0);
}

void * send_client(void * m) {
        struct descript_socket *desc = (struct descript_socket*) m;

	while(1) {
		if(!server.is_online() && server.get_last_closed_sockets() == desc->id) {
			std::cerr << "Connessione chiusa: stop send_clients( id:" << desc->id << " ip:" << desc->ip << " )"<<std:: endl;
			break;
		}
		std::time_t t = std::time(0);
		std::tm* now = std::localtime(&t);
		int hour = now->tm_hour;
		int min  = now->tm_min;
		int sec  = now->tm_sec;

		std::string date = 
			    std::to_string(now->tm_year + 1900) + "-" +
			    std::to_string(now->tm_mon + 1)     + "-" +
			    std::to_string(now->tm_mday)        + " " +
			    std::to_string(hour)                + ":" +
			    std::to_string(min)                 + ":" +
			    std::to_string(sec)                 + ":" +
                "服务端已经收到客户端消息"              +":"+
                desc->message                       +"\r\n";
		std::cerr << date << std::endl;
		server.Send(date, desc->id);
    
		sleep(time_send);
	}
	pthread_exit(NULL);
	return 0;
}

void * received(void * m)
{
    
	std::vector<descript_socket> desc;
	while(1)
	{
		desc = server.getMessage();
		for(unsigned int i = 0; i < desc.size(); i++) {
			
			
				if(!desc[i].enable_message_runtime) 
				{
					desc[i].enable_message_runtime = true;
			        //         if( pthread_create(&msg1[num_message], NULL, send_client, (void *) desc[i]) == 0) {
					// 	std::cerr << "服务端ATTIVA THREAD INVIO MESSAGGI" <<std:: endl;
					// }
					num_message++;
					// start message background thread
				}
				std::cout << "id:      " << desc[i].id      << std::endl
				     << "ip:      " << desc[i].ip      << std::endl
				     << "message: " << desc[i].message << std::endl
				     << "socket:  " << desc[i].socketfd  << std::endl
				     << "enable:  " << desc[i].enable_message_runtime << std::endl;
				server.clean(i);
			
		}
		usleep(1000);
	}
	return 0;
}

int main(int argc, char **argv)
{

	std::signal(SIGINT, close_app);
	
	
        std::vector<int> opts = { SO_REUSEPORT, SO_REUSEADDR };

	if( server.SetUp("192.168.69.246",8000,IPPROTO_TCP) ) {

	}
	else
		std::cerr << "Errore apertura socket" << std::endl;
	
}
