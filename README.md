# yar

Typesafe, single-implementation, zero-copy dynamic arrays in C without crazy
macros.

Suitable as a single-header library, just copy `yar.h` into your project and
use it. `#define YAR_IMPLEMENTATION` in one C file of the project before
including the header.

Single-implementation means that there is one definition of the implementation
functions, and all calls delegate to those functions. This reduces the
'copy/paste' effect of using a pure-macro approach, which would duplicate the
allocation logic at every call site. It also means that the implementation can
use a more specialised allocation function, and avoid different allocation/free
implementation issues in different C files (freeing with a different allocation
scheme in another file, or in particular on Windows DLLs where the DLL
allocating the memory must be the same one to free it).

## Usage

`yar_append` adds a new element to the end of the dynamic array, and returns it
to you as a pointer of the correct type.

See also the [examples directory](examples).

## User-define struct

Yar can use user-defined structures. They just need `items`, `count`, and `capacity` fields.

```c
#define YAR_IMPLEMENTATION
#include "yar.h"
#include <stdio.h>

typedef struct {
    float whatever_other_fields_you_have;

    // These 3 are required for yar to work
    double* items;
    size_t count;
    size_t capacity;

    float and_in_any_order;
} FavouriteNumbers;

int main()
{
    FavouriteNumbers numbers = {0};

    *yar_append(&numbers) = 3.14159;
    *yar_append(&numbers) = 2.71828;
    *yar_append(&numbers) = 1.61803;

    double* number = yar_append(&numbers);
    *number = 1234;

    // Type error: error: incompatible types when assigning to type ‘double’ from type ‘char *’
    // *yar_append(&numbers) = "hello";

    // Will print all 4 values
    for(size_t i = 0; i < numbers.count; i++) {
        printf("%f\n", numbers.items[i]);
    }
    yar_free(&numbers);
}
```

## yar(type) macro struct

`yar(type)` defines a struct with the 3 required fields. It is convenient if
you just need the dynamic array and no other fields in the struct.

```c
#define YAR_IMPLEMENTATION
#include "yar.h"
#include <stdio.h>

int main()
{
    yar(double) numbers = {0};

    *yar_append(&numbers) = 3.14159;
    *yar_append(&numbers) = 2.71828;
    *yar_append(&numbers) = 1.61803;

    for(size_t i = 0; i < numbers.count; i++) {
        printf("%f\n", numbers.items[i]);
    }
    yar_free(&numbers);
}
```

## Works as you would expect

It can be used inside other structs, have dynamic arrays of structures, ...

```c
#define YAR_IMPLEMENTATION
#include "yar.h"
#include <stdio.h>

int main()
{
    typedef struct {
        const char* name;
        int age;
        yar(const char*) aliases;
    } Person;

    yar(Person) people;

    Person* your = yar_append(&people);
    your->name = "Timothy";
    your->age = 23;
    *yar_append(&your->aliases) = "Captain Lobsterlegs";
    *yar_append(&your->aliases) = "Avocado Toast Man";
    *yar_append(&your->aliases) = "The T-Bone";

    printf("Hello %s, or should I call you:\n", your->name);
    for(size_t i = 0; i < your->aliases.count; i++) {
        printf("   %s\n", your->aliases.items[i]);
    }
    yar_free(&your->aliases);
    yar_free(&people);
}
```

## Other notes

### Avoiding YAR_IMPLEMENTATION

Instead of
```c
#define YAR_IMPLEMENTATION
#include "yar.h"
```

You can instead just do:
```c
#include "yar.c"
```

Alternatively, you can build with yar.c from this repo.

### Multiple definition errors

```
a.c:(.text+0x0): multiple definition of `_yar_append'; /tmp/cc4PALri.o:yar.c:(.text+0x0): first defined here
```

Multiple definitions of functions like _yar_append indicate that you have `YAR_IMPLEMENTATION`
in more than one file of the project. It must be only defined in one file.

Alternatively, define it in none of your files and include yar.c from this repo.

### Undefined reference to _yar_append or similar functions

```
a.c:(.text+0x38): undefined reference to `_yar_append'
```

You do not have `YAR_IMPLEMENTATION` defined in your project at all. Define it
before including yar.h in one of your C files, or alternatively build with
yar.c from this repo.

## Inspiration

Thanks to Tsoding's [nob.h](https://github.com/tsoding/nob.h)'s `da_append`
macro, and to Jai's `array_add :: (array: *[..] $T) -> *T` function for the
inspiration.
