#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
    if (fork() == 0)
    {
        printf("Jdu spat...\n");
        sleep(3);
        printf("Vzbudil jsem se...\n");
        exit(0);
    }
    else
    {
        printf("Cekam na potomka, az se vzbudi...\n");
        wait(NULL);
        printf("Aaa, uz se vzbudil...\n");
    }

    return  EXIT_SUCCESS;
}
