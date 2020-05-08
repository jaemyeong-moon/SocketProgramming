#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "UdpSocket.h"
#include "main.h"
#include "time.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

void UdpSocket::openServer()
{
    int sock;
    struct sockaddr_in addr, client_addr;
    char recv_buffer[BUF_LENGTH];
    unsigned int recv_len;
    unsigned int addr_len;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        return ;
    }
    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(m_port);
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return ;
    }
    while (1)
    {
        printf("waiting for messages\n");
        addr_len = sizeof(client_addr);
        if ((recv_len = recvfrom(sock, recv_buffer, BUF_LENGTH, 0, (struct sockaddr *)&client_addr, &addr_len)) < 0)
        {
            perror("recvfrom ");
            return ;
        }
        recv_buffer[recv_len] = '\0';
        printf("ip : %s\n", inet_ntoa(client_addr.sin_addr));
        printf("received data ; %s\n", recv_buffer);
    }

    close(sock);
}

int UdpSocket::send(unsigned char buf[], int len, int port)
{

    return 0;
}