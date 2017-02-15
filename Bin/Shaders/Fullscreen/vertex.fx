#include <structs.fx>

PosTex_InputPixel VS_PosTex(PosTex_InputVertex input)
{
	PosTex_InputPixel output;
	output.position = input.position;
    output.tex = input.tex;

	return output;
}
