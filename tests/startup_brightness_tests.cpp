#include <cassert>

#include "startupIntro/Brightness.h"

int main()
{
    assert(startup_intro::HiddenBrightness == 0x8010);
    assert(startup_intro::BlackBrightness(16) == 0x8010);
    assert(startup_intro::BlackBrightness(1) == 0x8001);
    assert(startup_intro::BlackBrightness(0) == 0x8000);
    assert(startup_intro::MainBackdropColor == 0);
    assert(startup_intro::MainBackdropDisplayMode == (1u << 16));
}
