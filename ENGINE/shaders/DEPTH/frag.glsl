#version 330 core 
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D depthMap;

// Pass these as uniforms from your application
float near = 0.1;  
float far = 5.0;   

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC space (-1 to 1)
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    float depthValue = texture(depthMap, TexCoords).r;
    float linearDepth = LinearizeDepth(depthValue) / far; // Normalize for visualization
    FragColor = vec4(vec3(linearDepth), 1.0); // Convert depth to grayscale
}
