#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "RawSocket.h"
#include "main.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <linux/ip.h>
#include <linux/udp.h>

// UDP header's structure

struct udpheader
{
    unsigned short int udph_srcport;
    unsigned short int udph_destport;
    unsigned short int udph_len;
    unsigned short int udph_chksum;
};

unsigned short csum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void RawSocket::openServer()
{
    int sd = socket(PF_INET, SOCK_RAW, 999);
    if (sd < 0)
    {
        perror("socket() error");
    }

    char recvPacket[1024] = {
        0,
    };
    int pkt_size = 0;
    if ((pkt_size = recv(sd, recvPacket, 1024, 0)) < 0)
    {
        perror("recvfrom() failed");
    }

    printf("read succeeded");

    printf("pkt_size = %d\n", pkt_size);
    struct iphdr *iphdr = (struct iphdr *)&recvPacket[0];
    printf("iphdr.ihl = %d\n", iphdr->ihl);
    printf("iphdr.version = %d\n", iphdr->version);
    printf("iphdr.tos = %d\n", iphdr->tos);
    printf("iphdr.tot_len = %d\n", htons(iphdr->tot_len));
    printf("iphdr.frag_off = %d\n", iphdr->frag_off);
    printf("iphdr.ttl = %d\n", iphdr->ttl);
    printf("iphdr.protocol = %d\n", iphdr->protocol);
}

int RawSocket::raw_send(char buf[], int len)
{
    char argv[][20] = {"127.0.0.1",
                       "5002",
                       "127.0.0.1",
                       "6000"};
    struct sockaddr_in sin, din;
    int one = 1;
    const int *val = &one;

    char buffer[BUF_LENGTH];
    memset(buffer, 0, BUF_LENGTH);

    // Create a raw socket with UDP protocol
    int sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sd < 0)
    {
        perror("socket() error");
        // If something wrong just exit
        exit(-1);
    }
    else
        printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");
    // The source is redundant, may be used later if needed
    // The address family
    sin.sin_family = AF_INET;
    din.sin_family = AF_INET;
    // Port numbers
    sin.sin_port = htons(atoi(argv[1]));
    din.sin_port = htons(atoi(argv[3]));
    // IP addresses
    sin.sin_addr.s_addr = inet_addr(argv[0]);
    din.sin_addr.s_addr = inet_addr(argv[2]);
      int cnt = 0;
      std::cout << "===========RAW_SEND================" << std::endl;
        for(int i = 0 ; i < 100 ; i++)
        {
            std::cout << std::hex << (int)buf[i] <<" ";
        }
        std::cout << "===========================================" << std::endl;
     if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    {
        perror("setsockopt() error");
        exit(-1);
    }
    else
        printf("setsockopt() is OK.\n");

      if (sendto(sd, buf, (unsigned int)len, 0, (struct sockaddr *)&din, sizeof(din)) < 0)
        {
            perror("sendto() error");
            exit(-1);
        }
        else
        {
            printf("sendto() is OK.\n" );
            // sleep(2);
        }
    return 0;
}
int RawSocket::send(char buf[], int &len)
{
    char argv[][20] = {"127.0.0.1",
                       "6001",
                       "127.0.0.1",
                       "6000"};
    int sd;

    // No data/payload just datagram
    char buffer[BUF_LENGTH];
    memset(buffer, 0, BUF_LENGTH);
    // Our own headers' structures
    //예제에 나와있는 직접만든 구조체로 ip헤더를 만들었을 때는 정상적으로 동작하지 않았다. Linux에서 지원하는 구조체로 사용하니 정상동작한다. 왜그러지? 
    struct iphdr *ip = (struct iphdr *)buffer; 
    struct udpheader *udp = (struct udpheader *)(buffer + sizeof(struct iphdr));
    char *data = (char *)(buffer + sizeof(struct iphdr)+ sizeof(struct udpheader));
    strcpy(data, buf);
    
    // Source and destination addresses: IP and port
    struct sockaddr_in sin, din;
    int one = 1;
    const int *val = &one;

    // Create a raw socket with UDP protocol
    sd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sd < 0)
    {
        perror("socket() error");
        // If something wrong just exit
        exit(-1);
    }
    else
        printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");
    // The source is redundant, may be used later if needed
    // The address family
    sin.sin_family = AF_INET;
    din.sin_family = AF_INET;
    // Port numbers
    sin.sin_port = htons(atoi(argv[1]));
    din.sin_port = htons(atoi(argv[3]));
    // IP addresses
    sin.sin_addr.s_addr = inet_addr(argv[0]);
    din.sin_addr.s_addr = inet_addr(argv[2]);
    // Fabricate the IP header or we can use the
    // standard header structures but assign our own values.
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 16; // Low delay
    // ip->iph_len = sizeof(struct iphdr) + sizeof(struct udpheader);
    ip->id = htons(54321);
    ip->ttl = 64;      // hops
    ip->protocol = 17; // UDP
    // Source IP address, can use spoofed address here!!!
    ip->saddr = inet_addr(argv[0]);
    // The destination IP address
    ip->daddr = inet_addr(argv[2]);
    // Fabricate the UDP header. Source port number, redundant
    udp->udph_srcport = htons(atoi(argv[1]));
    // Destination port number
    udp->udph_destport = htons(atoi(argv[3]));
    udp->udph_len = htons(sizeof(struct udpheader) + strlen(data));
    // Calculate the checksum for integrity
    ip->check = csum((unsigned short *)buffer, sizeof(struct iphdr) + sizeof(struct udpheader));
    ip->tot_len = sizeof(struct iphdr) + ntohs(udp->udph_len);
    len = ip->tot_len;
    std::cout << "send data : " << data << " / len : " << std::dec << (int) len << std::endl;
    // Inform the kernel do not fill up the packet structure. we will build our own...
    for(int i = 0 ; i < 50 ; i++)
        {
            printf("%02x ",buffer[i]);
        }
        std::cout << std::endl;
    if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
    {
        perror("setsockopt() error");
        exit(-1);
    }
    else
        printf("setsockopt() is OK.\n");
    // Send loop, send fo every 2 second for 100 count
    // printf("Using Source IP: %s port: %u, Target IP: %s port: %u.\n", argv[0], atoi(argv[1]), argv[2], atoi(argv[3]));
    // int count;
    // for (count = 1; count <= 20; count++)
    // {
        if (sendto(sd, buffer, ip->tot_len, 0, (struct sockaddr *)&din, sizeof(din)) < 0)
        {
            perror("sendto() error");
            exit(-1);
        }
        else
        {
            printf("sendto() is OK.\n" );
            // sleep(2);
        }
    // }
    close(sd);
    return 0;
}
