#include "server.h"

int main(int argc, char **argv)
{
    long int nclients = enter_N(argc, argv);
    struct Client *clients = (struct Client*)malloc(nclients * sizeof(clients[0]));
    if (clients == NULL) {
        fprintf(stderr, "Error: malloc\n");
        return EXIT_FAILURE;
    }
    int server_fd = open_TCPsocket();
    wait_clients(server_fd, nclients, clients);
    send_tasks();
    receive_results();

    close(server_fd);
    for (int i = 0; i < nclients; i++) {
        close(clients[i].fd);
    }
    free(clients);
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

int wait_clients(int sk, int nclients, struct Client *clients) {
    fd_set fdset;
    struct timeval timeout = {
        .tv_sec = 10,
        .tv_usec = 0
    };
    for (int i = 0; i < nclients; i++) {
        for (int try = 0, evt = 0; evt == 0; try++) {
            FD_ZERO(&fdset);
            FD_SET(sk, &fdset);
            if (send_broadcast() < 0) {
                return -1;
            }
            evt = select(sk + 1, &fdset, NULL, NULL, &timeout);
            if (evt < 0) {
                perror("select");
                return -1;
            }
            if (try == 5) {
                fprintf(stderr, "Clients not found.\n");
                return -1;
            }
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
        }
        int fd = accept(sk, NULL, NULL);
        if (fd < 0) {
            perror("accept");
            return -1;
        }
        clients[i].fd = fd;
        if (read(fd, &clients[i].workers, sizeof(clients[i].workers)) < 0) {
            perror("read");
            return -1;
        }
        printf("Client[%d]: %ld\n", i, clients[i].workers);
    }
    return sk;
}

int open_TCPsocket()
{
    int sk = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(TCP_PORT),
        .sin_addr = {htonl(INADDR_ANY)}
    };
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