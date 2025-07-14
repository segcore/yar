#undef NDEBUG // Force-enable asserts
#include <assert.h>
#include "yar.c"

int main()
{
    yar(int) ints = {0};
    *yar_append(&ints) = 10;
    assert(ints.count == 1);
    assert(ints.capacity == YAR_MIN_CAP);
    assert(ints.items[0] == 10);

    *yar_append(&ints) = 20;
    assert(ints.count == 2);
    assert(ints.capacity == YAR_MIN_CAP);
    assert(ints.items[0] == 10);
    assert(ints.items[1] == 20);

    *yar_append(&ints) = 30;
    assert(ints.count == 3);
    assert(ints.capacity == YAR_MIN_CAP);
    assert(ints.items[0] == 10);
    assert(ints.items[1] == 20);
    assert(ints.items[2] == 30);

    // New values must be zeroed
    for(int i = 0; i < 1000; i++) {
        int* x = yar_append(&ints);
        assert(*x == 0);
    }

    size_t count = ints.count;
    assert(count == 1003);
    for(int i = 0; i < 5000; i++) {
        int* x = yar_append(&ints);
        *x = i;
    }
    assert(ints.items[0] == 10);
    assert(ints.items[1] == 20);
    assert(ints.items[2] == 30);
    assert(ints.items[3] == 0);
    assert(ints.items[4] == 0);
    assert(ints.items[count] == 0);
    assert(ints.items[count + 1] == 1);
    for(int i = 0; i < 5000; i++) {
        assert(ints.items[count + i] == i);
    }

    yar_free(&ints);
}
