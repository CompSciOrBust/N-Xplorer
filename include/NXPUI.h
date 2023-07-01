#pragma once
#include <arriba.h>
#include <arribaElements.h>
#include <arribaPrimitives.h>
#include <NXPState.h>

namespace NXP::UI {
    inline Arriba::Maths::vec4<float> mainUIColour = {0.37,0.37,0.37,1.0};
    inline Arriba::Maths::vec4<float> UIColourAccent = {0.25,0.25,0.25,1.0};
    inline Arriba::Maths::vec4<float> UIColourHighlightedA = {0.63,0.63,0.63,1.0};
    inline Arriba::Maths::vec4<float> UIColourHighlightedB = {0.43,0.43,0.43,1.0};
    inline Arriba::Maths::vec4<float> UIBGColour = {0.17,0.17,0.17,1.0};
    inline Arriba::Maths::vec4<float> UITextColour = {1.0,1.0,1.0,1.0};
    inline Arriba::Maths::vec4<float> UIBorderColour = {0.0,0.0,0.0,1.0};
    inline const int headerFooterHeight = 70;
    void initUI();
    void pathChanged();
}

namespace NXP::UI::Objects {
    class clock : public Arriba::Primitives::Text {
        public:
            clock(int yOffset, int fontSize);
            virtual void onFrame();
    };

    class fileBrowser : public Arriba::Graphics::AdvancedTexture, public Arriba::Primitives::Quad {
        public:
            fileBrowser(int _x, int _y, int _width, int _height);
            void refreshFileListing();
            virtual void onFrame();
            virtual void update();

        private:
            const unsigned int entryHeight = 40;
            unsigned int itemCount = 0;
            int selectedIndex = 0;
            float inertia = 0;
            Arriba::Primitives::Quad* root = nullptr;
            Arriba::Primitives::Quad* bg = nullptr;
    };
}