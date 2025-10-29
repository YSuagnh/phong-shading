#version 330 core
in vec3 vNormal;
in vec3 vFragPos;
out vec4 FragColor;

uniform vec3 uLightPos;     // 世界空间光源位置
uniform vec3 uViewPos;      // 世界空间相机位置
uniform vec3 uLightColor;   // 光颜色
uniform vec3 uObjectColor;  // 物体基底颜色
uniform float uAmbientStrength;   // 环境光强度
uniform float uSpecularStrength;  // 高光强度系数
uniform float uShininess;         // 高光次幂

void main()
{
    // 法线与方向
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vFragPos);
    vec3 V = normalize(uViewPos - vFragPos);
    vec3 H = normalize(L + V); // Blinn-Phong 半角向量

    // 环境光
    vec3 ambient = uAmbientStrength * uLightColor;

    // 漫反射
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * uLightColor;

    // 高光（Blinn-Phong）
    float spec = pow(max(dot(N, H), 0.0), uShininess);
    vec3 specular = uSpecularStrength * spec * uLightColor;

    vec3 color = (ambient + diffuse + specular) * uObjectColor;
    FragColor = vec4(color, 1.0);
}
