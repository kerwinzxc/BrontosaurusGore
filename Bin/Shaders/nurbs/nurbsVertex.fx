#include "nurbsCommon.fx"

InputGeometry VS_PosSizeColor(InputVertex input)
{
    InputGeometry output = (InputGeometry)0;
    output.rotation = input.position.w;
    output.position = float4(input.position.xyz, 1.0f);
    //output.position = mul(worldSpace, output.position);
    output.position = mul(cameraSpaceInversed, output.position);

    output.size = input.size;
    output.color = input.color;
    output.center = input.position.xyz;
    return output;
}