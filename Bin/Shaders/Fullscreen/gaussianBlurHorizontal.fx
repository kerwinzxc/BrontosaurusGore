#include <blur.fx>

// COPY SHADER
PixelOutput PS_PosTex(PosTex_InputPixel input)
{
    return PS_HorizontalBlur(input);
}
