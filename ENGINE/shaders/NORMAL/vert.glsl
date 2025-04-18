#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
out vec2 TexCoord;
out vec3 point;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoord = texCoord;
    gl_Position = vec4(position, 0.0, 1.0);
    point = vec4(vec4(position, 0.0, 1.0)).xyz;
}
