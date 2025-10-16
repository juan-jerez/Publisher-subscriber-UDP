// broker_udp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROKER_PORT 5000
#define BUFFER_SIZE 1024
#define MAX_TOPICS 10
#define MAX_SUBS 10

typedef struct {
    char topic[64];
    struct sockaddr_in subs[MAX_SUBS];
    int sub_count;
} Topic;

int find_topic(Topic topics[], int count, const char *topic_name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(topics[i].topic, topic_name) == 0)
            return i;
    }
    return -1;
}

int sub_exists(Topic *t, struct sockaddr_in *addr) {
    for (int i = 0; i < t->sub_count; i++) {
        if (t->subs[i].sin_addr.s_addr == addr->sin_addr.s_addr &&
            t->subs[i].sin_port == addr->sin_port)
            return 1;
    }
    return 0;
}

int main() {
    int sockfd;
    struct sockaddr_in broker_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    Topic topics[MAX_TOPICS];
    int topic_count = 0;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    broker_addr.sin_family = AF_INET;
    broker_addr.sin_addr.s_addr = INADDR_ANY;
    broker_addr.sin_port = htons(BROKER_PORT);

    if (bind(sockfd, (struct sockaddr *)&broker_addr, sizeof(broker_addr)) < 0) {
        perror("Error en bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf( "Broker UDP escuchando en puerto %d...\n", BROKER_PORT);

    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                             (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) continue;
        buffer[n] = '\0';

        if (strncmp(buffer, "SUBSCRIBE ", 10) == 0) {
            char *topic_name = buffer + 10;
            topic_name[strcspn(topic_name, "\r\n")] = '\0';

            int t_index = find_topic(topics, topic_count, topic_name);
            if (t_index == -1 && topic_count < MAX_TOPICS) {
                strcpy(topics[topic_count].topic, topic_name);
                topics[topic_count].sub_count = 0;
                t_index = topic_count++;
                printf("🆕 Creado topic: %s\n", topic_name);
            }

            if (t_index != -1) {
                Topic *t = &topics[t_index];
                if (!sub_exists(t, &client_addr) && t->sub_count < MAX_SUBS) {
                    t->subs[t->sub_count++] = client_addr;
                    printf("✅ Suscriptor agregado al topic %s (%d total)\n",
                           t->topic, t->sub_count);
                }
            }
        } else {
            // formato esperado: TOPIC|MENSAJE
            char *sep = strchr(buffer, '|');
            if (!sep) continue;

            *sep = '\0';
            char *topic_name = buffer;
            char *payload = sep + 1;

            int t_index = find_topic(topics, topic_count, topic_name);
            if (t_index == -1) {
                printf("Mensaje recibido para topic desconocido: %s\n", topic_name);
                continue;
            }

            Topic *t = &topics[t_index];
            printf("Mensaje en %s: %s\n", topic_name, payload);

            for (int i = 0; i < t->sub_count; i++) {
                sendto(sockfd, payload, strlen(payload), 0,
                       (struct sockaddr *)&t->subs[i], sizeof(t->subs[i]));
            }
        }
    }

    close(sockfd);
    return 0;
}
