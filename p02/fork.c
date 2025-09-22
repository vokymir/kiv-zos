#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){
    int i;
    i = fork();
    if (i < 0)
        printf("Nepodarilo se vytvorit proces.\n");
    else if (i == 0)
    {
        int x = getpid();
        printf("Dite ma PID=%i a umi koukat okolo:\n", x);
        execve("/bin/ls",NULL,NULL);
    }
    else
        printf("Toto je rodic od ditete s PID: %i\n",i);

    return  EXIT_SUCCESS;
}
