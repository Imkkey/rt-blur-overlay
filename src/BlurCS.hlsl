Texture2D Frame0 : register(t0);
Texture2D Frame1 : register(t1);
Texture2D Frame2 : register(t2);
Texture2D Frame3 : register(t3);
RWTexture2D<float4> OutImage : register(u0);

[numthreads(8,8,1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    float4 a = Frame0[id.xy];
    float4 b = Frame1[id.xy];
    float4 c = Frame2[id.xy];
    float4 d = Frame3[id.xy];
    // Exponential weights: 0.5,0.25,0.125,0.125
    float4 result = a*0.5 + b*0.25 + c*0.125 + d*0.125;
    OutImage[id.xy] = result;
}
