#include"Client.h"
#include <iostream>
#include <signal.h>
 Client client;
void sig_exit(int s)
{
	client.exit();
	exit(0);
}
int main(int argc, char const *argv[])
{
    /* code */
    
    if(argc != 4) {
		std::cerr << "Usage: ./client ip port message" <<std:: endl;
		return 0;
	}
	signal(SIGINT, sig_exit);

	client.Setup("192.168.69.246",8000,IPPROTO_TCP);
	while(1)
	{
        std::string send="这里是客户端，收到请回答 over,over!";
		client.Send(send);
		std::string rec = client.receive();
		if( rec != "" )
		{
			std::cout << rec << std::endl;
		}
		sleep(1);
	}
	return 0;
    
}
