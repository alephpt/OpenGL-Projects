#Shader Vertex
#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout (location = 2) in vec3 aNorm;

out vec4 vecColor;
out vec4 vecNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vecColor = vec4(aCol, 1.0);
    vecNormal = vec4(normalize(aNorm.xyz), 0.0);
};

#Shader Fragment
#version 420 core

in vec4 vecColor;
in vec4 vecNormal;

out vec4 color;

void main(){
vec4 vectorize = vecNormal + vecColor;
color = vecColor;
};
