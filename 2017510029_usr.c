#include <stdio.h>
#include <string.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    long int opt;
    if(argc == 1){
        opt = syscall(335,0,"0");
    }
    else if(argc == 2){
        opt = syscall(335,1,"0");
    }
    else if(argc == 3){
        if(strcmp(argv[1],"-p") == 0){
            opt = syscall(335,2,argv[2]);
        }
        else {
            opt = syscall(335,3,argv[2]);
        }
    }
    return 0;
}
