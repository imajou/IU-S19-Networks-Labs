#include "common.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_ADDRESS "192.168.0.0"

int client_socket;
char buffer[BUFFER_SIZE];
struct sockaddr_in server_socket;

void throw(char *message)
{
    fprintf(stderr, "[E]: %s \n", message);
    close(client_socket);
    exit(errno);
}

void log_i(char *message)
{
    printf(ANSI_COLOR_CYAN "[%i | I]: " ANSI_COLOR_RESET "%s \n", time(0), message);
}

void init_udp_connection()
{
    // Acquire server parameters
    server_socket.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    server_socket.sin_port = htons(PORT);
    server_socket.sin_family = AF_INET;

    log_i("Client initialized");

    // Create socket
    client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket == -1)
        throw("Socket creation failed");
    else
        log_i("Socket created");

    // Connect to server
    int connect_code = connect(client_socket, (struct sockaddr *)&server_socket, sizeof(server_socket));
    if (connect_code == -1)
        throw("Connection failed");
    else
        log_i("Connected to server");
}

void send_request(request_struct_t *data)
{
    // Send datagram
    ssize_t bytes_sent = sendto(client_socket, data, sizeof(request_struct_t), 0, (struct sockaddr *)NULL, sizeof(server_socket));
    if (bytes_sent <= 0)
        throw("Sending failed");
    else
        log_i("Request sent");

    // Wait for response
    ssize_t bytes_received = recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr *)NULL, NULL);
    if (bytes_received <= 0)
        throw("No message received");
    else
    {
        log_i("Message received:");
        log_i(buffer);
    }

    // Close socket descriptor
    close(client_socket);
}

request_struct_t *convert_args(int argc, char *argv[])
{

    request_struct_t *data = malloc(sizeof(request_struct_t));
    data->argc = argc - 1;

    for (int i = 0; i < argc - 1; i++)
    {
        strcpy(data->argv[i], argv[i + 1]);
    }

    return data;
}

int main(int argc, char *argv[])
{
    init_udp_connection();

    send_request(convert_args(argc, argv));

    return 0;
}
