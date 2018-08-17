#include <sm64.h>

#include "crash.h"

extern u32 exceptionRegContext[];

extern char *pAssertFile;
extern int   nAssertLine;
extern char *pAssertExpression;
extern int   nAssertStopProgram;

u16 fbFillColor = 0xFFFF;
u16 *fbAddress = NULL;

//const u8 crashFont[] = {
//    0x69, 0x99, 0x96, 0x62, 0x22, 0x27, 0x69, 0x16, 0x8F, 0x69, 0x21, 0x96,
//    0x35, 0x9F, 0x11, 0xF8, 0xE1, 0x96, 0x68, 0xE9, 0x96, 0xF1, 0x24, 0x44,
//    0x69, 0x69, 0x96, 0x69, 0x97, 0x16, 0x69, 0x9F, 0x99, 0xE9, 0xE9, 0x9E,
//    0x69, 0x88, 0x96, 0xE9, 0x99, 0x9E, 0xF8, 0xE8, 0x8F, 0xF8, 0xE8, 0x88,
//    0x69, 0x8B, 0x96, 0x99, 0x9F, 0x99, 0x72, 0x22, 0x27, 0x72, 0x22, 0xA4,
//    0x99, 0xAC, 0xA9, 0x88, 0x88, 0x8F, 0x9F, 0xDD, 0x99, 0x99, 0xDB, 0x99,
//    0x69, 0x99, 0x96, 0xE9, 0x9E, 0x88, 0x69, 0x9D, 0x61, 0xE9, 0x9E, 0x99,
//    0x69, 0x42, 0x96, 0xF2, 0x22, 0x22, 0x99, 0x99, 0x96, 0x99, 0x9A, 0xA4,
//    0x99, 0xBB, 0xF9, 0x99, 0x69, 0x99, 0x99, 0x96, 0x22, 0xF1, 0x24, 0x8F
//};

extern u8 crashFont[];

const char *szErrCodes[] = {
    "INTERRUPT",
    "TLB MOD",
    "UNMAPPED LOAD ADDR",
    "UNMAPPED STORE ADDR",
    "BAD LOAD ADDR",
    "BAD STORE ADDR",
    "BUS ERR ON INSTR FETCH",
    "BUS ERR ON LOADSTORE",
    "SYSCALL",
    "BREAKPOINT",
    "UNKNOWN INSTR",
    "COP UNUSABLE",
    "ARITHMETIC OVERFLOW",
    "TRAP EXC",
    "VIRTUAL COHERENCY INSTR",
    "FLOAT EXC"
};

const char *szGPRegisters1[] = {
    "R0", "AT", "V0", "V1", "A0", "A1", "A2", "A3",
    "T0", "T1", "T2", "T3", "T4", "T5", "T6", NULL
};

const char *szGPRegisters2[] = {
    "T7", "S0", "S1", "S2", "S3", "S4", "S5", "S6",
    "S7", "T8", "T9", /*"K0", "K1",*/
    "GP", "SP", "FP", "RA", NULL
};

/*
    Generates new preamble code at the exception vectors (0x000, 0x180)

    eg: generate_exception_preambles(crash_handler_entry, __osException);

    000: lui   k0, hi(crash_handler_entry)
    004: addiu k0, k0, lo(crash_handler_entry)
    008: jr    k0
    00C: nop
*/
void generate_exception_preambles(void *entryPoint)
{
    u8 *mem = (u8 *) 0xA0000000;
    int offs = 0;

    u16 hi = (u32)entryPoint >> 16;
    u16 lo = (u32)entryPoint & 0xFFFF;

    if(lo & 0x8000)
    {
        hi++;
    }

    for(int i = 0; i < 2; i++)
    {
        *(u32 *) &mem[offs+0x00] = 0x3C1A0000 | hi;
        *(u32 *) &mem[offs+0x04] = 0x275A0000 | lo;
        *(u32 *) &mem[offs+0x08] = 0x03400008;
        *(u32 *) &mem[offs+0x0C] = 0x00000000;
        offs += 0x180;
    }
}

