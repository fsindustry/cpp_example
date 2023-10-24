//
// Created by fsindustry on 23-10-24.
//


#include <cstdio>
#include <bits/socket_type.h>
#include <bits/socket.h>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

#define DNS_SVR             "114.114.114.114"


#define DNS_HOST            0x01
#define DNS_CNAME           0x05

#define ASYNC_EVENT_LENGTH      1024

struct dns_header {
  unsigned short id;
  unsigned short flags;
  unsigned short qdcount;
  unsigned short ancount;
  unsigned short nscount;
  unsigned short arcount;
};

struct dns_question {
  int length;
  unsigned short qtype;
  unsigned short qclass;
  char *qname;
};

struct dns_item {
  char *domain;
  char *ip;
};


void dns_create_header(dns_header *pHeader) {

}

void dns_create_question(dns_question *pQuestion, const char *domain) {

}


int dns_build_request(dns_header *pHeader, dns_question *pQuestion, char request[1024]) {
  return 0;
}


void dns_parse_response(char buffer[1024], dns_item **pItem) {

}


int dns_client_commit(const char *domain) {

  printf("url:%s\n", domain);

  // create socket for DNS client with UDP
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("create socket fialed\n");
    exit(-1);
  }

  // connect DNS server
  sockaddr_in dest;
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(53);
  dest.sin_addr.s_addr = inet_addr(DNS_SVR);
  int ret = connect(sockfd, (struct sockaddr *) &dest, sizeof(dest));
  printf("connect: %d\n", ret);

  struct dns_header header = {0};
  dns_create_header(&header);

  struct dns_question question = {0};
  dns_create_question(&question, domain);

  char request[1024] = {0};
  int req_len = dns_build_request(&header, &question, request);
  int slen = sendto(sockfd, request, req_len, 0, (struct sockaddr*)&dest, sizeof(struct sockaddr));

  char buffer[1024] = {0};
  struct sockaddr_in addr;
  size_t addr_len = sizeof(struct sockaddr_in);

  int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, (socklen_t*)&addr_len);

  printf("recvfrom n: %d\n",n);;
  struct dns_item *domains = NULL;
  dns_parse_response(buffer, &domains);

  return 0;
}



// domain to test
char *domain[] = {
    "www.baidu.com",
    "tieba.baidu.com",
    "news.baidu.com",
    "zhidao.baidu.com",
    "music.baidu.com",
    "image.baidu.com",
    "v.baidu.com",
    "map.baidu.com",
    "baijiahao.baidu.com",
    "xueshu.baidu.com",
    "cloud.baidu.com",
    "www.163.com",
    "open.163.com",
    "auto.163.com",
    "gov.163.com",
    "money.163.com",
    "sports.163.com",
    "tech.163.com",
    "edu.163.com",
    "www.taobao.com",
    "q.taobao.com",
    "sf.taobao.com",
    "yun.taobao.com",
    "baoxian.taobao.com",
    "www.tmall.com",
    "suning.tmall.com",
    "www.tencent.com",
    "www.qq.com",
    "www.aliyun.com",
    "www.ctrip.com",
    "hotels.ctrip.com",
    "hotels.ctrip.com",
    "vacations.ctrip.com",
    "flights.ctrip.com",
    "trains.ctrip.com",
    "bus.ctrip.com",
    "car.ctrip.com",
    "piao.ctrip.com",
    "tuan.ctrip.com",
    "you.ctrip.com",
    "g.ctrip.com",
    "lipin.ctrip.com",
    "ct.ctrip.com"
};


int main(int argc, char **argv) {
  int count = sizeof(domain) / sizeof(domain[0]);
  for (int i = 0; i < count; i++) {
    dns_client_commit(domain[i]);
  }
  getchar();
}

