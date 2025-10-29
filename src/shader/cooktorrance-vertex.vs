#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 vNormal;
out vec3 vFragPos;

uniform mat4 uMVP;
uniform mat4 uModel;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    vFragPos = vec3(uModel * vec4(aPos, 1.0));
    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vNormal = normalize(normalMatrix * aNormal);
}
