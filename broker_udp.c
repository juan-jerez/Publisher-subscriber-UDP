// broker_udp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROKER_PORT 5000
#define MAX_SUBS 10
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in broker_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    struct sockaddr_in subscribers[MAX_SUBS];
    int sub_count = 0;

    // Crear socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    broker_addr.sin_family = AF_INET;
    broker_addr.sin_addr.s_addr = INADDR_ANY;
    broker_addr.sin_port = htons(BROKER_PORT);

    // Enlazar el socket al puerto del broker
    if (bind(sockfd, (const struct sockaddr *)&broker_addr, sizeof(broker_addr)) < 0) {
        perror("Error en bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("🧠 Broker UDP escuchando en puerto %d...\n", BROKER_PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        // Verificar si es un mensaje de suscripción
        if (strncmp(buffer, "SUBSCRIBE", 9) == 0) {
            if (sub_count < MAX_SUBS) {
                subscribers[sub_count++] = client_addr;
                printf("✅ Nuevo suscriptor agregado (%d total)\n", sub_count);
            }
        } else {
            printf("📨 Mensaje recibido del publicador: %s\n", buffer);
            // Reenviar mensaje a todos los suscriptores
            for (int i = 0; i < sub_count; i++) {
                sendto(sockfd, buffer, strlen(buffer), 0,
                       (struct sockaddr *)&subscribers[i], sizeof(subscribers[i]));
            }
        }
    }

    close(sockfd);
    return 0;
}
