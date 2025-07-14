#undef NDEBUG // Force-enable asserts
#include <assert.h>
#include <stdio.h>

int main()
{
    printf("This test should ABORT. If it does not, none of the tests are valid.\n");
    fflush(stdout);
    assert(0 && "this should abort");
}
