#version 330 core

struct PointLight {
    vec3 position;
    vec3 color;
    float linear;
    float quadratic;
    float radius;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float linear;
    float quadratic;
    bool use;
};

const int MAX_NR_POINT_LIGHTS = 256;
const vec4 FOG_COLOR = vec4(0.8, 0.8, 0.8, 1.0);
const int MAX_NR_SPOT_LIGHTS = 16;
const vec3 SPOTLIGHT_COLOR = vec3(1.0, 1.0, 1.0);

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
uniform SpotLight spotLights[MAX_NR_POINT_LIGHTS];
uniform int nrSpotLights;

out vec4 FragColor;

// Helpers
vec3 calculatAmbientColor(vec3 diffuseColor, float ambientLevel);
vec3 calculatePointLightsColor(vec3 fragPos, vec3 normal, vec3 diffuse, float specular, vec3 cameraDir, PointLight pointLight);
vec3 calculateSpotlightColor(vec3 fragPos, vec3 normal, vec3 diffuse, float specular, vec3 cameraDir, SpotLight spotlight);
vec4 applyFogEffect(vec4 finalColor, vec3 fragPos, vec3 cameraPos, float fogMaxDist);

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

    vec3 cameraDir = normalize(cameraPos - fragPos);

    // Calculate lighting effect
    vec3 ambientColor = calculatAmbientColor(diffuse, ambientLevel);

    // Pointlights
    vec3 pointLightsColor = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < nrPointLights; i++)
    {
        pointLightsColor += calculatePointLightsColor(fragPos, normal, diffuse, specular, cameraDir, pointLights[i]);
    }

    // Spotlights
    vec3 spotlightsColor = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < nrSpotLights; i++)
    {
        spotlightsColor += calculateSpotlightColor(fragPos, normal, diffuse, specular, cameraDir, spotLights[i]);
    }

    // Combine all lights
    vec4 finalColor = vec4(ambientColor + pointLightsColor + spotlightsColor, 1.0);

    if (useFog)
    {
        FragColor = applyFogEffect(finalColor, fragPos, cameraPos, fogMaxDist);
    }
    else
    {
        FragColor = finalColor;
    }
}

vec3 calculatAmbientColor(vec3 diffuseColor, float ambientLevel)
{
    return diffuseColor * ambientLevel;
}

vec3 calculatePointLightsColor(vec3 fragPos, vec3 normal, vec3 diffuse, float specular, vec3 cameraDir, PointLight pointLight)
{
    vec3 pointLightsColor = vec3(0.0, 0.0, 0.0);
    float dist = length(pointLight.position - fragPos);
    if (dist < pointLight.radius)
    {
        // Diffuse
        vec3 lightDir = normalize(pointLight.position - fragPos);
        vec3 diffuseCol = max(dot(normal, lightDir), 0.0) * diffuse * pointLight.color;
        // Specular
        vec3 halfwayDir = normalize(lightDir + cameraDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
        vec3 specularCol = spec * specular * pointLight.color;
        // Attenuation
        float attenuation = 1.0 / (1.0 + pointLight.linear * dist + pointLight.quadratic * dist * dist);
        // Result
        diffuseCol *= attenuation;
        specularCol *= attenuation;
        pointLightsColor += (diffuseCol + specularCol);
    }
    return pointLightsColor;
}

vec3 calculateSpotlightColor(vec3 fragPos, vec3 normal, vec3 diffuse, float specular, vec3 cameraDir, SpotLight spotlight)
{
    if (!spotlight.use)
    {
        return vec3(0.0, 0.0, 0.0);
    }
    float dist = length(spotlight.position - fragPos);
    // Diffuse
    vec3 lightDir = normalize(spotlight.position - fragPos);
    vec3 diffuseCol = max(dot(normal, lightDir), 0.0) * diffuse * SPOTLIGHT_COLOR;
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 32.0);
    vec3 specularCol = spec * specular * SPOTLIGHT_COLOR;
    // Attenuation
    float attenuation = 1.0 / (1.0 + spotlight.linear * dist + spotlight.quadratic * dist * dist);
    // Intensity
    float theta = dot(lightDir, normalize(-spotlight.direction));
    float epsilon = spotlight.cutOff - spotlight.outerCutOff;
    float intensity = clamp((theta - spotlight.outerCutOff) / epsilon, 0.0, 1.0);
    diffuseCol *= (attenuation * intensity);
    specularCol *= (attenuation * intensity);
    return diffuseCol + specularCol;
}

vec4 applyFogEffect(vec4 finalColor, vec3 fragPos, vec3 cameraPos, float fogMaxDist)
{
    float fogMinDist = 0.1;
    float dist = length(fragPos - cameraPos);
    float fogFactor = (fogMaxDist - dist) /
    (fogMaxDist - fogMinDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return mix(FOG_COLOR, finalColor, fogFactor);
}