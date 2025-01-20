#version 330 core

const vec4 FOG_COLOR = vec4(0.8, 0.8, 0.8, 1.0);

in vec3 fragPos;

uniform vec3 lightColor;
uniform float fogMaxDist;
uniform bool useFog;
uniform vec3 cameraPos;

out vec4 FragColor;

void main()
{
    vec4 colorWithLight = vec4(lightColor, 1.0);
    if (useFog)
    {
        float fogMinDist = 0.1;
        float dist = length(fragPos - cameraPos);
        float fogFactor = (fogMaxDist - dist) /
        (fogMaxDist - fogMinDist);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        FragColor = mix(FOG_COLOR, colorWithLight, fogFactor);
    }
    else
    {
        FragColor = colorWithLight;
    }
}