//
// Created by fsindustry on 23-10-24.
//


#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>

// the first DNS service address in China
#define DNS_SVR             "114.114.114.114"


#define DNS_HOST            0x01
#define DNS_CNAME           0x05

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


static void dns_parse_name(const char *buffer, unsigned char *string, char aname[128], int *p_int);

int dns_create_header(struct dns_header *header) {

  if (header == NULL) return -1;
  memset(header, 0, sizeof(struct dns_header));

  // wait for a moment
  srandom(time(NULL));

  header->id = random();
  header->flags != htons(0x0100);
  header->qdcount = htons(1);

  return 0;
}

int dns_create_question(struct dns_question *question, const char *hostname) {

  if (question == NULL)return -1;
  memset(question, 0, sizeof(struct dns_question));

  question->qname = (char *) malloc(strlen(hostname) + 2);
  if (question->qname == NULL) return -2;

  question->length = strlen(hostname) + 2;
  question->qtype = htons(1);
  question->qclass = htons(1);

  const char delimiter[2] = ".";
  char *hostname_dup = strdup(hostname);
  char *token = strtok(hostname_dup, delimiter);
  char *qname_p = question->qname;
  while (token != NULL) {
    size_t len = strlen(token);
    *qname_p = len;
    qname_p++;

    strncpy(qname_p, token, len + 1);
    qname_p += len;

    token = strtok(NULL, delimiter);
  }

  free(hostname_dup);
  return 0;
}

static int is_pointer(int in) {
  // 0xC0 == 1100 0000
  return ((in & 0xC0) == 0xC0);
}

int dns_build_request(struct dns_header *header, struct dns_question *question, char *request) {

  int header_s = sizeof(struct dns_header);
  int question_s = question->length + sizeof(question->qtype) + sizeof(question->qclass);

  int length = question_s + header_s;
  int offset = 0;
  memcpy(request + offset, header, sizeof(struct dns_header));
  offset += sizeof(struct dns_header);

  memcpy(request + offset, question->qname, question->length);
  offset += question->length;

  memcpy(request + offset, &question->qtype, sizeof(question->qtype));
  offset += sizeof(question->qtype);

  memcpy(request + offset, &question->qclass, sizeof(question->qclass));

  return length;
}

static void dns_parse_name(unsigned char *chunk, unsigned char *ptr, char *out, int *len) {
  int flag = 0, n = 0, alen = 0;
  char *pos = out + (*len);

  while (1) {
    flag = ptr[0];
    if (flag == 0)break;

    if (is_pointer(flag)) { // duplicated hostname, just skip offset
      n = ptr[1];
      ptr = chunk + n;
    } else {
      ptr++;
      memcpy(pos, ptr, flag);
      pos += flag;
      ptr += flag;

      *len += flag;
      if (ptr[0] != 0) {
        memcpy(pos, ".", 1);
        pos += 1;
        *len += 1;
      }
    }
  }
}

static int dns_parse_response(unsigned char *buffer, struct dns_item **domains) {

  int i = 0;
  unsigned char *ptr = (unsigned char *) buffer;

  ptr += 4;
  int querys = ntohs(*(unsigned short *) ptr);

  ptr += 2;
  int answers = ntohs(*(unsigned short *) ptr);

  // skip Queries and set ptr to Answers
  ptr += 6;
  for (i = 0; i < querys; i++) {
    while (1) {
      int flag = (int) ptr[0];
      ptr += (flag + 1);

      if (flag == 0)break;
    }
    ptr += 4;
  }

  char cname[128], aname[128], ip[20], netip[4];
  int len, type, qclass, ttl, datalen;

  int cnt = 0;
  struct dns_item *list = (struct dns_item *) calloc(answers, sizeof(struct dns_item));
  if (list == NULL) {
    return -1;
  }

  for (i = 0; i < answers; i++) {
    bzero(aname, sizeof(aname));
    len = 0;

    dns_parse_name(buffer, ptr, aname, &len);
    ptr += 2;

    type = htons(*(unsigned short *) ptr);
    ptr += 2;

    qclass = htons(*(unsigned short *) ptr);
    ptr += 2;

    ttl = htons(*(unsigned short *) ptr);
    ptr += 4;

    datalen = ntohs(*(unsigned short *) ptr);
    ptr += 2;

    if (type == DNS_CNAME) {
      bzero(cname, sizeof(cname));
      len = 0;
      dns_parse_name(buffer, ptr, cname, &len);
      ptr += datalen;
    } else if (type == DNS_HOST) { // query ip address
      bzero(ip, sizeof(ip));
      if (datalen == 4) { // ipv4 address
        memcpy(netip, ptr, datalen);
        inet_ntop(AF_INET, netip, ip, sizeof(struct sockaddr));

        printf("%s has address %s\n", aname, ip);
        printf("\tTime to live: %d minutes , %d seconds\n", ttl / 60, ttl % 60);

        list[cnt].domain = (char *) calloc(strlen(aname) + 1, 1);
        memcpy(list[cnt].domain, aname, strlen(aname));

        list[cnt].ip = (char *) calloc(strlen(ip) + 1, 1);
        memcpy(list[cnt].ip, ip, strlen(ip));

        cnt++;
      }
      ptr += datalen;
    }
  }

  *domains = list;
  ptr += 2;
  return cnt;
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
  int slen = sendto(sockfd, request, req_len, 0, (struct sockaddr *) &dest, sizeof(struct sockaddr));

  char buffer[1024] = {0};
  struct sockaddr_in addr;
  size_t addr_len = sizeof(struct sockaddr_in);

  int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &addr, (socklen_t *) &addr_len);

  printf("recvfrom n: %d\n", n);;
  struct dns_item *domains = NULL;
  dns_parse_response((unsigned char *)buffer, &domains);

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

