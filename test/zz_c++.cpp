#undef NDEBUG // Force-enable asserts
#include <assert.h>
#include "yar.h"

// This is mostly a smoke-test to see if the header can be included in C++

int main()
{
    yar(int) ints = {};
    *yar_append(&ints) = 10;
    assert(ints.count == 1);
    assert(ints.items[0] == 10);
    yar_free(&ints);
}
