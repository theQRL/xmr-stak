#include <cstring>

typedef void (*void_func)();

#include "cryptonight.h"
#include "cryptonight_aesni.h"
#include "xmrstak/backend/cpu/crypto/asm/cnR/CryptonightR_template.h"
#include "xmrstak/misc/console.hpp"

static inline void add_code(uint8_t*& p, void (*p1)(), void (*p2)())
{
	const ptrdiff_t size = reinterpret_cast<const uint8_t*>(p2) - reinterpret_cast<const uint8_t*>(p1);
	if(size > 0)
	{
		memcpy(p, reinterpret_cast<void*>(p1), size);
		p += size;
	}
}

static inline void add_random_math(uint8_t*& p, const V4_Instruction* code, int code_size, const void_func* instructions, const void_func* instructions_mov, bool is_64_bit, int selected_asm)
{
	uint32_t prev_rot_src = (uint32_t)(-1);

	for(int i = 0;; ++i)
	{
		const V4_Instruction inst = code[i];
		if(inst.opcode == RET)
		{
			break;
		}

		uint8_t opcode = (inst.opcode == MUL) ? inst.opcode : (inst.opcode + 2);
		uint8_t dst_index = inst.dst_index;
		uint8_t src_index = inst.src_index;

		const uint32_t a = inst.dst_index;
		const uint32_t b = inst.src_index;
		const uint8_t c = opcode | (dst_index << V4_OPCODE_BITS) | (((src_index == 8) ? dst_index : src_index) << (V4_OPCODE_BITS + V4_DST_INDEX_BITS));

		switch(inst.opcode)
		{
		case ROR:
		case ROL:
			if(b != prev_rot_src)
			{
				prev_rot_src = b;
				add_code(p, instructions_mov[c], instructions_mov[c + 1]);
			}
			break;
		}

		if(a == prev_rot_src)
		{
			prev_rot_src = (uint32_t)(-1);
		}

		void_func begin = instructions[c];

		// AMD == 2
		if((selected_asm == 2) && (inst.opcode == MUL && !is_64_bit))
		{
			// AMD Bulldozer has latency 4 for 32-bit IMUL and 6 for 64-bit IMUL
			// Always use 32-bit IMUL for AMD Bulldozer in 32-bit mode - skip prefix 0x48 and change 0x49 to 0x41
			uint8_t* prefix = reinterpret_cast<uint8_t*>(begin);

			if(*prefix == 0x49)
			{
				*(p++) = 0x41;
			}

			begin = reinterpret_cast<void_func>(prefix + 1);
		}

		add_code(p, begin, instructions[c + 1]);

		if(inst.opcode == ADD)
		{
			*(uint32_t*)(p - sizeof(uint32_t) - (is_64_bit ? 3 : 0)) = inst.C;
			if(is_64_bit)
			{
				prev_rot_src = (uint32_t)(-1);
			}
		}
	}
}

#ifndef DISABLE_CRYPTONIGHTR
void v4_compile_code(unsigned long N, cryptonight_ctx* ctx, int code_size)
{
    printer::inst()->print_msg(LDEBUG, "CryptonightR update ASM code");
    const int allocation_size = 65536;

    if(ctx->fun_data == nullptr)
        ctx->fun_data = static_cast<uint8_t*>(allocateExecutableMemory(allocation_size));
    else
        unprotectExecutableMemory(ctx->fun_data, allocation_size);

    uint8_t* p0 = ctx->fun_data;
    uint8_t* p = p0;
    int code_size_new;

    if(ctx->fun_data != nullptr)
    {
        add_code(p, CryptonightR_template_part1, CryptonightR_template_part2);
        add_random_math(p, ctx->cn_r_ctx.code, code_size, instructions, instructions_mov, false, ctx->asm_version);
        add_code(p, CryptonightR_template_part2, CryptonightR_template_part3);
        *(int*)(p - 4) = static_cast<int>((((const uint8_t*)CryptonightR_template_mainloop) - ((const uint8_t*)CryptonightR_template_part1)) - (p - p0));
        add_code(p, CryptonightR_template_part3, CryptonightR_template_end);

        ctx->loop_fn = reinterpret_cast<cn_mainloop_fun>(ctx->fun_data);
        protectExecutableMemory(ctx->fun_data, allocation_size);
        flushInstructionCache(ctx->fun_data, p - p0);
    }
    else
    {
        printer::inst()->print_msg(L0, "Error: CPU CryptonightR update ASM code ctx->fun_data is a nullptr");
        ctx->loop_fn = nullptr;
    }
}

