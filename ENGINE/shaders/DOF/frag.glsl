#version 330 core
in vec2 TexCoord;
in vec3 point;
out vec4 FragColor;


uniform sampler2D sceneTexture;  // The original rendered scene
uniform sampler2D depthMap;      // Depth buffer texture
uniform sampler2D normalMap;  // The original rendered scene

// Camera and DoF parameters
// uniform float near;              // Near plane
// uniform float far;               // Far plane
// uniform float focalDistance;      // Distance where objects are sharp
// uniform float focalRange;         // Range around focalDistance that remains sharp

float near = 0.1f;
float far = 1000.0f;
float focalDistance = 5.00f;
float focalRange = 1.0f;


// Sampling offsets for blur
const vec2 offsets[9] = vec2[](
    vec2(-1,  1), vec2(0,  1), vec2(1,  1),
    vec2(-1,  0), vec2(0,  0), vec2(1,  0),
    vec2(-1, -1), vec2(0, -1), vec2(1, -1)
);

// Function to linearize depth from the depth buffer
float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Convert depth to NDC (-1 to 1)
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    // Get depth and linearize it
    float depth = texture(depthMap, TexCoord).r;
    float linearDepth = LinearizeDepth(depth);

    // Compute blur amount based on distance from focal plane
    float blurFactor = clamp(abs(linearDepth - focalDistance) / focalRange, 0.0, 1.0);
    blurFactor *= blurFactor; // Increase effect for stronger blur

    // Apply blur based on depth
    vec4 color = texture(sceneTexture, TexCoord);
    if (blurFactor > 0.0) {
        vec4 blurColor = vec4(0.0);
        float weight = 0.0;

        // Sample surrounding pixels for blur effect
        for (int i = 0; i < 9; i++) {
            vec2 sampleTexCoord = TexCoord + offsets[i] * blurFactor * 0.002; // Scale blur strength
            blurColor += texture(sceneTexture, sampleTexCoord);
            weight += 1.0;
        }

        blurColor /= weight; // Average color
        color = mix(color, blurColor, blurFactor); // Blend original and blurred image
    }

    FragColor = color;
    // FragColor = texture(sceneTexture, TexCoord);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
