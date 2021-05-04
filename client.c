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
    printf("Received succefully.\n\tFrom: %#X/%d\n\tlength: %d\n\t%s\n", 
        addr.sin_addr, addr.sin_port, addr_len, buf);
    close(sk);
    return EXIT_SUCCESS;
}