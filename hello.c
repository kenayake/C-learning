#include <stdio.h>

typedef struct {
    double x;
    double y;
    double z;
} Vector3;

void scale(Vector3* vector, double factor) {
    vector->x *= factor;
    vector->y *= factor;
    vector->z *= factor;
}

int main() {
    
    Vector3 vector = { 3, 2, 5 };

    scale(&vector, 3);


    return 0;
}