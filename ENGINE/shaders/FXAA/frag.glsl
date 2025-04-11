#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D sceneTexture;  // The original rendered scene
vec2 resolution = vec2(1280, 720);         // The resolution of the screen or framebuffer

// FXAA shader constants
const float fxaaQualitySubpix = 0.75; 
const float fxaaQualityEdgeThreshold = 0.166;

// Function to compute FXAA
vec4 fxaa(sampler2D tex, vec2 fragCoord, vec2 resolution)
{
    vec2 texel = 1.0 / resolution;
    vec3 color = texture(tex, fragCoord).rgb;

    // Calculate the difference between the current pixel and neighboring pixels
    vec3 colorNW = texture(tex, fragCoord + vec2(-texel.x, -texel.y)).rgb;
    vec3 colorNE = texture(tex, fragCoord + vec2(texel.x, -texel.y)).rgb;
    vec3 colorSW = texture(tex, fragCoord + vec2(-texel.x, texel.y)).rgb;
    vec3 colorSE = texture(tex, fragCoord + vec2(texel.x, texel.y)).rgb;

    // Compute the luma (brightness) of the current pixel and its neighbors
    float luma = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    float lumaNW = 0.2126 * colorNW.r + 0.7152 * colorNW.g + 0.0722 * colorNW.b;
    float lumaNE = 0.2126 * colorNE.r + 0.7152 * colorNE.g + 0.0722 * colorNE.b;
    float lumaSW = 0.2126 * colorSW.r + 0.7152 * colorSW.g + 0.0722 * colorSW.b;
    float lumaSE = 0.2126 * colorSE.r + 0.7152 * colorSE.g + 0.0722 * colorSE.b;

    // Calculate the luma difference between neighbors
    float lumaMin = min(min(lumaNW, lumaNE), min(lumaSW, lumaSE));
    float lumaMax = max(max(lumaNW, lumaNE), max(lumaSW, lumaSE));
    float lumaDiff = lumaMax - lumaMin;

    // Apply FXAA based on the edge detection and the luma difference
    if (lumaDiff < fxaaQualityEdgeThreshold) {
        return vec4(color, 1.0);  // No anti-aliasing if the difference is too small
    }

    // Compute the subpixel offset based on the luma difference and a quality factor
    vec2 dir = vec2(
        (lumaNW + lumaNE) - (lumaSW + lumaSE),
        (lumaNW + lumaSW) - (lumaNE + lumaSE)
    );
    float dirLength = length(dir);
    dir = normalize(dir);
    
    vec2 offset = dir * fxaaQualitySubpix * texel;

    // Return the color after applying the anti-aliasing filter
    return vec4(texture(tex, fragCoord + offset).rgb, 1.0);
}

void main() {
    FragColor = fxaa(sceneTexture, TexCoord, resolution);
}
