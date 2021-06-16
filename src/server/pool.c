#include "helper.h"

#define MAX_PROC 1


int main(void){
    char *response = get_response();

    int result;
    int sd = -1, new_sd = -1;
    int current_size, nfds = 1;
    int close_conn = 0, compress_array = 0;
    char buffer[65500];
    struct pollfd fds[5];

    memset(fds, 0 , sizeof(fds));

    for(int i = 0; i < MAX_PROC; i++){
        sd = create_socket(8080, 20);
        fds[i].fd = sd;
        fds[i].events = POLLIN;
    }

    for(int l = 0; l < 4; l++){
        if(poll(fds, nfds, 30000) < 0){
            printf("poll\n");
            exit(-1);
        }

        current_size = nfds;
        for(int i = 0; i < current_size; i++){
            if(fds[i].revents == 0){
                continue;
            }

            if(fds[i].revents != POLLIN) {
                printf("Error! revents = %d\n", fds[i].revents);
                exit(-1);
            }

            printf("I = %d\n", i);
            printf("SD = %d\n", sd);
            printf("FDS.FD = %d\n\n", fds[i].fd);

            if(fds[i].fd == sd){
                do{
                    new_sd = accept(fds[i].fd, NULL, NULL);
                    if(new_sd < 0){
                        if(errno != EWOULDBLOCK){
                            printf("Error accept\n");
                            exit(-1);
                        }
                        printf("err");
                        break;
                    }

                    printf("Accepted\n\n");
                    // add to poll queue
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;

                } while(new_sd != -1);
            }
            else{
                close_conn = 0;

                do {
                    result = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    if (result < 0) {
                        if (errno != EWOULDBLOCK) {
                            printf("  recv() failed\n");
                            close_conn = 1;
                        }
                    }
                    else if(result == 0){
                        // connection closed
                        close_conn = 1;
                    }

                    break;

                } while(1);

                result = send(fds[i].fd, response, strlen(response), 0);
                if (result < 0) {
                    printf("send\n");
                    close_conn = 1;
                    break;
                }

                // Connection should be closed since this is just a test
                close_conn = 1;
            }

            if (close_conn) {
                close(fds[i].fd);
                fds[i].fd = -1;
                compress_array = 1;
                break;
            }
        }

        if (compress_array) {
            compress_array = 0;
            for (int i = 0; i < nfds; i++) {
                if (fds[i].fd == -1) {
                    for(int j = i; j < nfds-1; j++) {
                        fds[j].fd = fds[j+1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
            break;
        }
    }

    close(sd);
}