#include <NXPUI.h>

namespace NXP::UI {
    void initUI()
    {
        // Create the header
        Arriba::Primitives::Quad* headerBase = new Arriba::Primitives::Quad(0, 0, Arriba::Graphics::windowWidth, headerFooterHeight, Arriba::Graphics::Pivot::topLeft);
        headerBase->setColour(mainUIColour);
        // N-Xplorer text
        Arriba::Primitives::Text* NXPText = new Arriba::Primitives::Text("N-Xplorer", 48);
        NXPText->setParent(headerBase);
        NXPText->setColour(UITextColour);
        NXPText->transform.position.x += NXPText->width / 2;
        NXPText->transform.position.y += headerFooterHeight / 2;
        // Clock
        Arriba::Primitives::Text* headerClock = new NXP::UI::Objects::clock(headerFooterHeight / 2, 48);
        headerClock->setParent(headerBase);
        headerClock->setColour(UITextColour);
        // Create the footer
        int footerY = Arriba::Graphics::windowHeight - headerFooterHeight;
        Arriba::Primitives::Quad* footerBase = new Arriba::Primitives::Quad(0, footerY, Arriba::Graphics::windowWidth, headerFooterHeight, Arriba::Graphics::Pivot::topLeft);
        footerBase->setColour(mainUIColour);
        // Create path text
        Arriba::Primitives::Text* pathText = new Arriba::Primitives::Text("Mount:/", 48);
        pathText->setParent(footerBase);
        pathText->transform.position.x = pathText->width / 2;
        pathText->transform.position.y = headerFooterHeight / 2;
        pathText->setColour(UITextColour);
        pathText->name = "PathTextObject";
    }

    void pathChanged() {
        Arriba::Primitives::Text* pathText = (Arriba::Primitives::Text*)Arriba::findObjectByName("PathTextObject");
        pathText->setText(NXPState::browserPath.c_str());
        pathText->transform.position.x = pathText->width / 2;
        pathText->transform.position.y = headerFooterHeight / 2;
    }
}

namespace NXP::UI::Objects {
    clock::clock(int yOffset, int fontSize) : Arriba::Primitives::Text("00/00/2023 00:00:00", fontSize){
        transform.position.x = Arriba::Graphics::windowWidth - width/2;
        transform.position.y += yOffset;
    }

    void clock::onFrame() {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char timeString[87];
        sprintf(timeString, "%02d/%02d/%d %02d:%02d:%02d ", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
        setText(timeString);
    }
}