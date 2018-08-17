.definelabel COP0_CAUSE, 13
.definelabel COP0_EPC, 14
.definelabel COP0_BADVADDR, 8

//.definelabel exceptionRegContext, 0x80365F40 // JP D_80365F40
//.definelabel exceptionRegContext, 0x803672B0 // US

//.definelabel __osException, 0x80327650 // US

crashFont: // PIXEL8 font by fraser
    .incbin "img/PIXEL8.FNT"

exceptionRegContext: .fill 0x108

pAssertFile: .dw 0
nAssertLine: .dw 0
pAssertExpression: .dw 0
nAssertStopProgram: .dw 0

_n64_assert:
    sw   a0, pAssertFile
    sw   a1, nAssertLine
    sw   a2, pAssertExpression
    sw   a3, nAssertStopProgram
    beqz a3, @@end
    nop
    syscall // trigger crash screen
    @@end:
    jr ra
    nop

cop0_get_cause:
    jr   ra
    mfc0 v0, cause

cop0_get_epc:
    jr   ra
    mfc0 v0, epc

cop0_get_badvaddr:
    jr   ra
    mfc0 v0, badvaddr

// If the error code field of cop0's cause register is non-zero,
// draw crash details to the screen and hang
//
// If there wasn't an error, continue to the original handler

__crash_handler_entry:
    la    k0, exceptionRegContext
    sd    r0, 0x018 (k0)
    sd    at, 0x020 (k0)
    sd    v0, 0x028 (k0)
    sd    v1, 0x030 (k0)
    sd    a0, 0x038 (k0)
    sd    a1, 0x040 (k0)
    sd    a2, 0x048 (k0)
    sd    a3, 0x050 (k0)
    sd    t0, 0x058 (k0)
    sd    t1, 0x060 (k0)
    sd    t2, 0x068 (k0)
    sd    t3, 0x070 (k0)
    sd    t4, 0x078 (k0)
    sd    t5, 0x080 (k0)
    sd    t6, 0x088 (k0)
    sd    t7, 0x090 (k0)
    sd    s0, 0x098 (k0)
    sd    s1, 0x0A0 (k0)
    sd    s2, 0x0A8 (k0)
    sd    s3, 0x0B0 (k0)
    sd    s4, 0x0B8 (k0)
    sd    s5, 0x0C0 (k0)
    sd    s6, 0x0C8 (k0)
    sd    s7, 0x0D0 (k0)
    sd    t8, 0x0D8 (k0)
    sd    t9, 0x0E0 (k0)
    sd    gp, 0x0E8 (k0)
    sd    sp, 0x0F0 (k0)
    sd    s8, 0x0F8 (k0)
    sd    ra, 0x100 (k0)
    mfc0  t0, cause
    srl   t0, t0, 2
    andi  t0, t0, 0x1F
    beqz  t0, @@end
    nop
    // cop unusable exception fired twice on startup so we'll ignore it for now
    li    at, 0x0B
    beq   t0, at, @@end
    nop
    jal   show_crash_screen_and_hang
    nop
    @@end:
    ld    r0, 0x018 (k0)
    ld    at, 0x020 (k0)
    ld    v0, 0x028 (k0)
    ld    v1, 0x030 (k0)
    ld    a0, 0x038 (k0)
    ld    a1, 0x040 (k0)
    ld    a2, 0x048 (k0)
    ld    a3, 0x050 (k0)
    ld    t0, 0x058 (k0)
    ld    t1, 0x060 (k0)
    ld    t2, 0x068 (k0)
    ld    t3, 0x070 (k0)
    ld    t4, 0x078 (k0)
    ld    t5, 0x080 (k0)
    ld    t6, 0x088 (k0)
    ld    t7, 0x090 (k0)
    ld    s0, 0x098 (k0)
    ld    s1, 0x0A0 (k0)
    ld    s2, 0x0A8 (k0)
    ld    s3, 0x0B0 (k0)
    ld    s4, 0x0B8 (k0)
    ld    s5, 0x0C0 (k0)
    ld    s6, 0x0C8 (k0)
    ld    s7, 0x0D0 (k0)
    ld    t8, 0x0D8 (k0)
    ld    t9, 0x0E0 (k0)
    ld    gp, 0x0E8 (k0)
    ld    sp, 0x0F0 (k0)
    ld    s8, 0x0F8 (k0)
    ld    ra, 0x100 (k0)
    lui   k0, hi(__osException)
    addiu k0, k0, lo(__osException)
    jr    k0 // run the original handler
    nop
