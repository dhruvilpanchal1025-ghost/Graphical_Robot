#version 330 core
out vec4 FragColor;

in vec3 vNormal;    // from VS (view space)
in vec3 vPos;       // from VS (view space)

// parameters
uniform vec3 uBaseColor;      // robot/ground base color (diffuse)
uniform bool uUseLight;

// two lights
uniform vec3 uDirLightDir;    // directional (view space)
uniform vec3 uDirLightColor;
uniform vec3 uPointPos;       // point light position (view space)
uniform vec3 uPointColor;

// material
uniform float uShininess;     // ~32..128
uniform vec3  uAmbient;       // small ambient term

void main() {
    if (!uUseLight) {
        FragColor = vec4(uBaseColor, 1.0);
        return;
    }

    vec3 N = normalize(vNormal);
    vec3 V = normalize(-vPos);

    // Dir light
    vec3 L0 = normalize(uDirLightDir);
    vec3 H0 = normalize(L0 + V);
    float diff0 = max(dot(N, L0), 0.0);
    float spec0 = pow(max(dot(N, H0), 0.0), uShininess);

    // Point light
    vec3 L1 = normalize(uPointPos - vPos);
    vec3 H1 = normalize(L1 + V);
    float diff1 = max(dot(N, L1), 0.0);
    float spec1 = pow(max(dot(N, H1), 0.0), uShininess);

    vec3 diffuse  = uBaseColor * (uDirLightColor * diff0 + uPointColor * diff1);
    vec3 specular = vec3(1.0)   * (uDirLightColor * spec0 + uPointColor * spec1);

    vec3 color = uAmbient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}