void show_crash_screen_and_hang(void)
{
    fb_set_address((void*)(*(u32 *)0xA4400004 | 0x80000000)); // replace me

    u32 cause = cop0_get_cause();
    u32 epc = cop0_get_epc();

    u8 errno = (cause >> 2) & 0x1F;
    
    fbFillColor = 0x6252;
    fb_fill(10, 10, 300, 220);

    if(nAssertStopProgram == 0)
    {
        fb_print_str(80, 20, "ERROR");
        fb_print_int_hex(80, 30, errno, 8);
        fb_print_str(98, 30, szErrCodes[errno]);
    }
    else
    {
        fb_print_str(80, 20, "ASSERT");
        fb_print_int_hex(80, 30, nAssertLine, 16);
        fb_print_str(80 + 5*6, 30, pAssertFile);
        fb_print_str(80, 40, pAssertExpression);
    }

    fb_print_str(80, 50, "PC");
    fb_print_int_hex(98, 50, epc, 32);

    if(errno >= 2 && errno <= 5)
    {
        /*
        2 UNMAPPED LOAD ADDR
        3 UNMAPPED STORE ADDR
        4 BAD LOAD ADDR
        5 BAD STORE ADDR
        */
        u32 badvaddr = cop0_get_badvaddr();

        fb_print_str(158, 50, "BADVADDR");
        fb_print_int_hex(212, 50, badvaddr, 32);
    }

    fb_print_gpr_states(80, 70, szGPRegisters1, &exceptionRegContext[6 + 0]);
    fb_print_gpr_states(158, 70, szGPRegisters2, &exceptionRegContext[6 + 15*2]);

    fb_swap();

    while(1) // hang forever
    {
        volatile int t = 0; // keep pj64 happy
    }
}

u8 ascii_to_idx(char c)
{
    if(c >= 'a' && c <= 'z')
    {
        return c - 0x57;
    }

    if(c >= 'A' && c <= 'Z')
    {
        return c - 0x37;
    }

    if(c >= '0' && c <= '9')
    {
        return c - 0x30;
    }

    return 0xFF;
}

void fb_set_address(void *address)
{
    fbAddress = (u16 *) address;
}

void fb_swap()
{
    // update VI frame buffer register
    // todo other registers
    *(u32 *)(0xA4400004) = (u32)fbAddress & 0x00FFFFFF;
}

void fb_fill(int baseX, int baseY, int width, int height)
{
    for(int y = baseY; y < baseY + height; y++)
    {
        for(int x = baseX; x < baseX + width; x++)
        {
            fbAddress[y*320+x] = fbFillColor;
        }
    }
}

void fb_draw_char(int x, int y, u8 idx)
{
    if(idx == 0xFF)
    {
        return;
    }

    u16 *out = &fbAddress[y*320 + x];
    const u8 *in = &crashFont[idx*3];

    for(int nbyte = 0; nbyte < 3; nbyte++)
    {
        u8 curbyte = in[nbyte];

        for(int nrow = 0; nrow < 2; nrow++)
        {
            for(int ncol = 0; ncol < 4; ncol++)
            {
                u8 px = curbyte & (1 << 7-(nrow*4+ncol));
                if(px != 0)
                {
                    out[ncol] = fbFillColor;
                }
            }
            out += 320;
        }
    }
}

void fb_draw_char_shaded(int x, int y, u8 idx)
{
    fbFillColor = 0x0001;
    fb_draw_char(x - 1, y + 1, idx);

    fbFillColor = 0xFFFF;
    fb_draw_char(x, y, idx);
}

void fb_print_str(int x, int y, const char *str)
{
    while(1)
    {
        u8 idx;
        char c = *str++;

        if(c == '\0')
        {
            break;
        }

        if(c == ' ')
        {
            x += 6;
            continue;
        }

        idx = ascii_to_idx(c);
        fb_draw_char_shaded(x, y, idx);
        x += 6;
    }
}

void fb_print_int_hex(int x, int y, u32 value, int nbits)
{
    nbits -= 4;

    while(nbits >= 0)
    {
        int idx = (value >> nbits) & 0xF;

        fb_draw_char_shaded(x, y, idx);
        x += 6;

        nbits -= 4;
    }
}

void fb_print_gpr_states(int x, int y, const char* regNames[], u32 *regContext)
{
    for(int i = 0;; i++)
    {
        if(regNames[i] == NULL)
        {
            break;
        }

        fb_print_str(x, y, regNames[i]);
        fb_print_int_hex(x + (3*6), y, regContext[i*2+1], 32);
        y += 10;
    }
}