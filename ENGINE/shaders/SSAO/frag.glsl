#version 330 core
in vec2 TexCoord;
in vec3 point;
out vec4 FragColor;

uniform sampler2D sceneTexture;  // The original rendered scene
uniform sampler2D depthMap;      // Depth buffer texture
uniform sampler2D normalMap;     // Normal buffer texture

uniform vec2 screenSize = vec2(1280.0, 720.0); // Screen resolution
uniform float radius = 1.0;                     // Sampling radius
uniform float bias = 0.025;                     // Depth bias
uniform int sampleCount = 32;                   // Number of samples
float strength = 2.0; // AO strength multiplier

vec3 getPosition(vec2 uv) {
    float depth = texture(depthMap, uv).r;
    vec3 ndc = vec3(uv * 2.0 - 1.0, depth * 2.0 - 1.0);
    return ndc;
}

vec3 getNormal(vec2 uv) {
    return texture(normalMap, uv).xyz * 2.0 - 1.0;
}

float calculateAO(vec2 uv, vec3 position, vec3 normal) {
    float occlusion = 0.0;
    for (int i = 0; i < sampleCount; i++) {
        float angle = (float(i) / float(sampleCount)) * 6.28318530718; // 2 * PI
        vec2 sampleOffset = vec2(cos(angle), sin(angle)) * radius;
        vec2 sampleUV = uv + sampleOffset / screenSize;

        vec3 samplePosition = getPosition(sampleUV);
        vec3 sampleNormal = getNormal(sampleUV);

        vec3 diff = samplePosition - position;
        float dist = length(diff);
        float influence = max(0.0, dot(normal, normalize(diff)) - bias);

        occlusion += influence / (1.0 + dist * dist);
    }
    return 1.0 - (occlusion / float(sampleCount));
}

void main() {
    vec3 position = getPosition(TexCoord);
    vec3 normal = getNormal(TexCoord);

    float ao = calculateAO(TexCoord, position, normal);
    ao = pow(ao, strength); // Apply strength (optional: use a power function for better control)

    vec3 sceneColor = texture(sceneTexture, TexCoord).rgb;

    FragColor = vec4(sceneColor * ao, 1.0);
}