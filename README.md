# yar

Typesafe, single-implementation, zero-copy dynamic arrays in C without crazy
macros.

Suitable as a single-header library, just copy [yar.h](yar.h) into your project and
use it. `#define YAR_IMPLEMENTATION` in one C file of the project before
including the header.

## Single-implementation

Each function has one implementation which *all types* delegate to. Using
`yar(double)`, `yar(int)`, and `yar(SomeStruct)` and custom types all delegate
to the same small function implementation. This reduces code bloat and reduces
the amount of code that needs to be loaded and kept in CPU cache. If all
dynamic array operations call the same function regardless of type, only that
entry need be in cache.
This constrasts with a per-type specialisation (e.g. C macros, C separate functions
per type, C++ std::vector, and many other language generic/template
implementations), where each instantiation is a 'copy/paste' with different
types. `std::vector<double>::push_back()` will share nothing with
`std::vector<int>::push_back()` and repeat a near-identical copy of the
same code. A traditional C macro method is similar, but duplicates at each call
site.

Single implementation also allows a more specialised allocation function, and
avoids having different-allocation/free implementation issues across C files
(freeing with a different allocation scheme in another file, or in particular
in Windows DLLs where the DLL allocating the memory must be the same one to
free it).

## Usage

Define a structure containing at least the 3 values:
```c
struct { your_type *items; size_t count; size_t capacity; }
```

Then all yar_* functions can use it as a dynamic array.

```c
#define YAR_IMPLEMENTATION
#include "yar.h"

int main() {
    // struct { double *items; size_t count; size_t capacity; } numbers = {0};
    yar(double) numbers = {0};
    *yar_append(&numbers) = 3.14159;
    *yar_append(&numbers) = 2.71828;
    *yar_append(&numbers) = 1.61803;

    for(size_t i = 0; i < numbers.count; i++) {
        printf("%f\n", numbers.items[i]);
    }

    yar_free(&numbers);
}

// This uses a simple optional macro defined in yar.h as follows.
#define yar(type)   struct { type *items; size_t count; size_t capacity; }
```

Function overview:

* `   yar(type)` - Optional: a quick way to create a struct just used as a dynamic array.
* `T* yar_append(array)` - Append an element, and return it as a pointer of the correct type.
* `T* yar_reserve(array, extra_space)` - Reserve extra_space new elements, returning a pointer to the beginning of that space.
* `T* yar_append_many(array, data, num)` - Append a copy of existing array elements.
* `T* yar_append_cstr(array, data)` - Append a C string (nul-terminated char array)
* `T* yar_insert(array, index, num)` - Insert items somewhere within the array.
  Moves items to higher indexes as required. Returns &array[index] for you to populate with values.
* `T* yar_remove(array, index, num)` - Remove items from somewhere within the array.
* `   yar_reset(array)` - Reset the count of elements to 0, to re-use the memory. Does not free the memory.
* `   yar_free(array)` - Free items memory, and set the items, count, and capacity to 0.

For more details read [yar.h](yar.h) - it is simple and small.
See also the [examples](examples).

### Zero Initialise

When creating dynamic arrays, it is often convenient to zero-initialize them at
the definition site. This allows them to be used straight away with no other
initialisation function necessary.

```c
yar(int) x; // Bad
yar(int) y = {0}; // Good

yar(int) *z = malloc(sizeof(*z));
memset(z, 0, sizeof(*z)); // OK, just inconvenient
yar_init(z); // Alternative, just for items/count/capacity zero-ing.
```

By default, new values of the array will be memset to 0. So `yar_append(&x)` is
sufficient to append a sentinal nil-value. Though you may get compiler warnings
for not using the return value on some compilers.

### User-define struct

Yar can use user-defined structures. They just need `items`, `count`, and `capacity` fields.

```c
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
    // ...
}
```

### Type checked

Incompatible types can be caught in many cases.

```c
yar(double) numbers = {0};

// ERROR: incompatible types when assigning to type ‘double’ from type ‘char *’
*yar_append(&numbers) = "hello";
```

### Can be used almost anywhere

It can be used inside other structs, have dynamic arrays of structures, the
sub-types can contain spaces...

```c
typedef struct {
    const char* name;
    int age;
    yar(const char*) aliases;
} Person;

yar(Person) people;
```

### Doesn't leak the abstraction

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

The CMake definition also exports two libraries, if you want to use it through
CMake directly.
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
structure every time you expand the macro.

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

## Licence

Available under Public Domain and MIT Licence at your choosing.
