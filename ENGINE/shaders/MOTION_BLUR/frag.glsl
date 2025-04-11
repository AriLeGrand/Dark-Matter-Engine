#version 330 core
in vec2 TexCoord;
in vec3 CurrentPosition;
in vec3 PreviousPosition;

out vec4 FragColor;
out vec2 MotionVector; // Store motion vector in a framebuffer

uniform sampler2D sceneTexture;

void main() {
    FragColor = texture(sceneTexture, TexCoord);

    // Convert from clip space to NDC (divide by w)
    vec2 currentNDC = CurrentPosition.xy / CurrentPosition.z;
    vec2 previousNDC = PreviousPosition.xy / PreviousPosition.z;

    // Compute screen-space motion vector
    MotionVector = currentNDC - previousNDC;
}
