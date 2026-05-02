#include <stdio.h>
#include <stdlib.h>



int main(int argc, char const *argv[])
{
    int *arr = malloc(10*sizeof(int));

    int arrLength = 10;

    // for (int i = 0; i < arrLength; i++)
    // {
    //     *(arr + i) = 10 + 10 * i;
    //     printf("%p\n", (arr + i));
    //     printf("%d\n", *(arr + i));
    // }

    printf("%d", arr[1000000]);
    
    return 0;
}
