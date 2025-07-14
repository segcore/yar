#undef NDEBUG // Force-enable asserts
#include <assert.h>
#include "yar.c"

int main()
{
    yar(double) things = {0};
    double* x;

    double ones;
    double twos;
    double threes;
    memset(&ones, 1, sizeof(ones));
    memset(&twos, 2, sizeof(twos));
    memset(&threes, 3, sizeof(threes));

    x = yar_reserve(&things, 100);
    assert(things.count == 0);
    assert(things.capacity >= 100);
    assert(x == things.items);

    things.count += 100;
    assert(things.count == 100);
    memset(x, 1, sizeof(double) * 100);

    // Reserve 1000, but only use 100
    x = yar_reserve(&things, 1000);
    assert(things.count == 100);
    assert(things.capacity >= 1100);
    assert(x == things.items + 100);

    memset(x, 2, sizeof(double) * 100);
    things.count += 100;

    // Reserver another 1000
    x = yar_reserve(&things, 1000);
    assert(things.count == 200);
    assert(things.capacity >= 1200);
    assert(x == things.items + 200);
    memset(x, 3, sizeof(double) * 1000);

    for(int i = 0; i < 100; i++) {
        x = &things.items[i];
        assert(memcmp(x, &ones, sizeof(double)) == 0);
    }
    for(int i = 100; i < 200; i++) {
        x = &things.items[i];
        assert(memcmp(x, &twos, sizeof(double)) == 0);
    }
    for(int i = 200; i < 1200; i++) {
        x = &things.items[i];
        assert(memcmp(x, &threes, sizeof(double)) == 0);
    }

    yar_free(&things);
}
