#include "client.h"

int main(int argc, char **argv)
{
    long int nworkers = enter_N(argc, argv);
    int server_fd = connect_server(nworkers);
    if (server_fd < 0) {
        return EXIT_FAILURE;
    }
    receive_task();
    run_workers();
    send_result();
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
    addr.sin_port = htons(TCP_PORT);
    if (connect(sk, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return -1;
    }
    if (write(sk, &nworkers, sizeof(nworkers)) < 0) {
        perror("write");
        return -1;
    }
    return sk;
}

int receive_task()
{
    return 0;
}

int run_workers()
{
    return 0;
}

int send_result()
{
    return 0;
}