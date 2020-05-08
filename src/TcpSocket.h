#include <iostream>
#include "main.h"

class TcpSocket
{
    std::string m_addr;
    int m_port;
    public:
    TcpSocket(std::string addr, int port)
    {
        m_addr = addr;
        m_port = port;
        std::cout << "TCP :: Addr : " << addr << " port : " << port <<std::endl;
    }
    void openServer();
    int send(unsigned char buf[], int len, int port); 
};