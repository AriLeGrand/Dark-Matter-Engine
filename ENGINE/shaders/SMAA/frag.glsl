#version 330 core
in vec2 TexCoord;
in vec3 point;
out vec4 FragColor;

uniform sampler2D sceneTexture;    // The original rendered scene
uniform sampler2D depthMap;       // Depth buffer texture
uniform sampler2D normalMap;      // Normal buffer texture
vec2 screenSize = vec2(1280, 720);          // The screen size (used for calculating pixel offsets)

const float edgeThreshold = 0.25; // Adjust for edge detection sensitivity

// SMAA edge detection kernel (simplified version)
const mat3 edgeDetectionKernel[3] = mat3[3](
    mat3(vec3(1.0, -2.0, 1.0), vec3(2.0, -4.0, 2.0), vec3(1.0, -2.0, 1.0)),
    mat3(vec3(1.0, -2.0, 1.0), vec3(2.0, -4.0, 2.0), vec3(1.0, -2.0, 1.0)),
    mat3(vec3(1.0, -2.0, 1.0), vec3(2.0, -4.0, 2.0), vec3(1.0, -2.0, 1.0))
);

// Calculate the screen-space derivatives
vec3 getScreenSpaceDerivative(vec2 uv, sampler2D tex, vec2 screenSize) {
    float offset = 1.0 / screenSize.x;
    return vec3(
        texture(tex, uv + vec2(offset, 0.0)).r - texture(tex, uv - vec2(offset, 0.0)).r,
        texture(tex, uv + vec2(0.0, offset)).r - texture(tex, uv - vec2(0.0, offset)).r,
        0.0
    );
}

// Basic edge detection using the depth and normal map (SMAA needs both)
bool detectEdge(vec2 texCoord, sampler2D depthMap, sampler2D normalMap) {
    vec3 currentNormal = texture(normalMap, texCoord).rgb;
    float currentDepth = texture(depthMap, texCoord).r;

    // Use screen-space derivatives to detect edges based on depth and normals
    vec3 dDepth = getScreenSpaceDerivative(texCoord, depthMap, screenSize);
    vec3 dNormal = getScreenSpaceDerivative(texCoord, normalMap, screenSize);

    // Basic edge detection logic using depth and normal changes
    if (length(dDepth) > edgeThreshold || length(dNormal) > edgeThreshold) {
        return true;
    }

    return false;
}

// Basic blend function for SMAA
vec4 blendEdges(vec2 texCoord, bool isEdge) {
    if (isEdge) {
        // Apply a simple blending algorithm
        vec4 originalColor = texture(sceneTexture, texCoord);
        vec4 edgeColor = vec4(0.0, 0.0, 0.0, 1.0);  // Simple edge color (could be more sophisticated)
        return mix(originalColor, edgeColor, 0.5); // Blend the original color with edge color
    }
    return texture(sceneTexture, texCoord);
}

void main() {
    bool isEdge = detectEdge(TexCoord, depthMap, normalMap);
    FragColor = blendEdges(TexCoord, isEdge);
}
