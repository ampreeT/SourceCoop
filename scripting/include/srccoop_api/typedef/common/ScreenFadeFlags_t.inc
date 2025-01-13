#pragma newdecls required
#pragma semicolon 1

//------------------------------------------------------
// Screen Fade flags
//------------------------------------------------------
enum ScreenFadeFlags_t
{
    FFADE_IN = 0x0001,          // Just here so we don't pass 0 into the function
    FFADE_OUT = 0x0002,         // Fade out (not in)
    FFADE_MODULATE = 0x0004,    // Modulate (don't blend)
    FFADE_STAYOUT = 0x0008,     // ignores the duration, stays faded out until new ScreenFade message received
    FFADE_PURGE = 0x0010,       // Purges all other fades, replacing them with this one
}
