#pragma once
#include <vector>
#include "transform.h"

namespace Engine {
    struct Vertex {
        glm::vec3 position;
    };

    class Mesh {
    public:
        Mesh() = default;
        Mesh(const std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) : vertices(vertices), indices(indices) {}
        
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices; 
    };
};