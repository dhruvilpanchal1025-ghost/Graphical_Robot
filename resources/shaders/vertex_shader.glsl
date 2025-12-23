#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vNormal;   // normal in view space
out vec3 vPos;      // position in view space

void main() {
    mat4 MV   = uView * uModel;
    mat3 NMat = mat3(transpose(inverse(MV)));
    vNormal   = normalize(NMat * aNormal);

    vec4 posV = MV * vec4(aPos, 1.0);
    vPos      = posV.xyz;

    gl_Position = uProj * posV;
}