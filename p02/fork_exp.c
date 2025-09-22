#include <unistd.h>
#include <stdio.h>

int main(void)
{
   for (int i = 0; i < 10; i++)
    {
        fflush(stdout);
        printf("i=%i\tPID=%i\n",i,fork());
    }
}
