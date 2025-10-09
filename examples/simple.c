#define YAR_IMPLEMENTATION
#include "yar.h"
#include <stdio.h>

// You can use your own structures
void user_struct()
{
    typedef struct {
        double* items;
        size_t count;
        size_t capacity;
    } FavouriteNumbers;

    FavouriteNumbers numbers = {0};

    *yar_append(&numbers) = 3.14159;
    *yar_append(&numbers) = 2.71828;
    *yar_append(&numbers) = 1.61803;

    // Type error: error: incompatible types when assigning to type ‘double’ from type ‘char *’
    // *yar_append(&numbers) = "hello";

    for(size_t i = 0; i < numbers.count; i++) {
        printf("User struct: %f\n", numbers.items[i]);
    }
    yar_free(&numbers);
}

// Or use the macro
void macro_struct()
{
    yar(double) numbers = {0};

    *yar_append(&numbers) = 3.14159;
    *yar_append(&numbers) = 2.71828;
    *yar_append(&numbers) = 1.61803;

    for(size_t i = 0; i < numbers.count; i++) {
        printf("Macro struct: %f\n", numbers.items[i]);
    }
    yar_free(&numbers);
}

// Yar-arrays can be used inside structures, and with types that contain spaces
void inside_structs()
{
    typedef struct {
        const char* name;
        int age;
        yar(const char*) aliases;
    } Person;

    Person your = {0};
    your.name = "Timothy";
    your.age = 23;
    *yar_append(&your.aliases) = "Captain Lobsterlegs";
    *yar_append(&your.aliases) = "Avocado Toast Man";
    *yar_append(&your.aliases) = "The T-Bone";

    printf("Hello %s, or should I call you:\n", your.name);
    for(size_t i = 0; i < your.aliases.count; i++) {
        printf("   %s\n", your.aliases.items[i]);
    }
    yar_free(&your.aliases);
}

// Dynamic arrays containing structures...
void of_structs()
{
    typedef struct {
        int x;
        int y;
        const char* message;
    } Message;

    yar(Message) messages = {0};

    Message* m = yar_append(&messages);
    m->x = 100; m->y = 200; m->message = "Accepted connection";

    m = yar_append(&messages);
    m->x = 100; m->y = 300; m->message = "Closed connection";

    for(size_t i = 0; i < messages.count; i++) {
        m = &messages.items[i];
        printf("(%d, %d) => %s\n", m->x, m->y, m->message);
    }

    yar_free(&messages);
}

int main()
{
    user_struct();
    macro_struct();
    inside_structs();
    of_structs();
}
