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
    sampler2D emission;
    float shininess;
};

uniform Material material;

struct Light
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{
    // Calculate ambient component.
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    
    // Calculate diffuse component.
    vec3 norm = normalize(normal);
    // Points from cube to light source.
    vec3 lightPosView = vec3(view * vec4(light.position, 1.0));
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
    vec3 emissive = vec3(texture(material.emission, texCoords));
    
    // Combine all lighting types (ambient, diffuse, specular, emissive). There you go, (modified) Phong lighting!
    vec3 result = ambient + diffuse + specular + emissive;
    fragColor = vec4(result, 1.0);
}