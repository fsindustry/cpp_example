//
// Created by fsindustry on 4/20/23.
//

#include <cstdlib>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unordered_set>
#include <cstring>
#include <unistd.h>
#include <cstdio>

#define BUFFER_LENGTH  128
#define EVENTS_LENGTH  128

#define PORT_COUNT    100
#define ITEM_LENGTH    1024

// 封装一个socket连接的上下文
struct sock_item {
  // clientfd
  int fd;

  // 连接读缓冲
  char *rbuf;
  int rlen;

  // 连接写缓冲
  char *wbuf;
  int wlen;

  int event;

  // 读事件处理回调
  void (*recv_cb)(int fd, char *buffer, int length);

  // 写事件处理回调
  void (*send_cb)(int fd, char *buffer, int length);

  // accept事件处理回调
  void (*accept_cb)(int fd, char *buffer, int length);
};

// 存放待处理连接上下文对象的动态数组
struct eventblock {
  sock_item *items;
  eventblock *next;
};

// reactor对象
struct reactor {
  int epoll_fd;
  int block_count;
  eventblock *eventblocks;
};

// 创建socket服务端句柄，并开启监听
int init_server(short port);

// 判断一个句柄是否是服务端监听句柄
bool is_listenfd(std::unordered_set<int> *sockfds, int connfd);

// 获取客户端socket对应的上下文对象
sock_item *reactor_lookup(reactor *r, int clientfd);

// 扩容reactor中的动态数组
int reactor_resize(reactor *r);

int main() {

  // 创建reactor对象
  auto *r = (reactor *) calloc(1, sizeof(reactor));
  if (nullptr == r) {
    return -3;
  }

  // 创建epoll句柄
  r->epoll_fd = epoll_create(1);
  epoll_event ev{}, events[EVENTS_LENGTH];

  // 创建多个socket服务端
  std::unordered_set<int> sockfds(PORT_COUNT);
  for (int i = 0; i < PORT_COUNT; i++) {
    int listenfd = init_server(9999 + i);
    sockfds.insert(listenfd);

    // 注册服务端socket读事件
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(r->epoll_fd, EPOLL_CTL_ADD, listenfd, &ev);
  }

  // 进入事件处理循环
  while (1) {

    // 等待事件就绪
    int nready = epoll_wait(r->epoll_fd, events, EVENTS_LENGTH, -1);

    // 遍历并处理事件
    for (int i = 0; i < nready; i++) {
      int eventfd = events[i].data.fd;

      // 处理accept事件
      if (is_listenfd(&sockfds, eventfd)) {

        // 接收客户端连接
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int clientfd = accept(eventfd, (sockaddr *) &client_addr, &len);
        if (-1 == clientfd) break;
        if (clientfd % 1000 == 999) {
          printf("accept: %d\n", clientfd);
        }

        // 设置客户端socket句柄为非阻塞状态
        int flag = fcntl(clientfd, F_GETFL, 0);
        flag |= O_NONBLOCK;
        fcntl(clientfd, F_SETFL, flag);

        // 注册客户端socket读事件
        ev.events = EPOLLIN;
        ev.data.fd = clientfd;
        epoll_ctl(r->epoll_fd, EPOLL_CTL_ADD, clientfd, &ev);

        // 为客户端socket创建上下文对象
        sock_item *item = reactor_lookup(r, clientfd);
        item->fd = clientfd;
        item->rbuf = static_cast<char *>(calloc(1, BUFFER_LENGTH));
        item->rlen = 0;
        item->wbuf = static_cast<char *>(calloc(1, BUFFER_LENGTH));
        item->wlen = 0;
      }
        // 处理客户端读事件
      else if (events[i].events & EPOLLIN) {

        sock_item *item = reactor_lookup(r, eventfd);
        char *rbuf = item->rbuf;
        char *wbuf = item->wbuf;

        int n = recv(eventfd, rbuf, BUFFER_LENGTH, 0);
        if (n > 0) {
          printf("recv: %s, n: %d\n", rbuf, n);
          memcpy(wbuf, rbuf, BUFFER_LENGTH);

          // 客户端fd注册写事件
          ev.events = EPOLLOUT;
          ev.data.fd = eventfd;
          epoll_ctl(r->epoll_fd, EPOLL_CTL_MOD, eventfd, &ev);
        } else if (0 == n) { // 客户端连接关闭
          // 释放缓存
          free(rbuf);
          free(wbuf);
          item->fd = 0;
          // 关闭服务端连接
          close(eventfd);
        }
      } else if (events[i].events & EPOLLOUT) { // 处理客户端写事件
        sock_item *item = reactor_lookup(r, eventfd);
        char *wbuf = item->wbuf;

        int sent = send(eventfd, wbuf, BUFFER_LENGTH, 0);
        printf("sent: %d\n", sent);

        // 注册客户端fd读事件
        ev.events = EPOLLIN;
        ev.data.fd = eventfd;
        epoll_ctl(r->epoll_fd, EPOLL_CTL_MOD, eventfd, &ev);
      }
    }
  }
}

int init_server(short port) {

  // 1.创建socket对象
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1) return -1;

  // 2.创建sockaddr对象
  sockaddr_in servaddr{};
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  // 3.执行bind操作
  if (-1 == bind(listenfd, (sockaddr *) &servaddr, sizeof(servaddr))) {
    return -2;
  }

  // 4.设置socket为非阻塞状态
  int flag = fcntl(listenfd, F_GETFL, 0);
  flag |= O_NONBLOCK;
  fcntl(listenfd, F_SETFL, flag);

  // 5.开始监听
  listen(listenfd, 1024);

  return listenfd;
}

bool is_listenfd(std::unordered_set<int> *sockfds, int connfd) {
  return sockfds->find(connfd) != sockfds->end();
}

sock_item *reactor_lookup(reactor *r, int clientfd) {

  if (nullptr == r) return nullptr;
  if (clientfd <= 0) return nullptr;

  printf("reactor_lookup --> %d\n", r->block_count);

  // 计算clientfd所在的eventblock
  int block_idx = clientfd / ITEM_LENGTH;

  // 若索引超过eventblock数量，则扩容
  while (block_idx >= r->block_count) {
    reactor_resize(r);
  }

  // 查找对应的eventblock
  int i = 0;
  eventblock *cur = r->eventblocks;
  while (i++ < block_idx && cur != nullptr) {
    cur = cur->next;
  }

  return &cur->items[clientfd % ITEM_LENGTH];
}

int reactor_resize(reactor *r) {
  if (nullptr == r) return -1;

  // 遍历到链表尾部
  eventblock *cur = r->eventblocks;
  while (cur != nullptr && cur->next != nullptr) {
    cur = cur->next;
  }

  // 创建一批sock_item对象
  auto *item = (sock_item *) malloc(ITEM_LENGTH * sizeof(sock_item));
  if (nullptr == item) return -4;
  memset(item, 0, ITEM_LENGTH * sizeof(sock_item));

  // 创建eventblock对象
  auto *block = static_cast<eventblock *>(malloc(sizeof(eventblock)));
  if (nullptr == block) {
    free(item);
    return -5;
  }
  memset(block, 0, sizeof(eventblock));

  block->items = item;
  block->next = nullptr;
  if (nullptr == cur) {
    r->eventblocks = block;
  } else {
    cur->next = block;
  }

  r->block_count++;

  return 0;
}