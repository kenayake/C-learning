#include <stdio.h>
#include <stdlib.h>



int main(int argc, char const *argv[])
{
    for (int i = 0; i < 1000000; i++)
    {
        int *garbo = malloc(1024);
    }

    int lmao;

    scanf("%d", &lmao);
    
    return 0;
}
