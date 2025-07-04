#define YAR_IMPLEMENTATION
#include "yar.h"
#include <stdio.h>

typedef struct {
    char* items;
    size_t count;
    size_t capacity;
} StringBuilder;

int main(int argc, char** argv)
{
    StringBuilder sb = {0};

    // --- yar_append_cstr
    // Append nul-terminated strings
    yar_append_cstr(&sb, "Some data");
    yar_append_cstr(&sb, " and some more...");
    yar_append_cstr(&sb, " more again...");
    *yar_append(&sb) = '\0'; // nul terminate
    fprintf(stderr, "Final string: %s\n", sb.items);


    // --- yar_append_many
    // Appends a copy of an existing sequence of data
    StringBuilder copy = {0};
    char hello[] = "Hello there! ";
    yar_append_many(&copy, hello, strlen(hello));
    yar_append_many(&copy, sb.items, sb.count);
    fprintf(stderr, "Copied string: %s\n", copy.items);

    // re-use the already-allocated memory of `sb` for the next example
    sb.count = 0; // or, if you prefer: yar_reset(&sb);

    // --- yar_reserve
    // Reserve ensures that the data is available in the buffer, but does not increase the count.
    // This means that data can be placed directly in the final memory.

    if (argc > 0) {
        FILE* fp = fopen(argv[0], "r");
        if(fp != NULL) {
            size_t bufsize = 4096;
            char* buf = yar_reserve(&sb, bufsize);
            size_t read_count;
            while((read_count = fread(buf, sizeof(char), bufsize, fp)) > 0) {
                sb.count += read_count;
                buf = yar_reserve(&sb, bufsize);
            }
            *yar_append(&sb) = '\0'; // nul terminate
            fclose(fp);
        }
    }

    fprintf(stderr, "This program has %zu bytes\n", sb.count);

    yar_free(&sb);
    yar_free(&copy);
}
