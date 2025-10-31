#version 330 core
in vec3 vNormal;
in vec3 vFragPos;
out vec4 FragColor;

uniform vec3 uLightPos;     // world-space light position
uniform vec3 uViewPos;      // world-space camera position
uniform vec3 uLightColor;   // light color (radiance)

// PBR material parameters
uniform vec3 uAlbedo;         // 表面反射率/基础颜色 (linear)
uniform vec3 F0;    // [0,1]
uniform float uRoughness;   // [0,1]
uniform float uMetallic;    // [0,1] 金属度：0=非金属，1=金属
uniform float uAmbient;   // 环境光强度

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / max(PI * denom * denom, 1e-7);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    return NdotV / max(NdotV * (1.0 - k) + k, 1e-7);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // Normalized inputs
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uViewPos - vFragPos);
    vec3 L = normalize(uLightPos - vFragPos);
    vec3 H = normalize(V + L);

    float a = uRoughness * uRoughness;
    float k = (uRoughness + 1.0) * (uRoughness + 1.0) / 8.0;
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float cosTheta = max(dot(H, V), 0.0); // Fresnel uses angle between V and H

    vec3 F0_calculated = mix(vec3(0.04), uAlbedo, uMetallic);

    // Cook-Torrance BRDF components
    float D = DistributionGGX(N, H, a);
    float G = GeometrySmith(N, V, L, k);
    vec3  F = fresnelSchlick(cosTheta, F0_calculated);

    float denom = max(4.0 * NdotV * NdotL, 1e-4);
    vec3 specular = (D * G * F) / denom;

    vec3 kd = (vec3(1.0) - F) * (1.0 - uMetallic);
    vec3 diffuse = kd * uAlbedo * (1.0 / PI);

    vec3 radiance = uLightColor * NdotL;
    vec3 Lo = (diffuse + specular) * radiance;
    
    vec3 ambient = uAmbient * uLightColor * mix(uAlbedo, uAlbedo * 0.5, uMetallic);

    vec3 color = ambient + Lo;
    FragColor = vec4(color, 1.0);
}
