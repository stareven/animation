#version 410 core

layout(location=0) in vec2 vPos;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vPos, 0, 1);
    //gl_Position = vec4(vPos, 0, 1);
    //fColor = vColor;
}
