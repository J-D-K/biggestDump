#pragma once

#include "gfx.h"

class console
{
    public:
        console(int maxLines){ maxl = maxLines; }
        void out(const font *f, const std::string& s)
        {
            lines.push_back(s);
            if((int)lines.size() >= maxl)
                lines.erase(lines.begin());

            //cheat
            drawRect(frameBuffer, 30, 88, 1220, 560, clrCreateU32(0xFF2D2D2D));

            for(unsigned i = 0; i < lines.size(); i++)
            {
                drawText(lines[i].c_str(), frameBuffer, f, 38, 98 + (i * 20), 16, clrCreateU32(0xFFFFFFFF));
            }

            gfxHandleBuffs();
        }

    private:
        std::vector<std::string> lines;
        int maxl;
};
