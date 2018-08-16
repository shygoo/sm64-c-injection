.ifndef _SECTIONS_ASM_
.definelabel _SECTIONS_ASM_, 1

/********** Custom section **********/

// Stored over padding bytes at the end of ROM
// These addresses may be changed to anywhere that has ample free space

.definelabel SEC_CUSTOM_ROM,  0x007CC6C0
.definelabel SEC_CUSTOM_RAM,  0x80400000
.definelabel SEC_CUSTOM_HEADERSIZE, (SEC_CUSTOM_RAM - SEC_CUSTOM_ROM)
.definelabel SEC_CUSTOM_SIZE, 0x00010000

/********** Existing sections **********/

.definelabel SEC_MAIN_ROM, 0x00001000
.definelabel SEC_MAIN_RAM, 0x80246000
.definelabel SEC_MAIN_HEADERSIZE, (SEC_MAIN_RAM - SEC_MAIN_ROM)

.definelabel SEC_ENGINE_ROM, 0x000F5580
.definelabel SEC_ENGINE_RAM, 0x80378800
.definelabel SEC_ENGINE_HEADERSIZE, (SEC_ENGINE_RAM - SEC_ENGINE_ROM)

.definelabel SEC_GODDARD_ROM, 0x0021F4C0
.definelabel SEC_GODDARD_RAM, 0x8016F000
.definelabel SEC_GODDARD_HEADERSIZE, (SEC_GODDARD_RAM - SEC_GODDARD_ROM)

.endif // _SECTIONS_ASM_
