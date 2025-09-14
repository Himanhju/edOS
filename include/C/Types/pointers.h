#ifndef POINTER_TYPES
#define POINTER_TYPES

#include <Types/Common.h>

#ifdef __i386__

typedef dword   intptr_t;

#elif __x86_64__

typedef qword   intptr_t;

#endif /* __i386__ */

#endif