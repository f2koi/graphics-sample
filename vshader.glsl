#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) uniform mat4 translation;
layout(location = 2) uniform vec3 color;

void main() {
    gl_Position = translation * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
