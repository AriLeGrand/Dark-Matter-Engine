#version 330 core 
in vec2 TexCoord;
in vec3 point;
out vec4 FragColor;


uniform sampler2D sceneTexture;  // The original rendered scene
uniform sampler2D depthMap;  // The original rendered scene
uniform sampler2D normalMap;  // The original rendered scene


// Pass these as uniforms from your application
uniform float near = 0.1f;  
uniform float far = 1000.0f;   

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC space (-1 to 1)
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    FragColor = texture(sceneTexture, TexCoord);


    // float depthValue = texture(depthMap, TexCoord).r;
    // float linearDepth = LinearizeDepth(depthValue) / far; // Normalize for visualization
    // FragColor = vec4(vec3(linearDepth), 1.0); // Convert depth to grayscale

    // // Sample the normal from the texture
    // vec3 normal = texture(normalMap, TexCoord).rgb;
    
    // // Convert from [-1,1] range to [0,1] for visualization
    // normal = normalize(normal) * 0.5 + 0.5;
    // FragColor = vec4(normal, 1.0);
}
