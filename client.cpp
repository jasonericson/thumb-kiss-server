// Client side implementation of UDP client-server model
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#define PORT 1234
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr;
float other_client_x;
float other_client_y;

void* recv_from_other_client(void* ptr)
{
    char buffer[MAXLINE];
    socklen_t len;
    int n;

    len = sizeof(servaddr);
    while (1)
    {
        n = recvfrom(sockfd, (char*)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *) &servaddr,
                     &len);
        buffer[n] = '\0';

        rapidjson::Document doc;
        doc.Parse(buffer);

        other_client_x = doc["pos"][0].GetFloat();
        other_client_y = doc["pos"][1].GetFloat();
    }
}

// Driver code
int main(int argc, char* argv[])
{
    char msg[32];
    pthread_t recv_thread;

    if (argc != 2)
    {
        perror("Wrong number of arguments.");
        exit(EXIT_FAILURE);
    }

    char* id = argv[1];

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sin;
    socklen_t sinlen;
    if (getsockname(sockfd, (struct sockaddr*)&sin, &sinlen) < 0)
    {
        perror("could not get socket info");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_aton("127.0.0.1", (struct in_addr*) &servaddr.sin_addr.s_addr);

    pthread_create(&recv_thread, NULL, recv_from_other_client, NULL);

    socklen_t len;
    int n;
    float x = 0.0f;
    float y = 0.0f;

    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Value id_val;
    id_val.SetString(rapidjson::StringRef(id));
    doc.AddMember("id", id_val, doc.GetAllocator());

    rapidjson::Value listenPort_val;
    listenPort_val.SetInt(sin.sin_port);
    doc.AddMember("listenPort", listenPort_val, doc.GetAllocator());

    rapidjson::Value pos_val;
    pos_val.SetArray();
    pos_val.PushBack(x, doc.GetAllocator());
    pos_val.PushBack(y, doc.GetAllocator());
    doc.AddMember("pos", pos_val, doc.GetAllocator());

    while (1)
    {
        x = float(rand()) / float(RAND_MAX);
        y = float(rand()) / float(RAND_MAX);
        doc["pos"][0] = x;
        doc["pos"][1] = y;

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        const char* json_string = buffer.GetString();
        printf("%s\n", json_string);

        len = sizeof(servaddr);
        
        sendto(sockfd, json_string, strlen(json_string),
               0, (const struct sockaddr *) &servaddr,
               len);
        printf("Our position: (%f, %f).\n", x, y);
        printf("Other client position: (%f, %f)\n", other_client_x, other_client_y);

        sleep(1);
    }

    close(sockfd);
    return 0;
}
