#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "main.h"
#include "RawSocket.h"
#include "TcpSocket.h"
#include "UdpSocket.h"
#include <pthread.h>
void* wrapper_tcp_server(void* pInst)//Upload
{

	TcpSocket* pInstance = static_cast<TcpSocket*>(pInst);
	pInstance->openServer();

}
void start_tcp_server(int port)
{

	pthread_t p;
    TcpSocket *ts = new TcpSocket("127.0.0.1",port);
	TcpSocket* o = ts;
	int th1 = pthread_create(&p, NULL, &wrapper_tcp_server, o);

}

void* wrapper_udp_server(void* pInst)//Upload
{

	UdpSocket* pInstance = static_cast<UdpSocket*>(pInst);
	pInstance->openServer();

}
void start_udp_server(int port)
{

	pthread_t p;
    UdpSocket *ts = new UdpSocket("127.0.0.1",port);
	UdpSocket* o = ts;
	int th1 = pthread_create(&p, NULL, &wrapper_udp_server, o);

}

int main()
{
	start_tcp_server(TCP_SERVER_PORT);
	start_udp_server(UDP_SERVER_PORT);
	int cnt = 1;
	int len = 0;
	// TcpSocket ts("127.0.0.1",1234);
	RawSocket rs("127.0.0.1",5678);
	while(1)
	{	
		sleep(1);
		if(cnt++%5 == 0) 
		{
			// std::cout << "Running" << std::endl;
			// ts.send((unsigned char*)"hello\n",1024,TCP_SERVER_PORT);
			rs.send("helloworld!\0", len);
		}
	}
    return 0;
}
