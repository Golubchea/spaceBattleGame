#pragma once

namespace DepthShader {

constexpr const char *vertex_shader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
)";

constexpr const char *fragment_shader = R"(
#version 330 core

void main() {
    // Ничего не делаем, так как нам нужна только глубина
}
)";
} // namespace DepthShader

namespace PBRShader {

constexpr const char *vertex_shader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

constexpr const char *fragment_shader = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

// Свет
struct Light {
    vec3 position;
    vec3 color;
};
uniform Light light;

// Материал
struct Material {
    float metallic;    // Металличность [0, 1]
    float roughness;   // Шероховатость [0, 1]
    vec3 albedo;       // Цвет материала
};
uniform Material material;

// Камера
uniform vec3 viewPos;

// Ambient Occlusion
uniform float ao;

// Тени
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

// Функция для вычисления теней
float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Преобразуем координаты в пространство света
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Переводим в [0, 1]

    // Проверяем, находится ли фрагмент в тени
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main() {
    // Нормали и направления
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * light.color;

    // PBR (упрощённая модель)
    vec3 F0 = vec3(0.04); // Базовая отражательная способность
    F0 = mix(F0, material.albedo, material.metallic);

    vec3 Lo = vec3(0.0);
    vec3 kS = F0; // Отражение
    vec3 kD = (1.0 - kS) * (1.0 - material.metallic); // Диффузия

    // Освещение
    vec3 radiance = light.color;
    Lo += (kD * material.albedo / 3.14159 + specular) * radiance * diff;

    // Ambient
    vec3 ambient = vec3(0.03) * material.albedo * ao;

    // Тени
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = calculateShadow(fragPosLightSpace, norm, lightDir);
    vec3 lighting = ambient + (1.0 - shadow) * Lo;

    FragColor = vec4(lighting, 1.0);
}
)";

} // namespace PBRShader
