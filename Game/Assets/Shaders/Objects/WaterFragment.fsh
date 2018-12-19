#version 330 core

in vec3 fPosition;
in vec3 fNormal;
in vec4 fColor;
in vec2 fTexCoord;

out vec4 FragColor;

struct Material {
vec3 ambient;
vec3 diffuse;
vec3 specular;
float shininess;
sampler2D ourTexture_0;
};

struct Light {
vec3 direction;

vec3 ambient;
vec3 diffuse;
vec3 specular;
};

uniform vec3 viewPos;
uniform Light light;
uniform Material material;

void main()
{
FragColor = texture(material.ourTexture_0, fTexCoord);
}
/*
void main()
{
Material material;
material.ambient = vec3(0.0,0.0,1.0);
material.diffuse = vec3(0.0,0.0,1.0);
material.specular = vec3(0.5,0.5,0.5);
material.shininess = 1;

// Ambient
vec3 ambient = light.ambient * material.ambient;

// Diffuse
vec3 lightDir = normalize(-light.direction);
float diff = max(dot(fNormal, lightDir), 0.0);
vec3 diffuse = light.diffuse * (diff * material.diffuse);

// Specular
vec3 viewDir = normalize(viewPos - fPosition);
vec3 reflectDir = reflect(-lightDir, fNormal);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
vec3 specular = light.specular * (spec * material.specular);

vec3 result = ambient + diffuse + specular;
FragColor = vec4(result, 1.0);
}*/