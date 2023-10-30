//
// Created by fsindustry on 23-10-30.
//

#include <netinet/in.h>

extern "C" {
#include "unp.h"
}

void dg_cli(FILE *, int, const SA *, socklen_t);

void dg_cli_with_connect(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen);

int main(int argc, char **argv) {

  int sockfd;
  struct sockaddr_in servaddr;
  if (argc != 2) {
    err_quit("usage: <IP address>");
  }
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

  sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
  dg_cli(stdin, sockfd, (SA *) &servaddr, sizeof(servaddr));
  exit(0);
}

void dg_cli(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen) {
  int n;
  char sendline[MAXLINE], recvline[MAXLINE + 1];

  while (Fgets(sendline, MAXLINE, fp) != NULL) {

    Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);

    n = Recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);

    recvline[n] = 0;  /* null terminate */
    Fputs(recvline, stdout);
  }
}

void dg_cli_with_connect(FILE *fp, int sockfd, const SA *pservaddr, socklen_t servlen) {
  int n;
  char sendline[MAXLINE], recvline[MAXLINE + 1];

  Connect(sockfd, (SA *) pservaddr, servlen);

  while (Fgets(sendline, MAXLINE, fp) != NULL) {

    Write(sockfd, sendline, strlen(sendline));

    n = Read(sockfd, recvline, MAXLINE);

    recvline[n] = 0;  /* null terminate */
    Fputs(recvline, stdout);
  }
}
