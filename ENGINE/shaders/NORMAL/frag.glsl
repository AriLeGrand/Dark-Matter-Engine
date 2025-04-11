#version 330 core
in vec2 TexCoord;
in vec3 point;
out vec4 FragColor;

uniform sampler2D sceneTexture;  // The original rendered scene
uniform sampler2D depthMap;      // Depth buffer texture
uniform sampler2D normalMap;  // The original rendered scene

void main() {
    // Sample the normal from the texture
    vec3 normal = texture(normalMap, TexCoord).rgb;
    
    // Convert from [-1,1] range to [0,1] for visualization
    normal = normalize(normal) * 0.5 + 0.5;
    FragColor = vec4(normal, 1.0);
}
