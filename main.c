#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <ctype.h>

char response_ok_part1[] = "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                  "<!DOCTYPE html><html><head><title>PSI-ukol2</title>"
                  "<body><h1>Hello PSI</h1>"
                  "<p>";

char response_ok_part2[] = "</p></body></html>\r\n";

char response_wrong_part[] = "HTTP/1.0 400 Bad Request\r\n";

/*
 * Nove vlakno pro obsluhu prichoziho pozadavku
 */
void* serve_request(void *arg)
{
    int client_sock;
    char cbuf[4000];
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    memset(cbuf, 0, 4000);

    client_sock = *(int *) arg;
    read(client_sock, cbuf, 4000);

    /* Byla přijata zpráva s validni hlavickou HTTP GET zpravy */
    if(strncmp(cbuf, "GET / HTTP/1.1", 14) == 0){
        printf("%s\n",cbuf);
        memset(cbuf, 0, 4000);
        sprintf(cbuf, "%s %s %s", response_ok_part1, asctime (timeinfo), response_ok_part2);
        write(client_sock, cbuf, sizeof(cbuf) - 1);

    }else{ /* Byla přijata nějaká podezrela zprava vrat 'Bad Request'*/
        write(client_sock, response_wrong_part, sizeof(response_wrong_part) - 1);
    }


    close(client_sock);

    free(arg);

    return 0;
}

bool utils_validate_number(char *str) {
    while (*str) {
        if(!isdigit(*str)){
            return false;
        }
        str++;
    }
    return true;
}


int server_loop(int *server_sock){
    int client_sock;
    int *th_socket;
    struct sockaddr_in remote_addr;
    socklen_t remote_addr_len;
    pthread_t thread_id;

    while(1)
    {
        client_sock = accept(*server_sock, (struct sockaddr *)&remote_addr, &remote_addr_len);

        if (client_sock > 0)
        {
            th_socket = malloc(sizeof(int));
            *th_socket = client_sock;
            pthread_create(&thread_id, NULL, (void *)&serve_request, (void *)th_socket);
        }
        else
        {
            printf("Fatal ERROR - accept \n");
            return -1;
        }
    }
}


int start_server(int *server_sock, char *ip, char *port){
    int return_value;
    int param = 1;
    struct sockaddr_in addr;




    inet_pton(AF_INET, ip, &(addr.sin_addr));
    addr.sin_port        = htons(atoi(port));
    addr.sin_family = AF_INET;

    *server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (*server_sock <= 0)
    {
        printf("Socket ERR\n");
        return -1;
    }

    return_value = setsockopt(*server_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&param, sizeof(param));

    if (return_value == -1){
        printf("setsockopt ERR\n");
    }

    return_value = bind(*server_sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

    if (return_value == 0)
        printf("Bind OK\n");
    else
    {
        printf("Bind ERR\n");
        return -1;
    }

    return_value = listen(*server_sock, 10);
    if (return_value == 0){
        printf("Listen OK\n");
    }else{
        printf("Listen ERR\n");
        return -1;
    }
    return 0;
}

bool utils_is_valid_ip_address(char *ipAddress){
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}

bool check_parametrs(char *ip, char *port){
    int port_num;

    if(utils_is_valid_ip_address(ip) == false){
        return false;
    }

    if(utils_validate_number(port) == true){
        port_num = atoi(port);

        if(port_num > 65535 || port_num < 1){
            return false;
        }
    }else{
        return false;
    }

    return true;
}

void print_usage(){
    printf("./simpleHttp <ip address> <port>\n");
}

int main (int argc, char **argv)
{
    int server_sock;
    int client_sock;
    int return_value;
    int *th_socket;
    struct sockaddr_in addr;
    struct sockaddr_in remote_addr;
    socklen_t remote_addr_len;
    pthread_t thread_id;


    if(argc == 3){
        if(check_parametrs(argv[1], argv[2]) == true){


       ;

            server_sock = socket(AF_INET, SOCK_STREAM, 0);

            if (server_sock <= 0)
            {
                printf("Socket ERR\n");
                return -1;
            }

            memset(&addr, 0, sizeof(struct sockaddr_in));

            inet_pton(AF_INET, argv[1], &(addr.sin_addr));
            addr.sin_port        = htons(atoi(argv[2]));
            addr.sin_family = AF_INET;

            int param = 1;
            return_value = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&param, sizeof(int));

            if (return_value == -1)
                printf("setsockopt ERR\n");

            return_value = bind(server_sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));

            if (return_value == 0)
                printf("Bind OK\n");
            else
            {
                printf("Bind ERR\n");
                return -1;
            }

            return_value = listen(server_sock, 5);
            if (return_value == 0)
                printf("Listen OK\n");
            else
            {
                printf("Listen ERR\n");
                return -1;
            }


            while(1)
            {
                client_sock = accept(server_sock, (struct sockaddr *)&remote_addr, &remote_addr_len);

                if (client_sock > 0)
                {
                    th_socket = malloc(sizeof(int));
                    *th_socket = client_sock;
                    pthread_create(&thread_id, NULL, (void *)&serve_request, (void *)th_socket);
                }
                else
                {
                    printf("Fatal ERROR - Accept\n");
                    return -1;
                }
            }
        }else{
            print_usage();
        }
   }else{
        print_usage();
    }

    return 0;
}