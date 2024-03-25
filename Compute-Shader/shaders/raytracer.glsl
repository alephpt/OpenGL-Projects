#version 430

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D apparition;

void main() {
    // Orientation
    ivec2 pixel_position = ivec2(gl_GlobalInvocationID.xy);

    // Write a Pixel
}