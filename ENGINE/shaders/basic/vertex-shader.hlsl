// Vertex shader input structure using xyznxnynzuv format
struct VS_INPUT
{
    float3 Pos : POSITION; // Position (x, y, z, w)
    float3 Normal : NORMAL; // Normal (nx, ny, nz)
    float2 TexCoord : TEXCOORD; // Texture coordinates (u, v)
};

// Output structure for vertex shader
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION; // Clip space position (x, y, z, w)
    float3 Normal : NORMAL; // Normal (nx, ny, nz)
    float2 TexCoord : TEXCOORD; // Texture coordinates (u, v)
    float4 Color : COLOR; // Color
};

// Vertex shader
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    


    // Transform position to clip space (you can apply a model-view-projection matrix here)
    output.Pos = float4(input.Pos + float3(0.0f, 0.0f, 0.0f), 1.0);
    
    // Pass normal and texture coordinates through
    output.Normal = input.Normal;
    output.TexCoord = input.TexCoord;
    
    // You can compute the color here, or pass it from the input
    output.Color = float4(input.Normal, 1.0f); // Example color (Red)
    
    return output;
}

// Pixel shader
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    // For simplicity, returning the color from the vertex shader output
    return input.Color;
}
