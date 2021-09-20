
--- Color, https://www.cnblogs.com/cainiao-chuanqi/p/11301471.html
local module <const> = {
    Blue            = 0x0000FF,
    White           = 0xFFFFFF,
    Red             = 0xF0FFFF,
    LimeGreen       = 0x32CD32,
    Yellow          = 0xFFFF00,
    Gold            = 0xFFD700,
    Gray            = 0x808080,
}

function RGB(r, g, b)
    local _r, _g, _b = r or 0, g or 0, b or 0
    if _r > 255 then _r = 255 end
    if _g > 255 then _g = 255 end
    if _b > 255 then _b = 255 end 
    return _r | (_g << 8) | (_b << 16) 
end

return module;