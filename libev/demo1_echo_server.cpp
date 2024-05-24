//
// Created by fsindustry on 5/24/24.
//

#include <iostream>
#include <netinet/in.h>
#include <ev.h>
#include <cstring>
#include <unistd.h>
#include <asm-generic/ioctls.h>
#include <sys/ioctl.h>

#define SERVER_PORT 3033
#define SERVER_ADDR INADDR_ANY
#define BUFFER_SIZE 1024

int total_clients = 0;

struct io_buf {
    char *data;
    size_t len;
};

int set_nonblocking(int fd) {
    int flags = 1;
    if (ioctl(fd, FIONBIO, &flags) == -1) {
        perror("ioctl FIONBIO");
        return -1;
    }
    return 0;
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

void io_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

int main() {
    std::cout << "Hello World!" << std::endl;
    int listenfd;
    struct sockaddr_in addr;
    struct ev_loop *loop = ev_default_loop();
    struct ev_io w_accept;

    // 1. create socket fd
    if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error.");
        return -1;
    }

    if (set_nonblocking(listenfd) < 0) {
        perror("set_nonblocking error.");
        return -1;
    }

    int reuse = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        return 1;
    }

    // 2. bind address
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = SERVER_ADDR;
    if (bind(listenfd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        perror("bind error");
        return -1;
    }

    // 3. start listening
    if (listen(listenfd, 2) < 0) {
        perror("listen error");
        return -1;
    }

    // 4. register accpet watcher
    ev_io_init(&w_accept, accept_cb, listenfd, EV_READ);
    ev_io_start(loop, &w_accept);

    // 5. start eventloop
    ev_run(loop);

    // 6. destory eventloop
    ev_loop_destroy(loop);
}


void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int clientfd;
    struct ev_io *io_watcher = (struct ev_io *) malloc(sizeof(struct ev_io));

    if (EV_ERROR & revents) {
        perror("invalid event");
        return;
    }

    clientfd = accept(watcher->fd, (struct sockaddr *) &client_addr, &client_len);
    if (clientfd < 0) {
        perror("accept error");
        return;
    }

    if (set_nonblocking(clientfd) < 0) {
        perror("set_nonblocking error.");
        return;
    }

    total_clients++;
    printf("Successfully connected with client: %d\n", clientfd);
    printf("Total clients is %d.\n", total_clients);

    // register read and write watcher for current clientfd
    struct io_buf *buf = (struct io_buf *) malloc(sizeof(io_buf));
    buf->data = (char *) malloc(BUFFER_SIZE);
    bzero(buf->data, BUFFER_SIZE);
    buf->len = 0;
    io_watcher->data = buf;
    ev_io_init(io_watcher, io_cb, clientfd, EV_READ);
    ev_io_start(loop, io_watcher);
}

void io_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    ssize_t read_len;

    if (EV_ERROR & revents) {
        perror("invalid event");
        return;
    }

    int clientfd = watcher->fd;
    io_buf *buf = (io_buf *) watcher->data;

    if (EV_READ & revents) { // handle read event
        read_len = read(clientfd, buf->data, BUFFER_SIZE);
        if (read_len < 0) {
            perror("read error");
            return;
        }

        if (read_len == 0) {
            ev_io_stop(loop, watcher);
            free(buf->data);
            free(watcher->data);
            free(watcher);
            printf("Successfully close client: %d\n", clientfd);
            total_clients--;
            printf("Total clients is %d.\n", total_clients);
            return;
        }

        buf->len = read_len;
        printf("recevie message:%s from %d\n", buf->data, clientfd);

        // register write event
        ev_io_stop(loop, watcher);
        ev_io_set(watcher, clientfd, EV_WRITE);
        ev_io_start(loop, watcher);
    } else if (EV_WRITE & revents) { // handle write event
        if (buf->len > 0) {
            write(watcher->fd, buf->data, buf->len);
            bzero(buf->data, BUFFER_SIZE);
            buf->len = 0;
        }

        // register read event
        ev_io_stop(loop, watcher);
        ev_io_set(watcher, clientfd, EV_READ);
        ev_io_start(loop, watcher);
    }
}
