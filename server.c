#include "server.h"

static void close_clientsfd(long nclients, struct Client *clients);

int main(int argc, char **argv)
{
    int return_code = EXIT_FAILURE;
    long int nclients = enter_N(argc, argv);
    struct Client *clients = (struct Client*)malloc(nclients * sizeof(clients[0]));
    if (clients == NULL) {
        fprintf(stderr, "Error: malloc\n");
        goto return_error;
    }
    int server_fd = open_TCPsocket();
    if (server_fd < 0) {
        goto clients_cleanup;
    }
    long nworkers = wait_clients(server_fd, nclients, clients);
    if (nworkers < 0) {
        goto server_fd_cleanup;
    }
    if (send_tasks(clients, nclients, nworkers) < 0) {
        goto cleanup;
    }
    if (receive_results(clients, nclients) < 0) {
        goto cleanup;
    }
    return_code = EXIT_SUCCESS;

    cleanup:
    close_clientsfd(nclients, clients);
    server_fd_cleanup:
    close(server_fd);
    clients_cleanup:
    free(clients);
    return_error:
    return return_code;
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

long wait_clients(int sk, long nclients, struct Client *clients) {
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
                close_clientsfd(i, clients);
                return -1;
            }
            evt = select(sk + 1, &fdset, NULL, NULL, &timeout);
            if (evt < 0) {
                perror("select");
                close_clientsfd(i, clients);
                return -1;
            }
            if (try == 5) {
                fprintf(stderr, "Clients not found.\n");
                close_clientsfd(i, clients);
                return -1;
            }
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
        }
        int fd = accept(sk, NULL, NULL);
        if (fd < 0) {
            perror("accept");
            close_clientsfd(i, clients);
            return -1;
        }
        clients[i].fd = fd;
        if (read(fd, &clients[i].workers, sizeof(clients[i].workers)) < 0) {
            perror("read");
            close_clientsfd(i + 1, clients);
            return -1;
        }
        total_workers += clients[i].workers;
        printf("Client[%d]: %ld\n", i, clients[i].workers);
    }
    return total_workers;
}

static void close_clientsfd(long nclients, struct Client *clients)
{
    for (int i = 0; i < nclients; i++) {
        close(clients[i].fd);
    }
}

int open_TCPsocket()
{
    int sk = socket(PF_INET, SOCK_STREAM, 0);
    if (sk < 0) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(TCP_PORT),
        .sin_addr = {htonl(INADDR_ANY)}
    };
    socklen_t socklen = sizeof(addr);
    struct sockaddr *addr_ptr = (struct sockaddr*)&addr;
    if (bind(sk, addr_ptr, socklen) < 0) {
        perror("bind");
        close(sk);
        return -1;
    }
    if (listen(sk, SOMAXCONN) < 0) {
        perror("listen");
        close(sk);
        return -1;
    }
    return sk;
}

int send_tasks(struct Client *clients, long nclients, long nworkers)
{
    struct Task task;
    if (parse_task_file(&task) < 0) {
        fprintf(stderr, "Failed to parse config file\n");
        return -1;
    }
    double step = (task.to - task.from) / nworkers;
    size_t wrk_steps = task.nsteps / nworkers;
    double left = task.from;
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
            .tv_sec = 60,
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
                FD_CLR(clients[i].fd, &readfds);
                double resbuf = 0;
                if (read(clients[i].fd, &resbuf, sizeof(resbuf)) < 0) {
                    perror("read");
                    return -1;
                }
                result += resbuf;
                received++;
            } else {
                FD_SET(clients[i].fd, &readfds);
            }
        }
    }
    printf("Result = %f\n", result);
    return 0;
}

int parse_task_file(struct Task *task)
{
    FILE* f = fopen("config_integral", "r");
    if (f == NULL) {
        perror("fopen");
        return -1;
    }
    fscanf(f, "a:%lf\nb:%lf\nd:%ld", &task->from, &task->to, &task->nsteps);
    printf("Task: cos(3x) from %lf to %lf steps %ld\n", task->from, task->to, task->nsteps);
    return 0;
}