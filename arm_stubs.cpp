// ARM stub implementations for x86 assembly functions
// These are needed when building on ARM64 where x86 assembly is not available

#include "xmrstak/backend/cpu/crypto/cryptonight.h"

extern "C" {
// Stub implementations - these do nothing but prevent link errors
void cryptonight_v8_mainloop_ivybridge_asm(cryptonight_ctx* ctx0)
{
    // Stub implementation - does nothing
}

void cryptonight_v8_mainloop_ryzen_asm(cryptonight_ctx* ctx0)
{
    // Stub implementation - does nothing
}

void cryptonight_v8_double_mainloop_sandybridge_asm(cryptonight_ctx* ctx0, cryptonight_ctx* ctx1)
{
    // Stub implementation - does nothing
}
}