void v4_compile_code_double(unsigned long N, cryptonight_ctx* ctx0, cryptonight_ctx* ctx1, int code_size)
{
    printer::inst()->print_msg(LDEBUG, "CryptonightR update ASM code for double");
    const int allocation_size = 65536;

    if(ctx0->fun_data == nullptr)
        ctx0->fun_data = static_cast<uint8_t*>(allocateExecutableMemory(allocation_size));
    else
        unprotectExecutableMemory(ctx0->fun_data, allocation_size);

    uint8_t* p0 = ctx0->fun_data;
    uint8_t* p = p0;

    if(ctx0->fun_data != nullptr)
    {
        add_code(p, CryptonightR_template_double_part1, CryptonightR_template_double_part2);
        add_random_math(p, ctx0->cn_r_ctx.code, code_size, instructions, instructions_mov, false, ctx0->asm_version);
        add_code(p, CryptonightR_template_double_part2, CryptonightR_template_double_part3);
        add_random_math(p, ctx1->cn_r_ctx.code, code_size, instructions, instructions_mov, false, ctx1->asm_version);
        add_code(p, CryptonightR_template_double_part3, CryptonightR_template_double_part4);
        *(int*)(p - 4) = static_cast<int>((((const uint8_t*)CryptonightR_template_double_mainloop) - ((const uint8_t*)CryptonightR_template_double_part1)) - (p - p0));
        add_code(p, CryptonightR_template_double_part4, CryptonightR_template_double_end);

        ctx0->loop_fn = reinterpret_cast<cn_mainloop_fun>(ctx0->fun_data);
        ctx1->loop_fn = ctx0->loop_fn;
        protectExecutableMemory(ctx0->fun_data, allocation_size);
        flushInstructionCache(ctx0->fun_data, p - p0);
    }
    else
    {
        printer::inst()->print_msg(L0, "Error: CPU CryptonightR update ASM code ctx0->fun_data is a nullptr");
        ctx0->loop_fn = nullptr;
        ctx1->loop_fn = nullptr;
    }
}
#else
// Stub implementation for ARM
void v4_compile_code(unsigned long N, cryptonight_ctx* ctx, int code_size)
{
    // On ARM we use a stub implementation since we can't generate x86 code
    if(ctx->fun_data == nullptr)
        ctx->fun_data = static_cast<uint8_t*>(allocateExecutableMemory(4096));
    else
        unprotectExecutableMemory(ctx->fun_data, 4096);

    // Set a no-op implementation for ARM
    ctx->loop_fn = [](cryptonight_ctx* /*ctx*/) { };
}

void v4_compile_code_double(unsigned long N, cryptonight_ctx* ctx0, cryptonight_ctx* ctx1, int code_size)
{
    // On ARM we use stub implementations
    v4_compile_code(N, ctx0, code_size);
    v4_compile_code(N, ctx1, code_size);
}
#endif

template<size_t N>
void v4_compile_code_multi(cryptonight_ctx** ctx, int code_size)
{
    // Handle multi-threaded compilation
    for(size_t i = 0; i < N; i++) {
        v4_compile_code(N, ctx[i], code_size); 
    }
}

// Template instantiations
template void v4_compile_code_multi<2>(cryptonight_ctx**, int);
template void v4_compile_code_multi<3>(cryptonight_ctx**, int);
template void v4_compile_code_multi<4>(cryptonight_ctx**, int);
template void v4_compile_code_multi<5>(cryptonight_ctx**, int);
