
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct sockaddr_in tmp_client_address;
struct sockaddr_in server_address;
char buffer[10 * BUFFER_SIZE];

int server_socket;
int tmp_address_length;

void throw(char *message)
{
    fprintf(stderr, "[E]: %s \n", message);
    close(server_socket);
    exit(errno);
}

void log_i(char *message)
{
    printf(ANSI_COLOR_CYAN "[%i | I]: " ANSI_COLOR_RESET "%s \n", time(0), message);
}

int sum(int a, int b)
{
    return a + b;
}

char *name(char *name, int age, int group)
{
    char result[BUFFER_SIZE];
    sprintf(result, "Hello, %s! Seems you were born in %d. Your TA on Networks is %s.\n", name, age, group == 3 ? "Adam Zubair" : "Nikita Bogomazov");
    return result;
}

void *process_connection(void *vargp)
{
    char log_msg[64];
    sprintf(log_msg, "{thread %i} Created", pthread_self());
    log_i(log_msg);

    request_struct_t *request_body = (request_struct_t *)vargp;

    char message[BUFFER_SIZE];

    sprintf(log_msg, "{thread %i} Data received: %s %s %s %s", pthread_self(), request_body->argv[0], request_body->argv[1], request_body->argv[2], request_body->argv[3]);
    log_i(log_msg);

    // Check for resourse type
    if (!strcmp(request_body->argv[0], "sum"))
    {
        int sum_res = sum(atoi(request_body->argv[1]), atoi(request_body->argv[2]));
        sprintf(message, "%d", sum_res);
    }

    if (!strcmp(request_body->argv[0], "name"))
    {
        sprintf(message, "%s", name(request_body->argv[1], atoi(request_body->argv[2]), atoi(request_body->argv[3])));
    }

    sprintf(log_msg, "{thread %i} Sending answer to client", pthread_self());
    log_i(log_msg);

    sendto(server_socket, message, BUFFER_SIZE, 0,
           (struct sockaddr *)&tmp_client_address, sizeof(tmp_client_address));

    sprintf(log_msg, "{thread %i} Sleeping thread", pthread_self());
    log_i(log_msg);

    sleep(10);

    sprintf(log_msg, "{thread %i} Sleep ended", pthread_self());
    log_i(log_msg);
}

void init_udp_server()
{
    // Create a UDP Socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1)
        throw("Socket creation failed");
    else
        log_i("Server socket created");

    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);
    server_address.sin_family = AF_INET;
    log_i("Server instance created");

    // Bind server address to socket descriptor
    int bind_code = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (bind_code == -1)
        throw("Socket binding failed");
    else
        log_i("Socket bound");

    // Receive datagram
    while (1)
    {
        tmp_address_length = sizeof(struct sockaddr_in);
        ssize_t bytes_received = recvfrom(server_socket, buffer, sizeof(buffer),
                                          0, (struct sockaddr *)&tmp_client_address, &tmp_address_length);

        log_i("Connection received, creating thread");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, process_connection, buffer);
    }
}

int main()
{
    init_udp_server();
    return 0;
}
