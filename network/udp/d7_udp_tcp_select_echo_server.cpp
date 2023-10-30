//
// Created by fsindustry on 23-10-30.
//

extern "C" {
#include "unp.h"
}

void sig_chld(int signo);

int main(int argc, char **argv) {
  int listenfd, connfd, udpfd, nready, maxfdp1;
  char mesg[MAXLINE];
  pid_t childpid;
  fd_set rset;
  ssize_t n;
  socklen_t len;
  const int on = 1;
  struct sockaddr_in cliaddr, servaddr;

  /* create listening TCP socket */
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

  Listen(listenfd, LISTENQ);

  /* create UDP socket */
  udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);

  Bind(udpfd, (SA *) &servaddr, sizeof(servaddr));

  Signal(SIGCHLD, sig_chld);  /* must call waitpid() */

  FD_ZERO(&rset);
  maxfdp1 = max(listenfd, udpfd) + 1;
  for (;;) {
    FD_SET(listenfd, &rset);
    FD_SET(udpfd, &rset);
    if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 0) {
      if (errno == EINTR)
        continue;    /* back to for() */
      else
        err_sys("select error");
    }

    // handle TCP Connection
    if (FD_ISSET(listenfd, &rset)) {
      len = sizeof(cliaddr);
      connfd = Accept(listenfd, (SA *) &cliaddr, &len);

      if ((childpid = Fork()) == 0) {  /* child process */
        Close(listenfd);  /* close listening socket */
        str_echo(connfd);  /* process the request */
        exit(0);
      }
      Close(connfd);      /* parent closes connected socket */
    }

    // handle UDP Request
    if (FD_ISSET(udpfd, &rset)) {
      len = sizeof(cliaddr);
      n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA *) &cliaddr, &len);
      Sendto(udpfd, mesg, n, 0, (SA *) &cliaddr, len);
    }
  }
}

// SIGCHLD 信号处理函数
void sig_chld(int signo) {
  pid_t pid;
  int stat;

  while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    printf("child %d terminated\n", pid);
  return;
}