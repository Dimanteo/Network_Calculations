#include "client.h"

int main(int argc, char **argv)
{
    long int nworkers = enter_N(argc, argv);
    if (nworkers <= 0) {
        fprintf(stderr, "No threads requested\n");
        return EXIT_FAILURE;
    }
    int server_fd = connect_server(nworkers);
    if (server_fd < 0) {
        return EXIT_FAILURE;
    }
    struct Task task;
    if (receive_task(server_fd, &task) < 0) {
        return EXIT_FAILURE;
    }
    numb_t res = run_workers(&task, nworkers);
    if (isnan(res)) {
        return EXIT_FAILURE;
    }
    printf("Result: %f\n", res);
    if (send_result(server_fd, res) < 0) {
        return EXIT_FAILURE;
    }
    close(server_fd);
    return EXIT_SUCCESS;
}

int wait_broadcast(struct sockaddr_in *addr)
{
    int sk = socket(PF_INET, SOCK_DGRAM, 0);
    if (sk < 0) {
        perror("socket");
        return -1;
    }
    addr->sin_family        = AF_INET;
    addr->sin_port          = htons(UDP_PORT);
    addr->sin_addr.s_addr   = htonl(INADDR_ANY);
    socklen_t addr_len = sizeof(*addr);
    struct sockaddr *addr_ptr = (struct sockaddr*)addr;
    if (bind(sk, addr_ptr, addr_len) < 0) {
        perror("bind");
        return -1;
    }
    if (recvfrom(sk, NULL, 0, 0, addr_ptr, &addr_len) < 0) {
        perror("recvfrom");
        return -1;
    }
    close(sk);
    return 0;
}

int connect_server(long int nworkers)
{
    struct sockaddr_in addr;
    if (wait_broadcast(&addr) < 0) {
        return -1;
    }
    int sk = socket(PF_INET, SOCK_STREAM, 0);
    if (sk < 0) {
        perror("socket");
        return -1;
    }
    if (set_sock_options(sk) < 0) {
        close(sk);
        return -1;
    }
    addr.sin_port = htons(TCP_PORT);
    if (connect(sk, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sk);
        return -1;
    }
    if (write(sk, &nworkers, sizeof(nworkers)) < 0) {
        perror("write");
        close(sk);
        return -1;
    }
    return sk;
}

int receive_task(int server_fd, struct Task *task)
{
    fd_set readfd;
    FD_ZERO(&readfd);
    FD_SET(server_fd, &readfd);
    int event = select(server_fd + 1, &readfd, NULL, NULL, NULL);
    if (event < 0) {
        perror("select");
        return -1;
    } else if (event == 0) {
        fprintf(stderr, "Server waiting timeout exceeded\n");
        return -1;
    }
    if (read(server_fd, task, sizeof(*task)) < 0) {
        perror("read");
        return -1;
    }
    printf("Task received:\n\ta = %f\n\tb = %f\n\tsteps = %ld\n",
        task->from, task->to, task->nsteps);
    return 0;
}

int send_result(int server_fd, numb_t res)
{
    if (write(server_fd, &res, sizeof(res)) < 0) {
        perror("write");
        return -1;
    }
    return 0;
}