#ifndef INET_H
#define INET_H

#include <Types/Common.h>

#include <Coms/IO/IO.h>
#include <Coms/PCI/PCI.h>

typedef enum Inet_ID {
    E1000,
    NO_NIC = 0x7fffffff
}Inet_ID;

#define INTEL_82544GC_ID_CP 0x8086100C
#define INTEL_82544GC_ID_LM 0x8086100D
#define INTEL_82540EM_ID    0x8086100E
#define INTEL_82541EI_ID1   0x80861013
#define INTEL_82541ER_ID1   0x80861014
#define INTEL_82541EI_ID2   0x80861018
#define INTEL_82547GI_ID    0x80861075
#define INTEL_82541GI_ID1   0x80861076
#define INTEL_82541GI_ID2   0x80861077
#define INTEL_82541ER_ID2   0x80861078
#define INTEL_82541PI_ID    0x8086107C



enum Inet_ID Inet_Init(void);

#endif 