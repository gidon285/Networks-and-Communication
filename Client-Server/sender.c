#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#define BYTESIZE 1024
#define PORT 9010

/**
 * @brief The main sending function of the project, sends a file 5 times
 * 
 * @param sender_socket The server socket to send the file to
 * @param filename the file path
 * @return int the number of times the file was sent, -1 if there was an error
 */
int send_file(int sender_socket, char *filename) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("!!| Error in file reading");
        exit(1);
        return -1;
    }    
    struct stat filesize_buffer;
    stat(filename, &filesize_buffer);
    int filesize = filesize_buffer.st_size;
    int for_loop_index = filesize / BYTESIZE;
    int file_remainder = filesize % BYTESIZE;
    int read_validation = 0;
    int total_bytes_sent = 0;
    int sending_count = 0;
    char cc_type[256];
    int send_status = 0;
    char data[BYTESIZE] = {0};
    int iteration_counter = 0;
    socklen_t len = sizeof(cc_type); 
    getsockopt(sender_socket, IPPROTO_TCP, TCP_CONGESTION, cc_type, &len);
    printf("==| Current congestion control algorithm: %s\n", cc_type); 
    for(int i=0; i < 5; i++) {
        for (int j=0; j < for_loop_index; j++) {
            read_validation = fread(data, BYTESIZE, 1, file);
            if (read_validation < 0) {
                perror("!!| Error in reading file.");
                exit(1);
                return -1;                
            }
            send_status = send(sender_socket, data, BYTESIZE, 0);
            total_bytes_sent += send_status;
            if (send_status == -1) {
                perror("!!| Error in sending file.");
                exit(1);
                return -1;
            }
            iteration_counter++;         
        }
        if (file_remainder > 0) {
            read_validation = fread(data, file_remainder, 1, file);
            if (read_validation < 0) {
                perror("!!| Error in reading file.");
                exit(1);
                return -1;                
            }            
            send_status = send(sender_socket, data, file_remainder, 0);
            total_bytes_sent += send_status;
            if (send_status == -1) {
                perror("!!| Error in sending file.");
                exit(1);
                return -1;
            }
            iteration_counter++;          
        }
        sending_count++;
        rewind(file);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t start_precision = (start.tv_sec*1000000000) + start.tv_nsec;
    uint64_t end_precision = (end.tv_sec*1000000000) + end.tv_nsec;
    double operation_time = (end_precision - start_precision) * 1e-9;
    printf("==| %s sending time - %f Seconds\n",cc_type,operation_time);
    printf("==| Number of iterations in %s algorithm = %d\n",cc_type, iteration_counter);
    printf("==| Bytes Sent in %s %d\n",cc_type, total_bytes_sent);
    fclose(file);
    return sending_count;
}

int main() {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    char cc_type[256];
    socklen_t len = sizeof(cc_type);
    int sending_count = 0;
    char *ip = "127.0.0.1";
    struct sockaddr_in server_addr;
    char *filename = "input/1mb.txt";
    struct stat filesize_buffer;
    stat(filename, &filesize_buffer);
    int filesize = filesize_buffer.st_size;
    printf("_____________________________Sender (Client)_____________________________\n");
    //Settings the client socket
    int sender_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(sender_socket < 0) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("==| Server socket created successfully.\n");
    //Checking CC algorithm
    //Setting client address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    int socket_validation = connect(sender_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(socket_validation == -1) {
        perror("!!| Error in socket");
        exit(1);
    }
    printf("==| Connected to Server.\n");
    printf("==| Sending received file size: %d Bytes\n", filesize);
    send(sender_socket, &filesize, sizeof(filesize), 0);
    sending_count += send_file(sender_socket, filename);
    printf("==| File was sent a total of %d times successfully\n",sending_count);
    strcpy(cc_type, "reno"); 
    len = strlen(cc_type);
    if (setsockopt(sender_socket, IPPROTO_TCP, TCP_CONGESTION, cc_type, len) != 0) {
        perror("!!| Set CC to reno Problem"); 
        return -1;
    }
    printf("==| Congestion Control Algorithm changed to %s, Resending files\n", cc_type); 
    sending_count += send_file(sender_socket, filename);
    printf("==| File was sent a total of %d times successfully\n",sending_count);
    close(sender_socket);
    clock_gettime(CLOCK_MONOTONIC, &end);
    uint64_t start_precision = (start.tv_sec*1000000000) + start.tv_nsec;
    uint64_t end_precision = (end.tv_sec*1000000000) + end.tv_nsec;
    double total_sending_time = (end_precision - start_precision) * 1e-9;
    printf("==|__________Total sending process time = %f__________|\n", total_sending_time);
    return 0;
}