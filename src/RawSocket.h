#include <iostream>
#include "main.h"

class RawSocket
{
    std::string m_addr;
    int m_port; // 소켓 열 때 사용할 프로토콜 번호.
    public:
    RawSocket(std::string addr, int port)
    {
        m_addr = addr;
        m_port = port;
    }
    void openServer();
    int send(char buf[], int &len);
    int raw_send(char buf[], int len);

    
};