//
// Created by fsindustry on 2023/4/10.
//


#include <sys/socket.h>

int main() {

  // 1）创建socket句柄
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd == -1) return -1;
}