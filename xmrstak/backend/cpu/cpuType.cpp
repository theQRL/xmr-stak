#include "xmrstak/backend/cpu/cpuType.hpp"

#include <cstdio>
#include <cstring>
#include <inttypes.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#include <intrin.h>
#elif defined(__aarch64__) || defined(__arm__)
// ARM-specific headers
#if defined(__linux__)
#include <sys/auxv.h>
#include <asm/hwcap.h>
#endif
#else
#include <cpuid.h>
#endif

namespace xmrstak
{
namespace cpu
{

#if defined(__aarch64__) || defined(__arm__)
// ARM implementation
Model getModel()
{
    Model result;
    
#if defined(__aarch64__)
    result.type_name = "ARM64";
    result.family = 8;  // AArch64
    result.model = 0;   // Generic model
    
    // AES detection for ARM
#if defined(__ARM_FEATURE_CRYPTO)
    result.aes = true;
#elif defined(__linux__)
    // On Linux we can use getauxval to detect CPU capabilities
#if defined(AT_HWCAP) && defined(HWCAP_AES)
    unsigned long hwcap = getauxval(AT_HWCAP);
    result.aes = (hwcap & HWCAP_AES) != 0;
#else
    result.aes = false;
#endif
#else
    result.aes = false;
#endif

    // NEON is always available in AArch64, roughly equivalent to SSE2
    result.sse2 = true;
    // No AVX equivalent on ARM
    result.avx = false;
    
#else // __arm__ (32-bit ARM)
    result.type_name = "ARM";
    result.family = 7;  // Generic ARMv7
    result.model = 0;   // Generic model
    
    // AES detection for ARM
#if defined(__ARM_FEATURE_CRYPTO)
    result.aes = true;
#elif defined(__linux__)
    // On Linux we can use getauxval to detect CPU capabilities
#if defined(AT_HWCAP2) && defined(HWCAP2_AES)
    unsigned long hwcap2 = getauxval(AT_HWCAP2);
    result.aes = (hwcap2 & HWCAP2_AES) != 0;
#else
    result.aes = false;
#endif
#else
    result.aes = false;
#endif

    // NEON detection for ARMv7
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
    result.sse2 = true;  // NEON is roughly equivalent to SSE2
#elif defined(__linux__)
#if defined(AT_HWCAP) && defined(HWCAP_NEON)
    unsigned long hwcap = getauxval(AT_HWCAP);
    result.sse2 = (hwcap & HWCAP_NEON) != 0;
#else
    result.sse2 = false;
#endif
#else
    result.sse2 = false;
#endif

    // No AVX equivalent on ARM
    result.avx = false;
#endif

    return result;
}

// ARM stub for get_masked function (used in autoAdjust but not meaningful on ARM)
int32_t get_masked(int32_t val, int32_t h, int32_t l)
{
    // Stub implementation for ARM - just return 0
    // This function is used for x86 CPUID parsing which doesn't apply to ARM
    return 0;
}

#else
// x86 implementation
void cpuid(uint32_t eax, int32_t ecx, int32_t val[4])
{
    std::memset(val, 0, sizeof(int32_t) * 4);

#ifdef _WIN32
    __cpuidex(val, eax, ecx);
#else
    __cpuid_count(eax, ecx, val[0], val[1], val[2], val[3]);
#endif
}

int32_t get_masked(int32_t val, int32_t h, int32_t l)
{
    val &= (0x7FFFFFFF >> (31 - (h - l))) << l;
    return val >> l;
}

bool has_feature(int32_t val, int32_t bit)
{
    int32_t mask = 1 << bit;
    return (val & mask) != 0u;
}

Model getModel()
{
    int32_t cpu_info[4];
    char cpustr[13] = {0};

    cpuid(0, 0, cpu_info);
    std::memcpy(cpustr, &cpu_info[1], 4);
    std::memcpy(cpustr + 4, &cpu_info[3], 4);
    std::memcpy(cpustr + 8, &cpu_info[2], 4);

    Model result;

    cpuid(1, 0, cpu_info);

    result.family = get_masked(cpu_info[0], 12, 8);
    result.model = get_masked(cpu_info[0], 8, 4) | get_masked(cpu_info[0], 20, 16) << 4;
    result.type_name = cpustr;

    // feature bits https://en.wikipedia.org/wiki/CPUID
    // sse2
    result.sse2 = has_feature(cpu_info[3], 26);
    // aes-ni
    result.aes = has_feature(cpu_info[2], 25);
    // avx - 27 is the check if the OS overwrote cpu features
    result.avx = has_feature(cpu_info[2], 28) && has_feature(cpu_info[2], 27);

    if(strcmp(cpustr, "AuthenticAMD") == 0)
    {
        if(result.family == 0xF)
            result.family += get_masked(cpu_info[0], 28, 20);
    }

    return result;
}
#endif

} // namespace cpu
} // namespace xmrstak
