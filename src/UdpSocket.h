#include <iostream>
#include "main.h"

class UdpSocket
{
    std::string m_addr;
    int m_port;
    public:
    UdpSocket(std::string addr, int port)
    {
        m_addr = addr;
        m_port = port;
        std::cout << "UDP :: Addr : " << addr << " port : " << port <<std::endl;
    }
    void openServer();
    int send(unsigned char buf[], int len, int port); 
};