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
        .sin_addr   = {htonl(INADDR_BROADCAST)}
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
    sk = socket(PF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8001);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    char buf[100];
    socklen_t socklen = sizeof(addr);
    if (bind(sk, addr_ptr, socklen) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }
    if (listen(sk, 256) < 0) {
        perror("listen");
        return EXIT_FAILURE;
    }
    int insk = accept(sk, addr_ptr, &socklen);
    if (insk < 0) {
        perror("accept");
        return EXIT_FAILURE;
    }
    if (read(insk, buf, sizeof(buf)) < 0) {
        perror("read");
        return EXIT_FAILURE;
    }
    printf("Received message: %s\n", buf);
    close(sk);
    return EXIT_SUCCESS;
}