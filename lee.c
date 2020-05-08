#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/udp.h>

void *Recv_thread(void*);

unsigned short csum(unsigned short *buf, int nwords)
{
  unsigned long sum;
  for(sum=0; nwords>0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum &0xffff);
  return (unsigned short)(~sum);
}

int main(int argc, char const *argv[])
{
    pthread_t th;
    int th_id;

    // th_id = pthread_create(&th, NULL, Recv_thread, NULL);
    // if(th_id < 0)
    // {
    //     perror("thread create Fail\n");
    //     exit(0);
    // }
    // sleep(1);

    u_int16_t udp_src_port, udp_dst_port;
    u_int32_t udp_src_addr, udp_dst_addr;

    udp_src_addr = inet_addr("192.168.225.144");
    udp_dst_addr = inet_addr("127.0.0.1");
    udp_src_port = 8000;
    udp_dst_port = 6000;

    int sd;
    int val = 1;
    char buffer[100] = {0, };
    char data[10] = {'1','2','3','4','5','6','7','8','9','0'};
    struct sockaddr_in sin;
    struct iphdr *ipheader = (struct iphdr *)buffer;
    struct udphdr *udpheader = (struct udphdr *)(buffer + sizeof(struct iphdr));

    sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(sd < 0)
    {
        perror("socket Error");
        exit(2);
    }

    printf("RAW UDP Socket Created\n");

    if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, &val, sizeof(val)) < 0)
    {
        perror("setcockopt Error");
        exit(2);
    }

    printf("Set Socket Option Completed\n");

    sin.sin_family = AF_INET;
    sin.sin_port = htons(5000);
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");

    // memset((char *)ipheader, 0, 20);

    udpheader->source = htons(udp_src_port);
    udpheader->dest = htons(udp_dst_port);
    udpheader->len = htons(sizeof(udpheader) + sizeof(data));

    // source IP address, can use spoofed address here
    ipheader->saddr = inet_addr("192.168.225.144");
    ipheader->daddr = inet_addr("127.0.0.1");

    ipheader->ihl      = 5;
    ipheader->version  = 4;
    ipheader->tos      = 16; // low delay
    ipheader->tot_len  = sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(data);
    ipheader->id       = htons(udp_src_port);
    ipheader->ttl      = 64; // hops
    ipheader->protocol = 17; // UDP

    memcpy(&buffer[sizeof(struct iphdr) + sizeof(struct udphdr)], data, sizeof(data));

    // udpheader->check = csum((unsigned short *)buffer,sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(data));
    // ipheader->check = csum((unsigned short *)buffer, /*sizeof(struct iphdr) + */sizeof(struct udphdr) + sizeof(data));
    ipheader->check = csum((unsigned short *)buffer, ipheader->tot_len);
    printf("\n==========buffer printing\n");
    int i = 0;
    for(i = 0; i < ipheader->tot_len; i++)
    {
        printf("%02x ", buffer[i]);
    }
    printf("\n=========================\n");

    int tcpSocket;
    if((tcpSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("TCP Socke error\n");
        exit(1);
    }

    if(connect(tcpSocket, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("Connect Error\n");
        exit(1);
    }

    write(tcpSocket, buffer, ipheader->tot_len);

    // if (sendto(sd, buffer, ipheader->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    // {
    //     perror("sendto() Error");
    //     exit(3);
    // }

    printf("Send Completed\n");

    close(tcpSocket);

    sleep(10);

    return 0;
}

void *Recv_thread(void* arg)
{
    printf("Receive Thread Created\n");

    int sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if(sd < 0)
    {
        perror("Recv socket Error\n");
        exit(2);
    }

    char recvPacket[3000] = {0, };
    int size = 0;

    struct sockaddr_in Addr;

    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    Addr.sin_port = htons(8888);

    while(1)
    {
        printf("Waiting recv\n");

        if((size = recvfrom(sd, recvPacket, sizeof(recvPacket), 0, (struct sockaddr *)&Addr, (socklen_t *)sizeof(Addr))) < 0)
        {
            printf("Recv Fail\n");
        }
        printf("Recv Success : size = %d\n", size);

        int i=0;
        for(i = 0; i < size; i++)
        {
            printf("%02x ", recvPacket[i]);
        }
        printf("\n");
    }
}
