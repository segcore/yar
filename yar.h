/*
 * yar - dynamic arrays in C.
 *
 * Copyright (c) 2025 Nicholas Rixson
 * Released under the MIT Licence. https://github.com/segcore/yar
 */
#ifndef YAR_H
#define YAR_H

#include <stddef.h> // size_t
#include <string.h> // strlen


#define yar(type)   struct { type *items; size_t count; size_t capacity; }
#define yar_append(array)   ((_yar_append((void**)&(array)->items, &(array)->count, &(array)->capacity, sizeof((array)->items[0])), \
                              &(array)->items[(array)->count - 1]))
#define yar_reserve(array, extra)       ((_yar_reserve((void**)&(array)->items, &(array)->count, &(array)->capacity, sizeof((array)->items[0]), (extra)), \
                                          &(array)->items[(array)->count]))
#define yar_append_many(array, data, num)   ((_yar_append_many((void**)&(array)->items, &(array)->count, &(array)->capacity, sizeof((array)->items[0]), 1 ? (data) : ((array)->items), (num)) ))
#define yar_append_cstr(array, data)        yar_append_many(array, data, strlen(data))
#define yar_reset(array)    (((array)->count = 0))
#define yar_free(array)     ((_yar_free((array)->items)), (array)->items = NULL, (array)->count = 0, (array)->capacity = 0)

// Implementation functions
void* _yar_append(void** items_pointer, size_t* count, size_t* capacity, size_t item_size);
void* _yar_append_many(void** items_pointer, size_t* count, size_t* capacity, size_t item_size, void* data, size_t extra);
void* _yar_reserve(void** items_pointer, size_t* count, size_t* capacity, size_t item_size, size_t extra);
void* _yar_realloc(void* p, size_t new_size);
void _yar_free(void* p);

/*
 * yar(type)
 * - Declare a new basic dynamic array
 *
 * yar_append(array)
 * - Add a new item at the end of the array, and return a pointer to it
 *
 * yar_reserve(array, extra)
 * - Reserve space for `extra` count of items
 *
 * yar_append_many(array, data, num)
 * - Append a copy of existing data
 *
 * yar_append_cstr(array, data)
 * - Append a C string (nul-terminated char array)
 *
 * yar_reset(array)
 * - Reset the count of elements to 0, to re-use the memory. Does not free the memory.
 *
 * yar_free(array)
 * - Free item memory, and set the items, count, and capacity to 0.
 */

#if defined(YAR_IMPLEMENTATION)

#ifndef YAR_MIN_CAP
  #define YAR_MIN_CAP 16
#endif

#ifndef YAR_REALLOC
  #define YAR_REALLOC realloc
#endif

#ifndef YAR_FREE
  #define YAR_FREE free
#endif

#include <string.h>
void* _yar_append(void** items_pointer, size_t* count, size_t* capacity, size_t item_size)
{
    void* result = _yar_reserve(items_pointer, count, capacity, item_size, 1);
    if (result != NULL) *count += 1;
    return result;
}

void* _yar_append_many(void** items_pointer, size_t* count, size_t* capacity, size_t item_size, void* data, size_t extra)
{
    void* result = _yar_reserve(items_pointer, count, capacity, item_size, extra);
    if (result != NULL) {
        memcpy(result, data, item_size * extra);
        *count += extra;
    }
    return result;
}

void* _yar_reserve(void** items_pointer, size_t* count, size_t* capacity, size_t item_size, size_t extra)
{
    void* items = *items_pointer;
    size_t newcount = *count + extra;
    if(newcount > *capacity) {
        size_t newcap = (*capacity < YAR_MIN_CAP) ? YAR_MIN_CAP : *capacity * 16 / 10;
        if (newcap < newcount) newcap = newcount;
        void* next = _yar_realloc(items, newcap * item_size);
        if (next == NULL) return NULL;
        items = next;
        *items_pointer = next;
        *capacity = newcap;
    }
    void* result = (char*)items + (*count * item_size);
    memset(result, 0, item_size * extra);
    return result;
}


void* _yar_realloc(void* p, size_t new_size)
{
    // Declaration, so we can call it if the definition is overridden
    extern void* YAR_REALLOC(void *ptr, size_t size);
    return YAR_REALLOC(p, new_size);
}

void _yar_free(void* p)
{
    extern void YAR_FREE(void *ptr);
    YAR_FREE(p);
}

#endif // YAR_IMPLEMENTATION
#endif // YAR_H
