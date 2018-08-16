#include <sm64.h>

#include "hello_world.h"
#include "crash.h"

extern void __osException(void);

// Called after the custom block is loaded
void hook_custom_sec_loaded(void)
{
    generate_exception_preambles(crash_handler_entry, __osException);
}

// Called from Mario's behavior routine
void hook_mario_behavior(void)
{
    // kill Mario if he grabs 5 or more coins
    if(gMarioCoins >= 5)
    {
        gMarioHealth = 0; 
    }
}

// Called every frame, before the last G_ENDDL command is pushed to the master display list
void hook_dlist_end(void)
{
    hello_world();
}
