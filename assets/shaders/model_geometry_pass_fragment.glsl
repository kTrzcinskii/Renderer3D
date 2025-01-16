#version 330 core

struct Material {
    sampler2D diffuse0;
    sampler2D specular0;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform Material material;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = texture(material.diffuse0, TexCoords).rgb;
    // Specular only need one value so we can store it like thir for better memory usage
    gAlbedoSpec.a = texture(material.specular0, TexCoords).r;
}