#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "TcpSocket.h"
#include "RawSocket.h"
#include "main.h"
#include "time.h"


#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>


void TcpSocket::openServer()
{
    
    char buffer[BUF_LENGTH];
    struct sockaddr_in server_addr, client_addr;
    char temp[20];
    int server_fd, client_fd;
    //server_fd, client_fd : 각 소켓 번호
    unsigned int len, msg_size;
 
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {// 소켓 생성
        printf("%d : Can't open stream socket\n",m_port);
        exit(0);
    }
    memset(&server_addr, 0x00, sizeof(server_addr));
    //server_Addr 을 NULL로 초기화
 
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(m_port);
    //server_addr 셋팅
 
    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <0)
    {//bind() 호출
        printf("%d : Can't bind local address.\n",m_port);
        exit(0);
    }
 
    if(listen(server_fd, 5) < 0)
    {//소켓을 수동 대기모드로 설정
        printf("%d : Can't listening connect.\n",m_port);
        exit(0);
    }
 
    memset(buffer, 0x00, sizeof(buffer));
    printf("%d : wating connection request.\n",m_port);
    len = sizeof(client_addr);
    while(1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
        if(client_fd < 0)
        {
            printf("%d : accept failed.\n",m_port);
            exit(0);
        }
        inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, temp, sizeof(temp));
        msg_size = read(client_fd, buffer, BUF_LENGTH);       
        std::cout << m_port << " : read " << std::endl;
        int cnt = 0;
        for(int i = 0 ; i < BUF_LENGTH ; i++)
        {
            if(buffer[i] == 0) cnt++;
            else cnt = 0;
            if(cnt > 5) break;
            std::cout << std::hex << (int)buffer[i] <<" ";
        }
        std::cout << std::endl;
        close(client_fd);
        RawSocket rs("127.0.0.1",RAW_SERVER_PORT);
        rs.raw_send(buffer,BUF_LENGTH);


    }
    close(server_fd);
}
int TcpSocket::send(unsigned char buf[], int len, int port)
{
        int re = 0;
        int s, n;
        struct sockaddr_in server_addr;
        //struct sockaddr_in server_addr : 서버의 소켓주소 구조체
	std::cout << "send : " << m_addr <<std::endl;
        if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {//소켓 생성과 동시에 소켓 생성 유효검사
                printf("can't create socket\n");
                exit(0);
        }
 
        bzero((char *)&server_addr, sizeof(server_addr));
        //서버의 소켓주소 구조체 server_addr을 NULL로 초기화
 
        server_addr.sin_family = AF_INET;
        //주소 체계를 AF_INET 로 선택
        server_addr.sin_addr.s_addr = inet_addr(m_addr.c_str());
        //32비트의 IP주소로 변환
        server_addr.sin_port = htons(port);
        //daytime 서비스 포트 번호
 
        if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {//서버로 연결요청
                printf("can't connect.\n");
                exit(0);
        }

        if(write(s, buf,len) == -1) re = -1;
 
        close(s);
        return re;
}
