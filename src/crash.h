#ifndef _CRASH_H_
#define _CRASH_H_

#include <types.h>

#define MIPS_KSEG0       0x80000000
#define MIPS_KSEG1       0xA0000000
#define VI_DRAM_ADDR_REG 0x04400004

#define N64_ASSERT(exp) (exp) ? ((void*)0) : _n64_assert(__FILE__, __LINE__, #exp, 1);

extern int is_recompiler(void);
extern u32 cop0_get_cause(void);
extern u32 cop0_get_epc(void);
extern u32 cop0_get_badvaddr(void);
extern u32 cop0_get_count(void);
extern void _n64_assert(const char* pFile, int nLine, const char *pExpression, int nStopProgram);
extern u8 __crash_handler_entry[];

#define COLOR_RGBA5551(r, g, b, a) \
(                             \
    ((r & 0x1F) << 11) |      \
    ((g & 0x1F) <<  6) |      \
    ((b & 0x1F) <<  1) |      \
    (a & 1)                   \
)

#define vi_get_fb_vaddr() \
    ((void *) (MIPS_KSEG1 | *(u32 *)(MIPS_KSEG1 | VI_DRAM_ADDR_REG)))

// note: do not use with tlb vaddrs
#define vi_set_fb_vaddr(addr) \
    *(u32 *)(MIPS_KSEG1 | VI_DRAM_ADDR_REG) = (((u32) addr) & 0x03FFFFFF)

#define FB_DEFAULT_ADDR 0xA0000400 // 0xA038F800 nusys default
#define CRASH_SCREEN_W 320
#define CRASH_SCREEN_H 240

#define fb_set_fill_color(r, g, b) fbFillColor = COLOR_RGBA5551(r, g, b, 1)
#define fb_set_shade_color(r, g, b) fbShadeColor = COLOR_RGBA5551(r, g, b, 1)
#define fb_set_buffer(buffer) fbAddress = (void *) (buffer)

#define ctoidx(c) (c-0x20)

void generate_exception_preambles(void *entryPoint);
void show_crash_screen_and_hang(void);

void fb_invalidate(void);
void fb_swap(void);
void fb_fill_rect(int baseX, int baseY, int width, int height);
void fb_draw_char(int x, int y, u8 idx);
int fb_print_str(int x, int y, const char *str);
int fb_print_uint(int x, int y, u32 value);
void fb_print_int_hex(int x, int y, u32 value, int nbits);
void fb_print_gpr_states(int x, int y, const char* regStrs[], u32 *regContext);

#endif /* _CRASH_H_ */