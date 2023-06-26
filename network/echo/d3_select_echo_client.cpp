//
// Created by fsindustry on 6/16/23.
//


#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>

extern "C" {
#include "unp.h"
}

#define BACKLOG_SIZE 10
#define BUFFER_LENGTH  1024

void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv) {

  // 0) 接收參數
  if (argc != 3) {
    err_quit("usage: program <IPaddress> <Port>");
  }
  // 服务端ip地址
  const char *ip = argv[1];
  // 服务端端口号
  int port = atoi(argv[2]);

  // 1）创建socket句柄
  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (sockfd == -1) return -1;

  // 2）封装socket地址
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  server_addr.sin_port = htons(port);

  // 3）连接服务器
  if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(sockaddr_in)) < 0) {
    err_quit("connect server %s:%d failed.", ip, port);
  }

  // 4）接收控制台输入，发送给服务端
  str_cli(stdin, sockfd);

  return 0;
}

void str_cli(FILE *fp, int sockfd) {

  int maxfd, n, stdineof = 0;
  fd_set rset;
  FD_ZERO(&rset);

  char write_buf[BUFFER_LENGTH], read_buf[BUFFER_LENGTH];

  for (;;) {

    // 仅当stdin未关闭，才注册stdin读事件监听
    if (stdineof == 0) {
      FD_SET(fileno(fp), &rset);
    }
    // 注册sockfd的读事件监听
    FD_SET(sockfd, &rset);

    maxfd = max(fileno(fp), sockfd) + 1;
    select(maxfd, &rset, NULL, NULL, NULL);

    if (FD_ISSET(sockfd, &rset)) { // sockfd可读
      // 读取服务端响应
      if ((n = read(sockfd, read_buf, BUFFER_LENGTH)) <= 0) {
        if (stdineof == 1) {
          return;
        } else {
          err_quit("str_cli: server terminated prematurely");
        }
      }

      // 输出响应到客户端
      write(fileno(stdout), read_buf, n);
    }

    if (FD_ISSET(fileno(fp), &rset)) { // stdin可读
      // 获取控制台输入，若未读取到数据，则关闭socket写端
      if((n = read(fileno(fp), write_buf, BUFFER_LENGTH)) <= 0){
        stdineof = 1;
        shutdown(sockfd, SHUT_WR);
        FD_CLR(fileno(fp), &rset);
        continue;
      }

      // 向服务端发送输入
      Writen(sockfd, write_buf, n);
    }
  }
}