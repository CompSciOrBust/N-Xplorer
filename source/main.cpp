#include <arriba.h>
#include <NXPUI.h>
#include <NXPState.h>

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Init
    romfsInit();
    Arriba::init();

    NXP::UI::initUI();

    // Main loop
    while (appletMainLoop())
    {
        if(Arriba::Input::buttonUp(Arriba::Input::PlusButtonSwitch)) break;
        if(Arriba::Input::buttonUp(Arriba::Input::AButtonSwitch)) NXPState::changePath("sdmc:/bootloader/");
        Arriba::drawFrame();
    }

    // Deinit
    romfsExit();
    Arriba::exit();
    return 0;
}
