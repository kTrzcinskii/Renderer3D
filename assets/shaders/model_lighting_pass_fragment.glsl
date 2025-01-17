#version 330 core

struct PointLight {
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
    float radius;
};

const int MAX_NR_POINT_LIGHTS = 256;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform vec3 cameraPos;
uniform int nrPointLights;

out vec4 FragColor;

void main()
{
    // Get data from gbuffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float specular = texture(gAlbedoSpec, TexCoords).a;

    // Calculate lighting
    // Ambient
    vec3 lighting = diffuse * 0.1;
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
    FragColor = vec4(lighting, 1.0);
}