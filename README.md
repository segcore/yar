# yar

Typesafe, single-implementation, zero-copy dynamic arrays in C without crazy
macros.

Suitable as a single-header library, just copy [yar.h](yar.h) into your project and
use it. `#define YAR_IMPLEMENTATION` in one C file of the project before
including the header.

## Single-implementation

Each function has one implementation which *all types* delegate to. Using
`yar(double)`, `yar(int)`, and `yar(SomeStruct)` and custom types all delegate
to the same small function implementation. This reduces code bloat, and can
help keep code in CPU cache longer. If all dynamic array operations call the
same function regardless of type, only that entry need be in cache. This
constrasts with a per-type specialisation (e.g. C macros, C separate functions
per type, C++ std::vector, and many other language generic/template
implementations), where each instantiation is a 'copy/paste' with different
types. `std::vector<double>::push_back()` will kick
`std::vector<int>::push_back()` out of cache with a near-identical copy of the
same code. A traditional C macro method is similar, but duplicates at each call
site.

Single implementation also allows a more specialised allocation function, and
avoids having different-allocation/free implementation issues across C files
(freeing with a different allocation scheme in another file, or in particular
in Windows DLLs where the DLL allocating the memory must be the same one to
free it).

## Usage

`yar_append` adds a new element to the end of the dynamic array, and returns it
to you as a pointer of the correct type. `yar_free` to free the memory.

You can read all functions defined in [yar.h](yar.h) - it is simple and small.
See the [examples directory](examples) for more usage and motivation.

### User-define struct

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

### yar(type) macro struct

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

### Works as you would expect

It can be used inside other structs, have dynamic arrays of structures, the
sub-types can contain spaces...

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

### Doesn't leak!

You can define structs in header files which are yar-compatible without those
headers needing to include yar.h. It doesn't "leak" its implementation into the
header, or require downstream applications to know what `yar.h` is.

```c
// things.h
typedef struct { ... } Thing;
typedef struct {
    Thing* items;
    size_t count;
    size_t capacity;
} ThingList;
void x(ThingList* things);

// things.c
#include "things.h"
#include "yar.h"
void x(ThingList* things) {
    *yar_append(things) = ...;
}
```

## Build and Test

Usually you won't need to build this library, you just copy and paste
[yar.h](yar.h) into your project and use it. However, there is a build system
available to build the library, examples, and tests, using CMake:

```sh
mkdir build
cd build
cmake ..
cmake --build .
ctest
```

The CMake also exports two libraries, in case you prefer to copy the whole repo
in or submodule it:
- `yar` - Interface library which simply has the include path setup.
- `yar_impl` - Object library which contains the implementation, and adds an
  include path.

## Notes and Troubleshooting

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

### yar(x) != yar(x)

I am yar(x). You are yar(x). We are not the same.

yar(type) is defined as `#define yar(type)   struct { type *items; size_t
count; size_t capacity; }`. If you know C, you know this defines a new
structure every time you 'run' the macro.

Just as if you had `struct MyInt { int a; }` and `struct OtherInt { int a; }`
-- these are different types, even though they are equivalent after
compilation.

This means you cannot use yar(x) inside function arguments, and some other
situations where you need the same type as per C rules.

```c
// --- BAD. Declares a new type in the function declaration, 
//     which is incompatible with all callers
void do_something(yar(int) list) {}
    // warning: anonymous struct declared inside parameter list will not be
    // visible outside of this definition or declaration

do_something(...);
    // error: incompatible type for argument 1 of ‘do_something’

// --- GOOD: Define a type and use that in relevant functions
typedef yar(int) Algorithms;
void do_something(Algorithms list) {}

Algorithms x = {0};
do_something(x);
```

This can be used to your advantage to provide stronger type-checking:

```c
typedef yar(int) Algorithms;
typedef yar(int) ListIndexes;

void check_algorithms(Algorithms algs);
int main() {
    Algorithms algs = {0};
    ListIndexes indexes = {0};
    // Whoops: wrong argument! But gets caught at compile-time
    check_algorithms(indexes);
    // error: incompatible type for argument 1 of ‘check_algorithms’
    // check_algorithms(indexes);
    //                  ^~~~~~~
    //                  |
    //                  ListIndexes
    // note: expected ‘Algorithms’ but argument is of type ‘ListIndexes’
}
```

### Avoiding YAR_IMPLEMENTATION

Don't like YAR_IMPLEMENTATION? Instead of
```c
#define YAR_IMPLEMENTATION
#include "yar.h"
```

You can instead just do:
```c
#include "yar.c"
```

Alternatively, you can build with yar.c from this repo.

## Inspiration

Thanks to Tsoding's [nob.h](https://github.com/tsoding/nob.h)'s `da_append`
macro, and to Jai's `array_add :: (array: *[..] $T) -> *T` function for the
inspiration.
