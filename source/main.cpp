#include <arriba.h>
#include <NXPUI.h>
#include <NXPState.h>

// Main program entrypoint
int main(int argc, char* argv[])
{
    // Init
    socketInitializeDefault();
    nxlinkStdio();
    romfsInit();
    Arriba::init();

    NXP::UI::initUI();

    printf("NXP Init done!\n");
    NXPState::changePath("");

    // Main loop
    while (appletMainLoop())
    {
        if(Arriba::Input::buttonUp(Arriba::Input::PlusButtonSwitch)) break;
        Arriba::drawFrame();
    }

    // Deinit
    socketExit();
    romfsExit();
    Arriba::exit();
    return 0;
}
