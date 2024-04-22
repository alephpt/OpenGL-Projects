#pragma once

#include <glm/glm.hpp>

#define FILL_MODE_STR(x)    \
    (x == FillMode::Solid ? "Solid" :               \
    (x == FillMode::Edges ? "Edges" :               \
    (x == FillMode::Tunnels ? "Tunnels" :           \
    (x == FillMode::Cells ? "Cells" : "Custom"))))  \

enum FillMode { 
    Solid, 
    Edges, 
    Tunnels, 
    Cells, 
    Custom 
};

struct Vec3Compare 
    {
        bool operator()(const glm::ivec3& lhs, const glm::ivec3& rhs) const 
            {
                if (lhs.x != rhs.x) return lhs.x < rhs.x;
                if (lhs.y != rhs.y) return lhs.y < rhs.y;
                return lhs.z < rhs.z;
            }
    };
