#pragma once

namespace ModelShader {

constexpr const char *phong_vertex_shader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

constexpr const char *phong_fragment_shader = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() {
    // Амбиентная составляющая
    vec3 ambient = light.ambient * material.ambient;

    // Диффузная составляющая
    vec3 norm = normalize(Normal);

 

    vec3 lightDir = normalize(light.position - FragPos);

 
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

     

    // Спекулярная составляющая
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    // Итоговый цвет
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";

} // namespace ModelShader
