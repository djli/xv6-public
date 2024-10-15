#include "types.h"
#include "user.h"
#include "stat.h"
#include "condvar.h"
#include "fcntl.h"


int main() {
    struct condvar cv;
    int fd = open("flag", O_RDWR | O_CREATE);
    init_lock(&cv.lk);
    int pid = fork();
    if(pid<0){
        printf(1,"Error forking first child.\n");
    }else if (pid == 0){
        sleep(5);
        printf(1,"child 1 executing\n");
        lock(&cv.lk);
        write(fd, "done", 4);
        cv_signal(&cv);
        unlock(&cv.lk);
    }else{
        pid = fork();
        if(pid<0){
            printf(1,"Error forking second child.\n");    
            }else if (pid == 0){
                lock(&cv.lk);
                struct stat stats;
                fstat(fd, &stats);
                printf(1, "file size: %d\n", stats.size);
                while(stats.size <= 0){
                    cv_wait(&cv);
                    fstat(fd, &stats);
                    printf(1, "file size: %d\n", stats.size);
                }
                cv_wait(&cv);
                unlock(&cv.lk);
                printf(1,"child 2 executing\n");
            }else{
                printf(1, "parent waiting\n");
                int i;
                for(i=0;i<2;i++){
                    wait();
                }
                printf(1, "children completed\n");
                printf(1, "parent executing\n");
                printf(1, "parent exiting\n");
            }
    }
    close(fd);
    unlink("flag");
    exit();
}