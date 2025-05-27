#version 460 core

out vec4 fragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

// Used to transform light position from world to view space.
uniform mat4 view;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emissive;
    float shininess;
    float emissiveStrength;
};

uniform Material material;

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
    
    float cutOff;
    float outerCutOff;
};

uniform SpotLight light;

void main()
{
    // Calculate ambient component.
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    
    // Calculate diffuse component.
    vec3 norm = normalize(normal);
    vec3 lightPosView = vec3(view * vec4(light.position, 1.0));
    // Points from cube to light source.
    vec3 lightDir = normalize(lightPosView - fragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
    
    // Calculate specular component.
    // Points from cube to viewer.
    vec3 viewDir = normalize(-fragPos); // Due to calculating lighting in view space, viewer is always at (0,0,0): viewDir = (0,0,0) - Position = -Position
    // reflect() expects the first argument to point towards the fragment position.
    vec3 reflectDir = reflect(-lightDir, norm);
    
    // Don't let shininess reach 0, since pow(0,0) is undefined behavior.
    float shininess = pow(max(dot(viewDir, reflectDir), 0.0), max(material.shininess, 0.1));
    vec3 specular = light.specular * shininess * vec3(texture(material.specular, texCoords));
    
    // Calculate emissive component.
    vec3 emissive = vec3(texture(material.emissive, texCoords)) * material.emissiveStrength;
    
    // Point Light - calculate and apply attenuation to all components.
    /*
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2.0));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    // We don't modify the emissive component, since that one emits light itself.
    */
    
    // Spot Light - Check if fragment is within cone.
    // Only transform the direction by the top 3x3 part of the view matrix, as this doesn't include translation.
    vec3 spotDirView = mat3(view) * light.direction;
    float theta = dot(lightDir, normalize(-spotDirView)); // Camera in view space points towards negative Z axis.
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Spot light illuminates fragment.
    if(theta > light.outerCutOff)
    {
        // Combine all lighting types (ambient, diffuse, specular, emissive). There you go, (modified) Phong lighting!
        // Ambient component will be left unaffected.
        diffuse *= intensity;
        specular *= intensity;
        vec3 result = ambient + diffuse + specular + emissive;
        fragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = ambient;
        fragColor = vec4(result, 1.0);
    }
}