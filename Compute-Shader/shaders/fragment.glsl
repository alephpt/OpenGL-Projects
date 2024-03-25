#version 330 core

in vec2 fragmentTexCoord;

uniform sampler2D apparition;

out vec4 color;

void main()
{
    vec4 pixel = texture(apparition, fragmentTexCoord);
    float intensity = 0.333 * (pixel.r + pixel.g + pixel.b);
    color = vec4(vec3(intensity), 1.0);
}