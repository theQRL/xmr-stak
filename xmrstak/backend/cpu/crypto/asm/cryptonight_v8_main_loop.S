// GAS Intel syntax for cryptonight_v8_main_loop.S
.intel_syntax noprefix

.text

#ifdef __APPLE__
#define GLOBAL_SYMBOL_NAME(name) _##name
#else
#define GLOBAL_SYMBOL_NAME(name) name
#endif

.global GLOBAL_SYMBOL_NAME(cryptonight_v8_mainloop_ivybridge_asm)
.global GLOBAL_SYMBOL_NAME(cryptonight_v8_mainloop_ryzen_asm)
.global GLOBAL_SYMBOL_NAME(cryptonight_v8_double_mainloop_sandybridge_asm)

.align 64
GLOBAL_SYMBOL_NAME(cryptonight_v8_mainloop_ivybridge_asm):
    sub rsp, 48
    mov rcx, rdi
    .include "cryptonight_v8_main_loop_ivybridge_linux.inc"
    add rsp, 48
    ret

.align 64
GLOBAL_SYMBOL_NAME(cryptonight_v8_mainloop_ryzen_asm):
    sub rsp, 48
    mov rcx, rdi
    .include "cryptonight_v8_main_loop_ryzen_linux.inc"
    add rsp, 48
    ret

.align 64
GLOBAL_SYMBOL_NAME(cryptonight_v8_double_mainloop_sandybridge_asm):
    sub rsp, 48
    mov rcx, rdi
    mov rdx, rsi
    .include "cryptonight_v8_double_main_loop_sandybridge_linux.inc"
    add rsp, 48
    ret
