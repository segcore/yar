#define YAR_IMPLEMENTATION
#include "yar.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    float vertexs[12000];
    unsigned colours[12000];
    int count;
} LargeStruct;

static bool parse_mesh(LargeStruct* mesh);

int main()
{
    yar(LargeStruct) meshes;

    // Allocate a large struct directly on the heap, to fill it in
    LargeStruct* mesh = yar_append(&meshes);

    mesh->count = 5000; // ... pretend this is more realistic
    for (int i = 0; i < mesh->count; i++) {
        mesh->vertexs[i] = 10 * i/100.0f;
    }

    // Reserve if it 'might' be used, and increment count if it is.
    LargeStruct* might_be_used = yar_reserve(&meshes, 1);
    if (parse_mesh(might_be_used)) {
        meshes.count += 1;
    }

    // Note: no need to create a LargeStruct on the stack like this.
    LargeStruct next = {0};
    *yar_append(&meshes) = next;

    printf("Created %zu meshes. Capacity before a realloc is required: %zu\n", meshes.count, meshes.capacity);

    yar_free(&meshes);
}


static bool parse_mesh(LargeStruct* mesh)
{
    FILE* f = fopen("mesh", "rb");
    if(f == NULL) {
        return false;
    }
    // ...
    mesh->count = fread(mesh->vertexs, 12000, sizeof(float), f);
    return true;
}

