.include "asm/sections.asm"
.include "asm/variables.asm"
.include "asm/functions.asm"

/******************** Custom segment ********************/

.headersize SEC_CUSTOM_HEADERSIZE
.org SEC_CUSTOM_RAM
.area SEC_CUSTOM_SIZE, 0

// Compiled code

.importlib "lib/libsec_custom.a"
.align 4

// Hook wrappers

__wrap_hook_dlist_end:
    addiu sp, sp, -0x18
    sw    ra, 0x14 (sp)
    jal   0x8024784C
    nop
    jal   hook_dlist_end
    nop
    lw    ra, 0x14 (sp)
    jr    ra
    addiu sp, sp, 0x18

.endarea // SEC_CUSTOM_SIZE

/******************** Custom segment loader ********************/

.headersize SEC_MAIN_HEADERSIZE

.org 0x80248AD8
    // overwrite useless branch in one of the bootup functions
    jal __load_custom_segment

.org 0x803396D8
__load_custom_segment:
    // overwrite unused debug text
    addiu sp, sp, -0x18
    sw    ra, 0x14 (sp)
    la    t0, SEC_CUSTOM_SIZE
    la    a0, SEC_CUSTOM_RAM
	la    a1, SEC_CUSTOM_ROM
	jal   dma_copy
    addu  a2, a1, t0
	nop
    jal   hook_custom_sec_loaded
    nop
    lw    ra, 0x14 (sp)
    jr    ra
    addiu sp, sp, 0x18

/******************** Hooks ********************/

.headersize SEC_MAIN_HEADERSIZE

// overwrite useless branch at the end of mario's behavior function
.org 0x802CB248
    jal hook_mario_behavior

// overwrite a jal at the start of the function
//  that writes the G_ENDDL command to the master display list
.org 0x80247D1C
    // was jal 0x8024784C
    jal __wrap_hook_dlist_end
