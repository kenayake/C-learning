#include <stdio.h>
#include <stdlib.h>

int bloat(int x) {
    return x * bloat(x + 1);
}

int main(int argc, char const *argv[])
{
    bloat(1);

    // Loop till all memory used up
    // while (1)
    // {
    //     int *i = malloc(4);
    //     if (i == NULL) {
    //         break;
    //     }
    // }
    
    

    return 0;
}
