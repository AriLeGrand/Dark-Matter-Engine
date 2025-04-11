#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord;
out vec3 CurrentPosition;
out vec3 PreviousPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 prevModel;
uniform mat4 prevView;
//uniform mat4 prevProjection;

void main() {
    TexCoord = texCoord;

    // Current frame position
    vec4 worldPos = model * vec4(position, 1.0);
    vec4 viewPos = view * worldPos;
    vec4 projPos = projection * viewPos;

    gl_Position = projPos;
    CurrentPosition = projPos.xyz;

    // Previous frame position (with correct previous projection)
    vec4 prevWorldPos = prevModel * vec4(position, 1.0);
    vec4 prevViewPos = prevView * prevWorldPos;
    vec4 prevProjPos = projection * prevViewPos;

    PreviousPosition = prevProjPos.xyz;
}
