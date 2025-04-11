#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D motionTexture;

const int samples = 10; // Number of blur samples
const float blurScale = 0.01; // Blur intensity

void main() {
    vec2 motion = texture(motionTexture, TexCoords).xy;
    vec4 color = texture(sceneTexture, TexCoords);
    vec4 accumulatedColor = color;

    for (int i = 1; i < samples; i++) {
        float t = float(i) / float(samples);
        vec2 sampleCoord = TexCoords - motion * t * blurScale;
        accumulatedColor += texture(sceneTexture, sampleCoord);
    }

    FragColor = accumulatedColor / float(samples);
}
