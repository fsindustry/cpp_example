//
// Created by fsindustry on 2023/4/10.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <cstdio>
#include <cstring>

#define BUFFER_LENGTH  8192
#define EVENTS_LENGTH 128

char rbuffer[BUFFER_LENGTH] = {0};
char wbuffer[BUFFER_LENGTH] = {0};


int main() {

  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == listenfd) return -1;

  sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(9999);

  if (-1 == bind(listenfd, (sockaddr *) &server_addr, sizeof(server_addr))) {
    return -2;
  }

  listen(listenfd, 10);

  // 创建epoll句柄
  int epfd = epoll_create(1);
  // 暂存当前处理事件，和接收到的事件
  epoll_event ev, events[EVENTS_LENGTH];

  // 注册服务端监听读事件
  ev.events = EPOLLIN;
  ev.data.fd = listenfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

  while (1) {
    // 等待epoll事件
    int nready = epoll_wait(epfd, events, EVENTS_LENGTH, -1);
    printf("------ %d\n", nready);

    for (int i = 0; i < nready; i++) {
      int eventfd = events[i].data.fd;
      if (listenfd == eventfd) { // 处理服务端事件
        sockaddr_in client;
        socklen_t len = sizeof(client);

        // 处理连接
        int clientfd = accept(listenfd, (sockaddr *) &client, &len);
        if (-1 == clientfd)break;

        printf("accept: %d\n", clientfd);
        // 注册客户端监听读事件，设置边缘触发
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = clientfd;
        epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);

      } else if (events[i].events & EPOLLIN) { // 处理客户端读事件

        int n = recv(eventfd, rbuffer, BUFFER_LENGTH, 0);
        if (n > 0) {
          printf("recv: %s, n: %d\n", rbuffer, n);

          memcpy(wbuffer, rbuffer, BUFFER_LENGTH);

          // 修改客户端关注写事件
          ev.events = EPOLLOUT;
          ev.data.fd = eventfd;
          epoll_ctl(epfd, EPOLL_CTL_MOD, eventfd, &ev);
        }
      } else if (events[i].events & EPOLLOUT) { // 处理客户端写事件

        int sent = send(eventfd, wbuffer, BUFFER_LENGTH, 0);
        printf("sent: %d\n", sent);

        // 修改客户端关注读事件
        ev.events = EPOLLIN;
        ev.data.fd = eventfd;
        epoll_ctl(epfd, EPOLL_CTL_MOD, eventfd, &ev);
      }
    }
  }
}