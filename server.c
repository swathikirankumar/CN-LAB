#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>

#define eerror(msg) { printf("%s\n", msg); exit(1); }
#define sfifo "c2s_fifo.dat"
#define MAX 10

int fd; char buf[128], tmp[128];
struct pollfd plfd[MAX]; int nClient=1;
int wfd[MAX];
void acceptReq(const char arg[]){
    printf("Got a req %s\n", arg);
    strcpy(tmp, arg);
    strcat(tmp, "r.dat");
    wfd[nClient] = open(tmp, O_WRONLY);
    if(wfd[nClient] == -1) eerror("WERROR!!!");
    
    strcpy(tmp, arg);
    strcat(tmp, "w.dat");
    plfd[nClient].fd = open(tmp, O_RDONLY);
    plfd[nClient].events = POLLIN;
    if(plfd[nClient].fd == -1) eerror("RERROR!!!");
    nClient++;
    printf("Req success %d\n", nClient);
}

int main (){
    mkfifo(sfifo, 0666); int i,j;
    fd = open(sfifo, O_RDONLY);
    if(fd == -1) eerror("sfifo error");
    plfd[0].fd = fd;
    plfd[0].events = POLLIN;
    
    while(1){
        int res=poll(plfd, nClient, 500);
        if(res > 0){
            for(i=0; i<nClient; i++){
                if(plfd[i].revents & POLLIN){
                    memset(buf, 128, '\0');
                    read(plfd[i].fd, buf, 128);
                    if (i==0){
                        //if(nClient == 3)
                        //    printf("%s\n", buf);
                        //else
                            acceptReq(buf);
                    } else {
                        printf("Send %s to all\n", buf);
                        for(j=1; j<nClient; j++){
                            if(j != i)
                            write(wfd[j], buf, strlen(buf)+1);
                        }
                    }
                    printf("Caught Signal\n");
                }
            }
        } else{
            printf("poll not worked\n");
        }
    }
    return 0;
}
