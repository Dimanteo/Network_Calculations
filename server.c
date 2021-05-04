#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    int sk = socket(PF_INET, SOCK_DGRAM, 0);
    if (sk < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(8000),
        .sin_addr   = htonl(INADDR_BROADCAST)
    };
    int optval = 1;
    char msg[] = "Hello UDP";
    struct sockaddr *addr_ptr = (struct sockaddr*)&addr;
    if (setsockopt(sk, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < -1) {
        perror("setsockopt");
        return EXIT_FAILURE;
    }
    if (sendto(sk, msg, sizeof(msg), 0, addr_ptr, sizeof(addr)) < 0) {
        perror("sendto");
        return EXIT_FAILURE;
    }
    close(sk);
    return EXIT_SUCCESS;
}