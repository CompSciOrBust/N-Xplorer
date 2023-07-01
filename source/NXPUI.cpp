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
        // Create file browser
        NXP::UI::Objects::fileBrowser* fb = new NXP::UI::Objects::fileBrowser(0, headerFooterHeight, Arriba::Graphics::windowWidth, Arriba::Graphics::windowHeight - (2 * headerFooterHeight));
        fb->name = "FileBrowserObject";
        Arriba::highlightedObject = fb;
    }

    void pathChanged() {
        Arriba::Primitives::Text* pathText = (Arriba::Primitives::Text*)Arriba::findObjectByName("PathTextObject");
        pathText->setText(NXPState::browserPath.c_str());
        pathText->transform.position.x = pathText->width / 2;
        pathText->transform.position.y = headerFooterHeight / 2;
        NXP::UI::Objects::fileBrowser* fb = (NXP::UI::Objects::fileBrowser*)Arriba::findObjectByName("FileBrowserObject");
        fb->refreshFileListing();
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

    fileBrowser::fileBrowser(int _x, int _y, int _width, int _height) : Arriba::Graphics::AdvancedTexture(_width, _height), Arriba::Primitives::Quad(_x, _y, _width, _height, Arriba::Graphics::Pivot::topLeft) {
        // Set the texture to the framebuffer
        renderer->setTexture(texID);
        // Create the background
        bg = new Arriba::Primitives::Quad(0, 0, _width, _height, Arriba::Graphics::Pivot::topLeft);
        bg->setFBOwner(this);
        bg->setColour(NXP::UI::UIBGColour);
        // Create the file quads
        root = new Arriba::Primitives::Quad(0,0,0,0,Arriba::Graphics::Pivot::centre);
        root->setColour({0.0f,0.0f,0.0f,0.0f});
        root->setFBOwner(this);
        refreshFileListing();
    }

    void fileBrowser::refreshFileListing() {
        // Reset the state
        selectedIndex = 0;
        inertia = 0;

        // Clear the existing entries
        std::vector<Arriba::UIObject*> rects = root->getChildren();
        for (size_t i = 0; i < rects.size(); i++)
        {
            root->getChildren()[0]->destroy();
        }

        // Create the new entries
        itemCount = NXPState::filesInCurrentPath.size();
        for (size_t i = 0; i < itemCount; i++)
        {
            // Create the rects
            Arriba::Primitives::Quad* entryRect = new Arriba::Primitives::Quad(0, entryHeight * i, width, entryHeight, Arriba::Graphics::Pivot::topLeft);
            entryRect->setFBOwner(this);
            entryRect->setParent(root);
            entryRect->setColour(NXP::UI::UIColourAccent);
            // Create the text
            Arriba::Primitives::Text* fileNameText = new Arriba::Primitives::Text(NXPState::filesInCurrentPath[i].name.c_str(), 30);
            fileNameText->setParent(entryRect);
            fileNameText->setColour(NXP::UI::UITextColour);
            fileNameText->transform.position.x += fileNameText->width / 2;
            fileNameText->transform.position.y += fileNameText->height / 2 + entryRect->height / 4;
            // Create top border
            Arriba::Primitives::Quad* topBorder = new Arriba::Primitives::Quad(0, 0, width, 3, Arriba::Graphics::Pivot::topLeft);
            topBorder->setParent(entryRect);
            topBorder->setColour(NXP::UI::UIBorderColour);
        }
    }

    void fileBrowser::onFrame() {
        // Handle button / stick input
        if (Arriba::highlightedObject == this) {
            // Up pressed
            if(Arriba::Input::buttonDown(Arriba::Input::controllerButton::DPadUp)) {
                selectedIndex -= 1;
                if (selectedIndex < 0) selectedIndex = itemCount - 1;
            }
            // Down pressed
            if(Arriba::Input::buttonDown(Arriba::Input::controllerButton::DPadDown)) {
                selectedIndex += 1;
                if (selectedIndex > itemCount-1) selectedIndex = 0;
            }
            // A pressed
            if (Arriba::Input::buttonDown(Arriba::Input::controllerButton::AButtonSwitch)) {
                // Do an action depending on the file type
                switch (NXPState::filesInCurrentPath[selectedIndex].entryType)
                {
                    // If folder / device open it
                    case FSEntryType::Device:
                    case FSEntryType::Folder:
                        NXPState::changePath(NXPState::filesInCurrentPath[selectedIndex].absolutePath);
                    break;
                }
            }
        }

        // Handle touch input
        if(Arriba::Input::touchScreenPressed() && Arriba::activeLayer == layer) {
            // Check if the list is being touched
            Arriba::Maths::vec3<float> pos = getGlobalPos().col4;
            float touchX = Arriba::Input::touch.pos.x;
            float touchY = Arriba::Input::touch.pos.y;
            bool listIsBeingTouched = (touchY < getTop() && touchY > getBottom() && touchX < getRight() && touchX > getLeft());
            // If the list is being touched
            if (listIsBeingTouched) {
                Arriba::highlightedObject == this;
            }
        }

        // Loop through each item to set the colour
        for (unsigned int i = 0; i < itemCount; i++) {

            if (i == selectedIndex) root->getChildren()[i]->setColour(NXP::UI::UIColourHighlightedA);
            else root->getChildren()[i]->setColour(NXP::UI::UIColourAccent);
        }

        // If the selected index is off the screen add inertia to bring it back on screen
        if(selectedIndex >= 0)
        {
            // 3 was chosen here because 0.7^x as x goes to infinity converges on 3
            if(selectedIndex * entryHeight + root->transform.position.y < 0) inertia = (selectedIndex * entryHeight + root->transform.position.y) / -3;
            else if((selectedIndex+1) * entryHeight + root->transform.position.y > Quad::height) inertia = ((selectedIndex+1) * entryHeight + root->transform.position.y - Quad::height) / -3;
        }

        // Deal with inertia
        root->transform.position.y += inertia;
        inertia *= 0.7;

        // Redraw the framebuffer
        update();
    }

    void fileBrowser::update() {
        // Render framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.0f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawTextureObject(bg);

        // Draew file rects
        std::vector<Arriba::UIObject*> rects = root->getChildren();
        for (size_t i = 0; i < rects.size(); i++)
        {
            root->getChildren()[i]->renderer->updateParentTransform(root->renderer->getTransformMatrix());
            drawTextureObject(root->getChildren()[i]);
        }
        

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}