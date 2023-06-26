#pragma once
#include <arriba.h>
#include <arribaElements.h>
#include <arribaPrimitives.h>
#include <NXPState.h>

namespace NXP::UI {
    inline Arriba::Maths::vec4<float> mainUIColour = {0.37,0.37,0.37,1.0};
    inline Arriba::Maths::vec4<float> UITextColour = {1.0,1.0,1.0,1.0};
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
}