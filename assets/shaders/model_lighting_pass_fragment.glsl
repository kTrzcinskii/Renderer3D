#version 330 core

struct PointLight {
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
    float radius;
};

const int MAX_NR_POINT_LIGHTS = 256;
const vec4 FOG_COLOR = vec4(0.4, 0.4, 0.4, 1.0);

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform vec3 cameraPos;
uniform int nrPointLights;
uniform float ambientLevel;
uniform float fogMaxDist;
uniform bool useFog;

out vec4 FragColor;

void main()
{

    // Get data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float specular = texture(gAlbedoSpec, TexCoords).a;

    // In case when we use fog, we don't have any skybox - we just want the whole sky to have color of the fog
    if (normal == vec3(0,0,0) && useFog)
    {
        FragColor = FOG_COLOR;
        return;
    }

    // Calculate lighting
    // Ambient
    vec3 lighting = diffuse * ambientLevel;
    vec3 cameraDir = normalize(cameraPos - fragPos);
    for (int i = 0; i < nrPointLights; i++)
    {
        float distance = length(pointLights[i].position - fragPos);
        if (distance < pointLights[i].radius)
        {
            // Diffuse
            vec3 lightDir = normalize(pointLights[i].position - fragPos);
            vec3 diffuse = max(dot(normal, lightDir), 0.0) * diffuse * pointLights[i].color;
            // Specular
            vec3 halfwayDir = normalize(lightDir + cameraDir);
            float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
            vec3 specular = spec * specular * pointLights[i].color;
            // Attenuation
            float attenuation = 1.0 / (1.0 + pointLights[i].linear * distance + pointLights[i].quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += (diffuse + specular);
        }
    }
    vec4 colorWithLight = vec4(lighting, 1.0);

    if (useFog)
    {
        // Fog effect
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