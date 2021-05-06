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
    long nworkers = wait_clients(server_fd, nclients, clients);
    if (nworkers < 0) {
        return EXIT_FAILURE;
    }
    if (send_tasks(clients, nclients, nworkers) < 0) {
        return EXIT_FAILURE;
    }
    receive_results(clients, nclients);

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

long wait_clients(int sk, int nclients, struct Client *clients) {
    fd_set fdset;
    long int total_workers = 0;
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
        total_workers += clients[i].workers;
        printf("Client[%d]: %ld\n", i, clients[i].workers);
    }
    return total_workers;
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

int send_tasks(struct Client *clients, long nclients, long nworkers)
{
    double step = (INT_TO - INT_FROM) / nworkers;
    size_t wrk_steps = INT_STEPS / nworkers;
    double left = INT_FROM;
    for (long i = 0; i < nclients; i++) {
        struct Task task = {
            .from   = left,
            .to     = left + step * clients[i].workers,
            .nsteps = wrk_steps * clients[i].workers
        };
        if (write(clients[i].fd, &task, sizeof(task)) < 0) {
            perror("write");
            return -1;
        }
        left += step * clients[i].workers;
    }
    
    return 0;
}

int receive_results(struct Client *clients, long nclients)
{
    fd_set readfds;
    int maxfd = -1;
    long received = 0;
    double result = 0;
    FD_ZERO(&readfds);
    for (int i = 0; i < nclients; i++) {
        FD_SET(clients[i].fd, &readfds);
        if (maxfd < clients[i].fd)
            maxfd = clients[i].fd;
    }
    while (received != nclients) {
        struct timeval timeout = {
            .tv_sec = 30,
            .tv_usec = 0
        };
        int events = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
        if (events < 0) {
            perror("select");
            return -1;
        } else if (events == 0) {
            fprintf(stderr, "Clients response timeout exceeded.\n");
            return -1;
        }
        for (int i = 0; i < nclients; i++) {
            if (FD_ISSET(clients[i].fd, &readfds)) {
                double resbuf = 0;
                if (read(clients[i].fd, &resbuf, sizeof(resbuf)) < 0) {
                    perror("read");
                    return -1;
                }
                result += resbuf;
                received++;
            }
        }
    }
    printf("Result = %f\n", result);
    return 0;
}