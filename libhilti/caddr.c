//
// Support functions HILTI's caddr data type.
//

#include <assert.h>

#include "string_.h"

hlt_string hlt_caddr_to_string(const hlt_type_info* type, const void* obj, int32_t options, __hlt_pointer_stack* seen, hlt_exception** exception, hlt_execution_context* ctx)
{
    assert(type->type == HLT_TYPE_CADDR);

    char buffer[32];
    snprintf(buffer, 32, "addr:%p", *((void**)obj));
    buffer[31] = '\0';

    return hlt_string_from_asciiz(buffer, exception, ctx);
}



