#include <sm64.h>

#include "hello_world.h"
#include "crash.h"

//extern u8 __osException[];



// Called after the custom block is loaded
void hook_custom_sec_loaded(void)
{
    generate_exception_preambles(__crash_handler_entry);
}

// Called from Mario's behavior routine
void hook_mario_behavior(void)
{
    N64_ASSERT(gMarioCoins == 0);
}

// Called every frame, before the last G_ENDDL command is pushed to the master display list
void hook_dlist_end(void)
{
    //hello_world();
}
