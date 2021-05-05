#include "server.h"

int main(int argc, char **argv)
{
    long int nclients = enter_N(argc, argv);
    int *clients_fdset = (int*)malloc(nclients);
    if (clients_fdset == NULL) {
        fprintf(stderr, "Error: malloc\n");
        return EXIT_FAILURE;
    }
    send_broadcast();
    int server_fd = wait_clients(nclients, clients_fdset);
    send_tasks();
    receive_results();

    close(server_fd);
    for (int i = 0; i < nclients; i++) {
        close(clients_fdset[i]);
    }
    free(clients_fdset);
    return EXIT_SUCCESS;
}

int send_broadcast() 
{
    int sk = socket(PF_INET, SOCK_DGRAM, 0);
    if (sk < 0) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(UDP_PORT),
        .sin_addr   = {htonl(INADDR_BROADCAST)}
    };
    int optval = 1;
    struct sockaddr *addr_ptr = (struct sockaddr*)&addr;
    if (setsockopt(sk, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < -1) {
        perror("setsockopt");
        return -1;
    }
    if (sendto(sk, NULL, 0, 0, addr_ptr, sizeof(addr)) < 0) {
        perror("sendto");
        return -1;
    }
    close(sk);
    return 0;
}

int wait_clients(int nclients, int *cl_fdset) {
    int sk = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(TCP_PORT),
        .sin_addr = {htonl(INADDR_ANY)}
    };
    char buf[100];
    socklen_t socklen = sizeof(addr);
    struct sockaddr *addr_ptr = (struct sockaddr*)&addr;
    if (bind(sk, addr_ptr, socklen) < 0) {
        perror("bind");
        return -1;
    }
    if (listen(sk, SOMAXCONN) < 0) {
        perror("listen");
        return -1;
    }
    for (int i = 0; i < nclients; i++) {
        int fd = accept(sk, addr_ptr, &socklen);
        if (fd < 0) {
            perror("accept");
            return -1;
        }
        cl_fdset[i] = fd;
        if (read(fd, buf, sizeof(buf)) < 0) {
            perror("read");
            return -1;
        }
        printf("Received message: %s\n", buf);
    }
    return sk;
}

int send_tasks()
{
    return 0;
}

int receive_results()
{
    return 0;
}