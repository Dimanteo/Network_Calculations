#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

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
        .sin_addr   = {htonl(INADDR_ANY)}
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
    printf("Received succefully.\n\tFrom: %#X/%d\n\tlength: %d\n\t%s\n", 
        addr.sin_addr.s_addr, addr.sin_port, addr_len, buf);
    close(sk);

    sk = socket(PF_INET, SOCK_STREAM, 0);
    if (sk < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }
    addr.sin_port = htons(8001);
    if (connect(sk, addr_ptr, sizeof(addr)) < 0) {
        perror("connect");
        return EXIT_FAILURE;
    }
    char response[] = "Client TCP response";
    if (write(sk, response, sizeof(response)) < 0) {
        perror("write");
        return EXIT_FAILURE;
    }
    close(sk);
    return EXIT_SUCCESS;
}