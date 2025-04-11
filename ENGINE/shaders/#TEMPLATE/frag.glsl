#version 330 core
in vec2 TexCoord;
in vec3 point;
out vec4 FragColor;

uniform sampler2D sceneTexture;  // The original rendered scene
uniform sampler2D depthMap;      // Depth buffer texture
uniform sampler2D normalMap;  // The original rendered scene

void main() {
    FragColor = texture(sceneTexture, TexCoord);
}
