struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color: COLOR;
};

VS_OUTPUT VS(float4 inPos: POSITION, float4 inColor : COLOR)
{
    VS_OUTPUT output;
    
    output.Pos = inPos;
    output.Color = inColor;
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    //return float4(0.0f, 0.0f, 1.0f, 1.0f);
    
    // Don't know why it's not working
    return float4(input.Color.rgb, 1.0);
}
