#version 330 core

struct Material {
    sampler2D diffuse0;
    sampler2D specular0;
};

in vec2 TexCoords;

uniform Material material;

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    FragColor = texture(material.diffuse0, TexCoords);
}