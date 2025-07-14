#undef NDEBUG // Force-enable asserts
#include <assert.h>
#include <string.h>
#include "yar.c"

int main()
{
    yar(double) arr = {0};
    double data[10] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    yar_append_many(&arr, data, 10);
    assert(arr.count == 10);
    assert(arr.capacity >= 10);

    yar_append_many(&arr, data, 10);
    assert(arr.count == 20);
    assert(arr.capacity >= 20);

    yar_append_many(&arr, data, 10);
    assert(arr.count == 30);
    assert(arr.capacity >= 30);

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 10; j++) {
            assert(arr.items[i*10 + j] == (10.0 - j));
        }
    }

    size_t old_count = arr.count;
    double* begin = yar_append_many(&arr, data, 1);
    assert(begin == &arr.items[old_count]);
    assert(arr.count == old_count + 1);
    assert(*begin == 10);

    old_count = arr.count;
    begin = yar_append_many(&arr, data, 2);
    assert(begin == &arr.items[old_count]);
    assert(arr.count == old_count + 2);
    assert(begin[0] == 10);
    assert(begin[1] == 9);

    old_count = arr.count;
    begin = yar_append_many(&arr, data, 3);
    assert(begin == &arr.items[old_count]);
    assert(arr.count == old_count + 3);
    assert(begin[0] == 10);
    assert(begin[1] == 9);
    assert(begin[2] == 8);

    yar_free(&arr);


    yar(char) str = {0};
    char text[] = "Hello, there!";
    size_t len = strlen(text);

    yar_append_cstr(&str, text);
    assert(str.count == len);

    yar_append_cstr(&str, text);
    assert(str.count == len*2);

    yar_append_cstr(&str, text);
    assert(str.count == len*3);

    yar_append_cstr(&str, "asdf");
    assert(str.count == len*3+4);

    *yar_append(&str) = '\0';

    assert(strcmp(str.items, "Hello, there!Hello, there!Hello, there!asdf") == 0);

    str.count--;
    yar_append_many(&str, text, 5);
    assert(str.count == len*3+4+5);

    *yar_append(&str) = '\0';
    assert(strcmp(str.items, "Hello, there!Hello, there!Hello, there!asdfHello") == 0);

    yar_free(&str);
}

