#include <sm64.h>

#include "crash.h"

extern u32 exceptionRegContext[];
extern u8 crashFont[];

extern char *pAssertFile;
extern int   nAssertLine;
extern char *pAssertExpression;
extern int   nAssertStopProgram;

u16  fbFillColor = 0xFFFF;
u16  fbShadeColor = 0x0001;
u16 *fbAddress = NULL;

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
    eg: generate_exception_preambles(__crash_handler_entry);
*/
void generate_exception_preambles(void *entryPoint)
{
    u32 *out = (u32 *) MIPS_KSEG1;

    u16 entryPointHi = (u32)entryPoint >> 16;
    u16 entryPointlo = (u32)entryPoint & 0xFFFF;

    if(entryPointlo & 0x8000)
    {
        entryPointHi++;
    }

    for(int i = 0; i < 2; i++)
    {
        out[0] = 0x3C1A0000 | entryPointHi; // 0x00: lui   k0, hi(entryPoint)
        out[1] = 0x275A0000 | entryPointlo; // 0x04: addiu k0, k0, lo(entryPoint)
        out[2] = 0x03400008;                // 0x08: jr    k0
        out[3] = 0x00000000;                // 0x0C: nop

        out += 0x180 / sizeof(u32);
    }
}

int crash_strlen(char *str)
{
    int len = 0;
    while(*str++)
    {
        len++;
    }
    return len;
}

void show_crash_screen_and_hang(void)
{
    //u32 benchStart, benchStop;
    u32 cause, epc, errno;

    //benchStart = cop0_get_count();
    cause = cop0_get_cause();
    epc = cop0_get_epc();
    errno = (cause >> 2) & 0x1F;

    fb_set_buffer(FB_DEFAULT_ADDR);

    fb_set_shade_color(0, 0, 0);

    if(nAssertStopProgram == 0)
    {
        // show error screen

        fb_set_fill_color(12, 9, 9);
        fb_fill_rect(0, 0, CRASH_SCREEN_W, CRASH_SCREEN_H);

        fb_set_fill_color(31, 31, 31);

        fb_print_str(80, 20, "ERROR");
        fb_print_int_hex(80, 30, errno, 8);
        fb_print_str(107, 30, szErrCodes[errno]);

        // show badvaddr if relevant
        if(errno >= 2 && errno <= 5)
        {
            //2 UNMAPPED LOAD ADDR
            //3 UNMAPPED STORE ADDR
            //4 BAD LOAD ADDR
            //5 BAD STORE ADDR

            u32 badvaddr = cop0_get_badvaddr();

            fb_print_str(145, 50, "VA");
            fb_print_int_hex(172, 50, badvaddr, 32);
        }

        // print out gpr registers
        fb_print_gpr_states(80, 70, szGPRegisters1, &exceptionRegContext[6 + 0]);
        fb_print_gpr_states(145, 70, szGPRegisters2, &exceptionRegContext[6 + 15*2]);
    }
    else
    {
        // show assert screen

        int afterFileX, exprBoxWidth;

        fb_set_fill_color(10, 9, 17);
        fb_fill_rect(0, 0, CRASH_SCREEN_W, CRASH_SCREEN_H);

        fb_set_fill_color(31, 31, 31);
        fb_print_str(80, 20, "ASSERT");
        
        afterFileX = fb_print_str(80, 30, pAssertFile);
        fb_print_str(afterFileX, 30, ":");
        fb_print_uint(afterFileX + 5, 30, nAssertLine);

        exprBoxWidth = (crash_strlen(pAssertExpression) * 5) + 2;
        fb_set_fill_color(0, 0, 0);
        fb_fill_rect(80-1, 40-1, exprBoxWidth, 10);

        fb_set_fill_color(31, 31, 31);
        fb_print_str(80, 40, pAssertExpression);
    }

    fb_print_str(80, 50, "PC");
    fb_print_int_hex(95, 50, epc, 32);
    
    //benchStop = cop0_get_count();
    // show num cycles it took to render
    //fb_print_uint(20, 20, benchStop - benchStart); 

    fb_swap();

    while(1) // hang forever
    {
        volatile int t = 0; // keep pj64 happy
    }
}

void fb_invalidate(void)
{
    // todo less stupid way of doing this
    for(int i = 0; i < (CRASH_SCREEN_W * CRASH_SCREEN_H); i++)
    {
        volatile u16 t = fbAddress[i];
    }
}

void fb_swap()
{
    fb_invalidate();

    // todo other registers
    vi_set_fb_vaddr(fbAddress);
}

void fb_fill_rect(int baseX, int baseY, int width, int height)
{
    for(int y = baseY; y < baseY + height; y++)
    {
        for(int x = baseX; x < baseX + width; x++)
        {
            fbAddress[y*CRASH_SCREEN_W+x] = fbFillColor;
        }
    }
}

void fb_draw_char(int x, int y, u8 idx)
{
    u16 *out = &fbAddress[y*CRASH_SCREEN_W + x];
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

                    if(fbShadeColor & 1)
                    {
                        out[ncol-1 + CRASH_SCREEN_W] = fbShadeColor;
                    }
                }
            }

            out += CRASH_SCREEN_W;
        }
    }
}

int fb_print_str(int x, int y, const char *str)
{
    while(1)
    {
        int yoffs = 0;
        char c = *str++;

        if(c == '\0')
        {
            break;
        }

        if(c == ' ')
        {
            x += 5;
            continue;
        }

        switch(c)
        {
        case 'j':
        case 'g':
        case 'p':
        case 'q':
        case 'y':
        case 'Q':
            yoffs = 1;
            break;
        case ',':
            yoffs = 2;
            break;
        }

        fb_draw_char(x, y + yoffs, ctoidx(c));
        x += 5;
    }

    return x;
}

void fb_print_int_hex(int x, int y, u32 value, int nbits)
{
    nbits -= 4;

    while(nbits >= 0)
    {
        int nib = ((value >> nbits) & 0xF);
        u8 idx;

        if(nib > 9)
        {
            idx = ctoidx('A') + (nib-0xA);
        }
        else
        {
            idx = ctoidx('0') + nib;
        }

        fb_draw_char(x, y, idx);
        x += 5;

        nbits -= 4;
    }
}

int fb_print_uint(int x, int y, u32 value)
{
    int nchars = 0;

    int v = value;
    while(v /= 10)
    {
        nchars++;
    }

    x += nchars * 5;

    for(int i = nchars; i >= 0; i--)
    {
        fb_draw_char(x, y, ctoidx('0') + (value % 10));
        value /= 10;
        x -= 5;
    }

    return (x + nchars*5);
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
        fb_print_int_hex(x + 15, y, regContext[i*2+1], 32);
        y += 10;
    }
}