#Shader Vertex
#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec3 aNorm;

out vec4 vecColor;
out vec4 vecNormal;
out vec4 vecPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vecColor = vec4(aCol, 1.0);
    vecNormal = vec4(aNorm, 1.0);
    vecPosition = model * vec4(aPos, 1.0);
};

#Shader Fragment
#version 420 core

in vec4 vecColor;
in vec4 vecNormal;
in vec4 vecPosition;

uniform vec3 lightPos;

out vec4 color;

void main() {
    float ambientStrength = 0.6;

    vec3 lightDir = normalize(lightPos - vecPosition.xyz);
    vec3 normal = normalize(vecNormal.xyz);
    float diff = max(dot(normal, lightDir), 0.0);

    float specularStrength = 0.3;
    vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0) - vecPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    float distance = length(lightPos - vecPosition.xyz);
    float attenuation = 2.0 / (1.0 + 0.1 * distance + 0.01 * distance);
    attenuation = clamp(attenuation, 0.3, 1.0);

    color = vecColor;// * (ambientStrength + diff) * attenuation + vec4(specularStrength * spec) * attenuation;
};