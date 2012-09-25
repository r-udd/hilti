// $Id$
//
// Replacement functions for malloc/calloc/realloc that provide automatic
// garbage collection.

#ifndef HILTI_MEMORY_H
#define HILTI_MEMORY_H

#include <stdint.h>

#define hlt_gc_malloc_atomic(n)         __hlt_gc_malloc_atomic(n, __FILE__, __LINE__)
#define hlt_gc_malloc_non_atomic(n)     __hlt_gc_malloc_non_atomic(n, __FILE__, __LINE__)
#define hlt_gc_calloc_atomic(c, n)      __hlt_gc_calloc_atomic(c, n, __FILE__, __LINE__)
#define hlt_gc_calloc_non_atomic(c, n)  __hlt_gc_calloc_non_atomic(c, n, __FILE__, __LINE__)
#define hlt_gc_realloc_atomic(p, n)     __hlt_gc_realloc_atomic(p, n, __FILE__, __LINE__)
#define hlt_gc_realloc_non_atomic(p, n) __hlt_gc_realloc_non_atomic(p, n, __FILE__, __LINE__)

// Allocates n bytes of memory and promises that they will never contain any
// pointers.
extern void* __hlt_gc_malloc_atomic(uint64_t n, const char* file, uint32_t line);

// Allocates n bytes of memory, which may be used to store pointers to other
// objects.
//
// Todo: At some point, we'll likely change this interface to require a
// pointer map to be passed in.
extern void* __hlt_gc_malloc_non_atomic(uint64_t n, const char* file, uint32_t line);

// Allocates n bytes of zero-initialized memory and promises that they will
// never contain any pointers.
extern void* __hlt_gc_calloc_atomic(uint64_t count, uint64_t n, const char* file, uint32_t line);

// Allocates n bytes of zero-initialized memory, which may be used to store
// pointers to other objects.
//
// Todo: At some point, we'll likely change this interface to require a
// pointer map to be passed in.
extern void* __hlt_gc_calloc_non_atomic(uint64_t count, uint64_t n, const char* file, uint32_t line);

// Reallocates the memory to a chunk of size n and promises that they never
// contain any pointers. The original memory must have been allocated with a
// *_atomic function as well.
extern void* __hlt_gc_realloc_atomic(void* ptr, uint64_t n, const char* file, uint32_t line);

// Reallocates the memory to a chunk of size n, which may be used to store
// pointers to other objects. The original memory must have been allocated
// with a *_non_atomic function as well.
extern void* __hlt_gc_realloc_non_atomic(void* ptr, uint64_t n, const char* file, uint32_t line);

typedef void hlt_gc_finalizer_func(void *obj);

/// Registers a finalyzer function to be called when objects gets collected.
/// Use only if absolutely necesssary!
extern void hlt_gc_register_finalizer(void* obj, hlt_gc_finalizer_func* func);

// Initializes the GC subsystem. This is called from hilti_init() and also
// from the initializer code generated by hiltic for globals.
void __hlt_gc_init();

// Forces a full garbage collection immediately. Mainly for debugging.
void __hlt_gc_force();

// Clears the given block of memory. This is essentially a bzero() with a
// fixed uint64_t argument for the size.
void hlt_memory_clear(void* ptr, uint64_t size);

#endif