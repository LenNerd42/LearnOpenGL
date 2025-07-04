﻿#version 460 core

out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

struct Material
{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

#define NR_POINT_LIGHTS 4

// Used to transform light position from world to view space.
uniform mat4 view;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    // Calculate ambient component.
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));

    // Calculate diffuse component.
    vec3 lightDirView = mat3(view) * light.direction;
    // Points from cube to light source.
    vec3 lightDir = normalize(-lightDirView);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));

    // Calculate specular component.
    // reflect() expects the first argument to point towards the fragment position.
    vec3 reflectDir = reflect(-lightDir, normal);

    // Don't let shininess reach 0, since pow(0,0) is undefined behavior.
    float shininess = pow(max(dot(viewDir, reflectDir), 0.0), max(material.shininess, 0.1));
    vec3 specular = light.specular * shininess * vec3(texture(material.texture_specular1, texCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Calculate ambient component.
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));

    // Calculate diffuse component.
    vec3 lightPosView = vec3(view * vec4(light.position, 1.0));
    // Points from cube to light source.
    vec3 lightDir = normalize(lightPosView - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));

    // Calculate specular component.
    // reflect() expects the first argument to point towards the fragment position.
    vec3 reflectDir = reflect(-lightDir, normal);

    // Don't let shininess reach 0, since pow(0,0) is undefined behavior.
    float shininess = pow(max(dot(viewDir, reflectDir), 0.0), max(material.shininess, 0.1));
    vec3 specular = light.specular * shininess * vec3(texture(material.texture_specular1, texCoords));

    // Calculate and apply attenuation to all components.
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2.0));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Calculate ambient component.
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));

    // Calculate diffuse component.
    vec3 lightPosView = vec3(view * vec4(light.position, 1.0));
    // Points from cube to light source.
    vec3 lightDir = normalize(lightPosView - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));

    // Calculate specular component.
    // reflect() expects the first argument to point towards the fragment position.
    vec3 reflectDir = reflect(-lightDir, normal);

    // Don't let shininess reach 0, since pow(0,0) is undefined behavior.
    float shininess = pow(max(dot(viewDir, reflectDir), 0.0), max(material.shininess, 0.1));
    vec3 specular = light.specular * shininess * vec3(texture(material.texture_specular1, texCoords));

    // Calculate and apply attenuation to all components.
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2.0));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // Check if fragment is within outer cone.
    // Only transform the direction by the top 3x3 part of the view matrix, as this doesn't include translation.
    vec3 spotDirView = mat3(view) * light.direction;
    float theta = dot(lightDir, normalize(-spotDirView)); // Camera in view space points towards negative Z axis.
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Spot light illuminates fragment.
    if(theta > light.outerCutOff)
    {
        // Combine all lighting types (ambient, diffuse, specular). There you go, Phong lighting!
        // Ambient component will be left unaffected.
        diffuse *= intensity;
        specular *= intensity;

        return (ambient + diffuse + specular);
    }
    // Spot light doesn't illuminate fragment.
    else
    {
        return ambient;
    }
}

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(-fragPos); // Due to calculating lighting in view space, viewer is always at (0,0,0): viewDir = (0,0,0) - Position = -Position

    // Phase 1: Directional Light
    vec3 result = CalcDirLight(directionalLight, norm, viewDir);
    // Phase 2: Point Lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
    }
    // Phase 3: Spot Light
    result += CalcSpotLight(spotLight, norm, fragPos, viewDir);

    fragColor = vec4(result, 1.0);
}