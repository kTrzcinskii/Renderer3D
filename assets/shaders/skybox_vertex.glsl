#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // We set z to w, so that at the end z component for skybox during depth testing will be
    // w/w = 1, so skybox will be rendered only if there is no other object visible before it
    gl_Position = pos.xyww;
}