#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "stdio.h"
#include "stdlib.h"
#include "signal.h"

#define BACKLOG 5

volatile sig_atomic_t exit_flag = 0;

void signal_handler(int signum) {
    printf(" ...Ctrl-C pressed...\n");
    exit_flag = 1;
}

int main(void) {

    struct sockaddr_in local;
    int s; // socket
    int s1; // accepted socket
    int rc; // return code
    struct sigaction act;

    printf("TinyNetSrv started\n");

    act.sa_handler = signal_handler;
    sigaction(SIGINT, &act, NULL);


    local.sin_family = AF_INET;
    local.sin_port = htons(19995);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s != -1) {
        rc = bind(s, (const struct sockaddr *)&local, sizeof(local));
        if (rc==0) {
            rc = listen(s, BACKLOG);
            if (rc==0) {
                do{
                    struct sockaddr_in addr;
                    int adr_len = sizeof(addr);
                    s1 = accept(s, (struct sockaddr *)&addr, &adr_len);
                    if (s1 != -1) {
                        printf("Connection accepted: %s\n", inet_ntoa(addr.sin_addr));
                        int t = 1;
                        rc = setsockopt(s1,SOL_SOCKET,SO_REUSEADDR,&t,sizeof(int));
                        if (rc == -1) {
                            perror("'setsockopt' call error");
                        }
                        // TODO: call handler thread
                        // TODO: remove it (it is example):
                        char resp[] = "HTTP/1.0 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html><html><body><h1>Sample of response</h1><body></html>";
                        rc = send(s1, (void *)resp, sizeof(resp)-1, 0);
                        if (rc == -1) {
                            perror("'send' call error");
                        }
                        shutdown(s1, SHUT_RDWR);
                    }
                } while (exit_flag == 0);
                shutdown(s, SHUT_RDWR);
            }
            else
            {
                perror("'listen' call error");
                exit(1);
            }
        }
        else
        {
            perror("'bind' call error");
            exit(1);
        }
    }
    else
    {
        perror("'socket' call error");
        exit(1);
    }

    printf("TinyNetSrv finished!\n");
    exit(0);
}