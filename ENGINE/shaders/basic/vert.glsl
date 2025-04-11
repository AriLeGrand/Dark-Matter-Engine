#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
out vec2 TexCoord;
out vec3 point;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    TexCoord = texCoord;
    gl_Position = projection * view * model * vec4(position, 1.0);
    point = vec4(projection * view * model * vec4(position, 1.0)).xyz;
}