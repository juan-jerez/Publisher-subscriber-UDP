// subscriber_udp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROKER_IP "192.168.100.10"  // Cambia por la IP del broker
#define BROKER_PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in sub_addr, broker_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(sub_addr);
    char topic[64];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    sub_addr.sin_family = AF_INET;
    sub_addr.sin_addr.s_addr = INADDR_ANY;
    sub_addr.sin_port = htons(0);  // puerto aleatorio
    bind(sockfd, (struct sockaddr *)&sub_addr, sizeof(sub_addr));

    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(BROKER_PORT);
    inet_pton(AF_INET, BROKER_IP, &broker_addr.sin_addr);

    printf("👥 Ingresa el topic a suscribirte (ej: PARTIDO1): ");
    fgets(topic, sizeof(topic), stdin);
    topic[strcspn(topic, "\n")] = '\0';

    char subscribe_msg[BUFFER_SIZE];
    snprintf(subscribe_msg, sizeof(subscribe_msg), "SUBSCRIBE %s", topic);
    sendto(sockfd, subscribe_msg, strlen(subscribe_msg), 0,
           (struct sockaddr *)&broker_addr, sizeof(broker_addr));

    printf("Suscrito al topic: %s\n", topic);

    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                             (struct sockaddr *)&broker_addr, &addr_len);
        if (n > 0) {
            buffer[n] = '\0';
            printf("[%s] %s\n", topic, buffer);
        }
    }

    close(sockfd);
    return 0;
}
