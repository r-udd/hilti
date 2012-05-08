///
/// Memory management.
///

#ifndef HILTI_MEMORY_H
#define HILTI_MEMORY_H

#include "types.h"

#include <stdint.h>
#include <stdlib.h>

#ifdef DEBUG
extern const char* __hlt_make_location(const char* file, int line);
#else
#define __hlt_make_location(file, line) 0
#endif

/// Common header of all managed memory objects.
///
/// If you change something here, also adapt ``hlt.gcdhr`` in ``libhilti.ll``.
typedef struct {
    uint64_t ref_cnt;  /// The number of references to the object currently retained.
} __hlt_gchdr;

/// Allocates an unmanaged memory chunk of the given size. This operates
/// pretty much like malloc but it will always return a valid address. If it
/// can't allocate sufficient memory, the function will terminate the current
/// process directly and not return. The allocated memory must be freed with
/// ``hlt_free``.
///
/// size: The number of bytes to allocate.
///
/// type: A string describing the type of object being allocated. This is for
/// debug purposes only.
///
/// location: A string describing the location where the object is allocaged.
/// This is for debugging purposes only.
///
/// Returns: A pointer to the new memory chunk. This will never be null.
///
/// \note This shouldn't be called directly by user code. Use the macro \c
/// hlt_malloc instead.
extern void* __hlt_malloc(uint64_t size, const char* type, const char* location);

/// Allocates a number of unmanaged memory elements of a given size. This
/// operates pretty much like calloc but it will always return a valid
/// address. If it can't allocate sufficient memory, the function will
/// terminate the current process directly and not return. The allocated
/// memory must be freed with ``hlt_free``.
///
/// count: The number of elements to allocate.
///
/// size: The size of one element being allocated.
///
/// type: A string describing the type of object being allocated. This is for
/// debug purposes only.
///
/// location: A string describing the location where the object is allocaged.
/// This is for debugging purposes only.
///
/// Returns: A pointer to the new memory chunk. This will never be null.
///
/// \note This shouldn't be called directly by user code. Use the macro \c
/// hlt_calloc instead.
extern void* __hlt_calloc(uint64_t count, uint64_t size, const char* type, const char* location);

/// Reallocates a number of unmanaged memory elements of a given size. This
/// operates pretty much like realloc but it will always return a valid
/// address. If it can't allocate sufficient memory, the function will
/// terminate the current process directly and not return. The reallocated
/// memory must be freed with ``hlt_free``.
///
/// ptr: The pointer to reallocate. This must have been allocated with
/// __hlt_malloc().
///
/// size: The size of bytes to reallocate.
///
/// type: A string describing the type of object being allocated. This is for
/// debug purposes only.
///
/// location: A string describing the location where the object is allocaged.
/// This is for debugging purposes only.
///
/// Returns: A pointer to the new memory chunk. This will never be null.
///
/// \note This shouldn't be called directly by user code. Use the macro \c
/// hlt_calloc instead.
extern void* __hlt_realloc(void *ptr, uint64_t size, const char* type, const char* location);

/// Frees an unmanaged memory chunk formerly allocated with ``hlt_malloc``.
/// This operates pretty much like the standard ``free``.
///
/// ptr: The memory chunk to free. Can be null, in which case the function
/// will not do anything.
///
/// type: A string describing the type of object being allocated. This is for
/// debug purposes only. This string should match what is passed to the
/// corresponding \c __hlt_malloc/hlt_calloc call.
///
/// location: A string describing the location where the object is allocaged.
/// This is for debugging purposes only.
extern void __hlt_free(void *ptr, const char* type, const char* location);

#define hlt_malloc(size)        __hlt_malloc(size, "-", __hlt_make_location(__FILE__,__LINE__))
#define hlt_calloc(count, size) __hlt_calloc(count, size, "-", __hlt_make_location(__FILE__,__LINE__))
#define hlt_realloc(ptr, size)  __hlt_realloc(ptr, size, "-", __hlt_make_location(__FILE__,__LINE__))
#define hlt_free(ptr)           __hlt_free(ptr, "-", __hlt_make_location(__FILE__,__LINE__))

// Internal function to increaes a memory managed object's reference count by
// one. Not to be used directly from user code.
extern void __hlt_object_ref(const hlt_type_info* ti, void* obj);

// Internal function to decrease a memory managed object's reference count by
// one. Not to be used directly from user code.
extern void __hlt_object_unref(const hlt_type_info* ti, void* obj);

/// XXX
extern void __hlt_object_dtor(const hlt_type_info* ti, void* obj, const char* location);

/// XXX
extern void __hlt_object_cctor(const hlt_type_info* ti, void* obj, const char* location);

/// XXX
extern void* __hlt_object_new(const hlt_type_info* ti, uint64_t size, const char* location);

/// XXX
#define GC_ASSIGN(obj, val, tag) \
   { \
   tag* tmp = val; \
   __hlt_object_cctor(hlt_type_info_##tag, (void*)&val, __hlt_make_location(__FILE__,__LINE__)); \
   __hlt_object_dtor(hlt_type_info_##tag, (void*)&obj, __hlt_make_location(__FILE__,__LINE__)); \
   obj = tmp; \
   }

/// XXX
#define GC_ASSIGN_REFED(obj, val, tag) \
   { \
   tag* tmp = val; \
   __hlt_object_dtor(hlt_type_info_##tag, (void*)&obj, __hlt_make_location(__FILE__,__LINE__)); \
   obj = tmp; \
   }

/// XXX
#define GC_INIT(obj, val, tag) \
   { \
   obj = val; \
   __hlt_object_cctor(hlt_type_info_##tag, &obj, __hlt_make_location(__FILE__,__LINE__)); \
   }

/// XXX
#define GC_NEW(tag) \
   __hlt_object_new(hlt_type_info_##tag, sizeof(tag), __hlt_make_location(__FILE__,__LINE__));

/// XXX
#define GC_NEW_CUSTOM_SIZE(tag, size) \
   __hlt_object_new(hlt_type_info_##tag, size, __hlt_make_location(__FILE__,__LINE__));

/// XXX
#define GC_DTOR(obj, tag) \
   { \
       __hlt_object_dtor(hlt_type_info_##tag, &obj, __hlt_make_location(__FILE__,__LINE__)); \
   }

/// XXX
#define GC_DTOR_GENERIC(objptr, ti) \
   { \
       __hlt_object_dtor(ti, objptr, __hlt_make_location(__FILE__,__LINE__)); \
   }

/// XXX
#define GC_CCTOR(obj, tag) \
   { \
       __hlt_object_cctor(hlt_type_info_##tag, &obj, __hlt_make_location(__FILE__,__LINE__)); \
   }

/// XXX
#define GC_CCTOR_GENERIC(objptr, ti) \
   { \
       __hlt_object_cctor(ti, objptr, __hlt_make_location(__FILE__,__LINE__)); \
   }

/// XXX
#define GC_CLEAR(obj, tag) \
   { \
       __hlt_object_dtor(hlt_type_info_##tag, &obj, __hlt_make_location(__FILE__,__LINE__)); \
       obj = 0; \
   }

#endif
