#undef NDEBUG // Force-enable asserts
#include <assert.h>
#include "yar.c"

typedef struct Thing {
    float w;
} Thing;

int main()
{
    yar(struct Thing) things = {0};
    Thing* x;

    x = yar_reserve(&things, 100);
    assert(things.count == 0);
    assert(things.capacity >= 100);
    assert(x == things.items);

    things.count += 100;
    assert(things.count == 100);

    for(int i = 0; i < 100; i++) {
        // We zero things for now, not sure if we always will
        assert(things.items[i].w == 0);
        things.items[i].w = i;
    }

    // Remove nothing
    yar_remove(&things, 0, 0);
    assert(things.count == 100);
    for(int i = 0; i < 100; i++) {
        assert(things.items[i].w == i);
    }

    // Remove nothing elsewhere
    yar_remove(&things, 100, 0);
    assert(things.count == 100);
    for(int i = 0; i < 100; i++) {
        assert(things.items[i].w == i);
    }

    yar_remove(&things, 50, 0);
    assert(things.count == 100);
    for(int i = 0; i < 100; i++) {
        assert(things.items[i].w == i);
    }

    // Remove at beginning
    yar_remove(&things, 0, 1);
    assert(things.count == 99);
    for(int i = 0; i < 99; i++) {
        assert(things.items[i].w == i + 1);
    }

    // Remove at end
    yar_remove(&things, 98, 1);
    assert(things.count == 98);
    for(int i = 0; i < 98; i++) {
        assert(things.items[i].w == i + 1);
    }

    // Remove in the middle
    yar_remove(&things, 50, 10);
    assert(things.count == 88);
    for(int i = 0; i < 88; i++) {
        int expected = (i < 50) ? (i + 1) : (i + 11);
        assert(things.items[i].w == expected);
    }

    // Remove all
    yar_remove(&things, 0, things.count);
    assert(things.count == 0);
    assert(things.capacity >= 100);
    assert(things.items != NULL);

    yar_free(&things);
}
