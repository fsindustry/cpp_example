//
// Created by fsindustry on 6/16/23.
//


#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <sys/epoll.h>

extern "C" {
#include "unp.h"
}

#define BACKLOG_SIZE 10
#define BUFFER_LENGTH  1024
#define MAX_POLL_SIZE 4096

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

  int n, stdineof = 0, nready;
  int epollfd = epoll_create(1);
  epoll_event ev, events[MAX_POLL_SIZE];
  char write_buf[BUFFER_LENGTH], read_buf[BUFFER_LENGTH];


  ev.events = EPOLLIN;
  ev.data.fd = fileno(fp);
  epoll_ctl(epollfd, EPOLL_CTL_ADD, fileno(fp), &ev);
  ev.events = EPOLLIN;
  ev.data.fd = sockfd;
  epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);

  for (;;) {

    // 6）获取监听事件
    nready = epoll_wait(epollfd, events, MAX_POLL_SIZE, -1);

    for (int i = 0; i < nready; i++) {
      int eventfd = events[i].data.fd;

      if (eventfd == sockfd && (events[i].events & EPOLLIN)) { // sockfd可读
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

      if (eventfd == fileno(fp) && (events[i].events & EPOLLIN)) { // stdin可读
        // 获取控制台输入，若未读取到数据，则关闭socket写端
        if ((n = read(fileno(fp), write_buf, BUFFER_LENGTH)) <= 0) {
          stdineof = 1;
          epoll_ctl(epollfd, EPOLL_CTL_DEL, fileno(fp), nullptr);
          shutdown(sockfd, SHUT_WR);
          continue;
        }

        // 向服务端发送输入
        Writen(sockfd, write_buf, n);
      }
    }
  }
}