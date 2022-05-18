#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT    8080
#define MAXLINE 1024

int main()
{
    int sockfd;
    char buffer[MAXLINE];
    char* hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;
    struct sockaddr_in *wanda_addr, *jason_addr;
    char msg[32];

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    wanda_addr = NULL;
    jason_addr = NULL;

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr*)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n;

    while (1)
    {
        len = sizeof(cliaddr); // len is value/result
        n = recvfrom(sockfd, (char*)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr*) &cliaddr,
                     &len);
        if (n < 2)
        {
            printf("Malformed message.\n");
            continue;
        }

        buffer[n] = '\0';
        if (buffer[0] == 'w')
        {
            // From Wanda
            if (wanda_addr == NULL)
            {
                wanda_addr = malloc(len);
                memcpy(wanda_addr, &cliaddr, len);
            }

            printf("From Wanda (port %d): %s\n", wanda_addr->sin_port, buffer + 1);

            if (jason_addr != NULL)
            {
                sendto(sockfd, (const char *)(buffer + 1), n - 1,
                    MSG_CONFIRM, (const struct sockaddr *) jason_addr,
                    len);
                printf("To Jason (port %d): %s\n", jason_addr->sin_port, buffer + 1);
            }
        }
        else
        {
            // From Jason
            if (jason_addr == NULL)
            {
                jason_addr = malloc(len);
                memcpy(jason_addr, &cliaddr, len);
            }

            printf("Jason (port %d): %s\n", jason_addr->sin_port, buffer + 1);

            if (wanda_addr != NULL)
            {
                sendto(sockfd, (const char *)(buffer + 1), n - 1,
                    MSG_CONFIRM, (const struct sockaddr *) wanda_addr,
                    len);
                printf("To Wanda (port %d): %s\n", wanda_addr->sin_port, buffer + 1);
            }
        }
    }

    // sendto(sockfd, (const char*)hello, strlen(hello),
    //        MSG_CONFIRM, (const struct sockaddr*) &cliaddr,
    //        len);
    // printf("Hello message sent.\n");

    return 0;
}
