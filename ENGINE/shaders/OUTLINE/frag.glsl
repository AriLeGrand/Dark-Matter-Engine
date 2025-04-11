#version 330 core
in vec2 TexCoord;
in vec3 point;
out vec4 FragColor;

uniform sampler2D sceneTexture;  // The original rendered scene
uniform sampler2D depthMap;      // Depth buffer texture
uniform sampler2D normalMap;     // Normal buffer texture

uniform float edgeThreshold = 0.1; // Threshold for edge detection

void main() {
    vec3 normal = texture(normalMap, TexCoord).rgb;
    float depth = texture(depthMap, TexCoord).r;

    // Sample neighboring pixels
    vec2 offset = vec2(1.0 / textureSize(sceneTexture, 0).x, 1.0 / textureSize(sceneTexture, 0).y);
    vec3 normalLeft = texture(normalMap, TexCoord - vec2(offset.x, 0.0)).rgb;
    vec3 normalRight = texture(normalMap, TexCoord + vec2(offset.x, 0.0)).rgb;
    vec3 normalUp = texture(normalMap, TexCoord - vec2(0.0, offset.y)).rgb;
    vec3 normalDown = texture(normalMap, TexCoord + vec2(0.0, offset.y)).rgb;

    float depthLeft = texture(depthMap, TexCoord - vec2(offset.x, 0.0)).r;
    float depthRight = texture(depthMap, TexCoord + vec2(offset.x, 0.0)).r;
    float depthUp = texture(depthMap, TexCoord - vec2(0.0, offset.y)).r;
    float depthDown = texture(depthMap, TexCoord + vec2(0.0, offset.y)).r;

    // Calculate differences
    float normalDiff = max(max(distance(normal, normalLeft), distance(normal, normalRight)),
                           max(distance(normal, normalUp), distance(normal, normalDown)));
    float depthDiff = max(max(abs(depth - depthLeft), abs(depth - depthRight)),
                          max(abs(depth - depthUp), abs(depth - depthDown)));

    // Determine if the current pixel is an edge
    if (normalDiff > edgeThreshold || depthDiff > edgeThreshold) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Outline color (black)
    } else {
        FragColor = texture(sceneTexture, TexCoord); // Original scene color
    }
}
