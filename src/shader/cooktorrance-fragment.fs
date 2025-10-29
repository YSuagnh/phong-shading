#version 330 core
in vec3 vNormal;
in vec3 vFragPos;
out vec4 FragColor;

uniform vec3 uLightPos;     // world-space light position
uniform vec3 uViewPos;      // world-space camera position
uniform vec3 uLightColor;   // light color (radiance)

// PBR material parameters
uniform vec3 uObjectColor;    // base albedo color (linear)
uniform float F0;    // [0,1]
uniform float uRoughness;   // [0,1]
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

vec3 fresnelSchlick(float cosTheta)
{
    // Use scalar F0 from CPU and lift to vec3 to avoid changing the app-side uniform type.
    vec3 F0vec = vec3(F0);
    return F0vec + (1.0 - F0vec) * pow(1.0 - cosTheta, 5.0);
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

    // Cook-Torrance BRDF components
    float D = DistributionGGX(N, H, a);
    float G = GeometrySmith(N, V, L, k);
    vec3  F = fresnelSchlick(cosTheta);

    float denom = max(4.0 * NdotV * NdotL, 1e-4);
    vec3 specular = (D * G * F) / denom;

    // Lambertian diffuse with energy conservation: kd = 1 - F
    vec3 kd = (vec3(1.0) - F);
    vec3 diffuse = kd * (1.0 / PI);

    // Lighting: no distance attenuation to keep parity with other shaders
    vec3 ambient = uAmbient * uLightColor;
    vec3 radiance = uLightColor * NdotL;
    vec3 Lo = (diffuse + specular) * radiance;

    vec3 color = (ambient + Lo) * uObjectColor;
    FragColor = vec4(color, 1.0);
}
