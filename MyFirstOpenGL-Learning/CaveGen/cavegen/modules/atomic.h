#pragma once

#include <glm/glm.hpp>

struct Vec3Compare 
    {
        bool operator()(const glm::ivec3& lhs, const glm::ivec3& rhs) const 
            {
                if (lhs.x != rhs.x) return lhs.x < rhs.x;
                if (lhs.y != rhs.y) return lhs.y < rhs.y;
                return lhs.z < rhs.z;
            }
    };
