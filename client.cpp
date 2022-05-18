// Client side implementation of UDP client-server model
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

#define PORT 8080
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

        // rapidjson::Document doc;
        // doc.Parse(buffer);

        // other_client_x = doc["pos"].GetObject()["x"];
        // other_client_y = doc["pos"].GetObject()["y"];
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

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // pthread_create(&recv_thread, NULL, recv_from_other_client, NULL);

    socklen_t len;
    int n;
    float x = 0.0f;
    float y = 0.0f;

    rapidjson::Document doc;
    doc.SetObject();

    rapidjson::Value id_val;
    id_val.SetString(rapidjson::StringRef(id));
    doc.AddMember("id", id_val, doc.GetAllocator());

    rapidjson::Value pos_val;
    pos_val.SetObject();
    pos_val.AddMember("x", x, doc.GetAllocator());
    pos_val.AddMember("y", y, doc.GetAllocator());
    doc.AddMember("pos", pos_val, doc.GetAllocator());

    while (1)
    {
        len = sizeof(servaddr);
        
        // sendto(sockfd, doc., strlen(msg),
        //        MSG_CONFIRM, (const struct sockaddr *) &servaddr,
        //        len);
        // printf("Message sent with %d.\n", thing);
        // printf("Other client position: (%f, %f)\n", other_client_x, other_client_y);
        
        doc["pos"]["x"] = x;
        doc["pos"]["y"] = y;

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        printf("%s\n", buffer.GetString());

        x += 1.0f;
        y += 2.0f;

        sleep(1);
    }

    close(sockfd);
    return 0;
}
