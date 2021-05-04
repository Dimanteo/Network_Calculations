#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    int sk = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(8000),
        .sin_addr   = htonl(INADDR_ANY)
    };
    char buf[100];
    socklen_t addr_len = sizeof(addr);
    struct sockaddr* addr_ptr = (struct sockaddr*)&addr;
    if (bind(sk, addr_ptr, addr_len) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }
    if (recvfrom(sk, buf, sizeof(buf), 0, addr_ptr, &addr_len) < 0) {
        perror("recvfrom");
        return EXIT_FAILURE;
    }
    printf("Received succefully.\n\tFrom: %s\n\tlength: %d\n", addr_ptr->sa_data, addr_len);
    return EXIT_SUCCESS;
}