#include "../../header/client/sketcher.h"
iRenderAssembly* iRenderAssembly::regist_action() noexcept
{
    this->vm_.create_named_table("Set",
        "SetLineStyle", [](int style, int thickness = 1)
                        { setlinestyle(style, thickness); },

        "SetLineColor", setlinecolor,

        "SetFillStyle", [](int style, long hatch = 0L)
                        { setfillstyle(style, hatch); },

        "SetFillColor", setfillcolor,

        "SetTextStyle", [](int height, int width, std::string &&font)
                        { settextstyle(height, width, std::wstring(font.begin(), font.end()).c_str()); });

    this->vm_.create_named_table("Act",
        "Xyout", [](int x, int y, std::string &&text)
                 { outtextxy(x, y, std::wstring(text.begin(), text.end()).c_str()); },

        "Circle", circle,
        "Line", line,
        "FillCircle", fillcircle);

    this->vm_.create_named_table("LineStyle",
                                 "Solid", PS_SOLID,
                                 "Dash", PS_DASH,
                                 "Dot", PS_DOT,
                                 "DashDot", PS_DASHDOT);

    this->vm_.create_named_table("FillStyle",
                                 "Solid", BS_SOLID,
                                 "Null", BS_NULL,
                                 "Hatched", BS_HATCHED);

    this->vm_.create_named_table("Hatch",
                                 "Cube", HS_CROSS,
                                 "DiagCube", HS_DIAGCROSS);
    this->vm_.create_named_table("ThreadId",
        "C", ThreadId::C,
        "U", ThreadId::U,
        "N", ThreadId::N,
        "R", ThreadId::R);
    return this;
}

Sketcher *Sketcher::initvm() noexcept
{
    this->open_libraries();
    this->vm_.create_named_table("LogTable");
    this->vm_.create_named_table("TaskQue");
    return this;
}

Sketcher *Sketcher::ensure() noexcept
{
    this->luavm()->operator[]("SketcherEnsure").call();
    return this;
}

Cache *Cache::ensure() noexcept
{
    this->luavm()->operator[]("CacheEnsure").call();
    return this;
}
