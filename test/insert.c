#undef NDEBUG // Force-enable asserts
#include <assert.h>
#include "yar.c"

typedef struct {
    float w;
} Thing;

int main()
{
    yar(Thing) things = {0};
    Thing* x;

    yar_insert(&things, 0, 0);
    assert(things.count == 0);

    x = yar_insert(&things, 0, 1);
    assert(things.count == 1);
    assert(x == things.items);
    assert(x->w == 0);

    things.items[0].w = 1;

    x = yar_insert(&things, 1, 1);
    assert(things.count == 2);
    assert(x == things.items + 1);
    assert(x->w == 0);

    things.items[1].w = 2;

    // Insert at beginning
    x = yar_insert(&things, 0, 1);
    assert(things.count == 3);
    assert(x == things.items);
    assert(x->w == 0);
    x->w = 10;
    assert(things.items[0].w == 10);
    assert(things.items[1].w == 1);
    assert(things.items[2].w == 2);

    // Insert in between
    x = yar_insert(&things, 1, 1);
    x->w = 20;
    assert(things.count == 4);
    assert(x == things.items + 1);
    assert(things.items[0].w == 10);
    assert(things.items[1].w == 20);
    assert(things.items[2].w == 1);
    assert(things.items[3].w == 2);

    // Insert many
    x = yar_insert(&things, 2, 3);
    assert(things.count == 7);
    assert(x == things.items + 2);
    x[0].w = 30;
    x[1].w = 31;
    x[2].w = 32;
    assert(things.items[0].w == 10);
    assert(things.items[1].w == 20);
    assert(things.items[2].w == 30);
    assert(things.items[3].w == 31);
    assert(things.items[4].w == 32);
    assert(things.items[5].w == 1);
    assert(things.items[6].w == 2);

    // Insert at end
    x = yar_insert(&things, 7, 3);
    assert(things.count == 10);
    assert(x == things.items + 7);
    x[0].w = 40;
    x[1].w = 41;
    x[2].w = 42;
    assert(things.items[0].w == 10);
    assert(things.items[1].w == 20);
    assert(things.items[2].w == 30);
    assert(things.items[3].w == 31);
    assert(things.items[4].w == 32);
    assert(things.items[5].w == 1);
    assert(things.items[6].w == 2);
    assert(things.items[7].w == 40);
    assert(things.items[8].w == 41);
    assert(things.items[9].w == 42);

    yar_free(&things);
}
