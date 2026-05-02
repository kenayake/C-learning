#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char const *argv[])
{
    char *buffer = malloc(5);
    char *buffer2 = malloc(4);
    printf("%zu\n", sizeof *(buffer));

    strcpy(buffer, "Hello!!!!!!!!!Hello!!!!!!!!!Hello!!!!!!!!!Hello!!!!!!!!!");

    printf("%zu\n", strlen(buffer) );
    printf("%s\n", buffer);
    printf("%s", buffer2);
    free(buffer);
    free(buffer2);
    return 0;
}
