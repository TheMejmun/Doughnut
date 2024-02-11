//
// Created by Sam on 2023-04-09.
//

#ifndef DOUGHNUT_VERTEX_H
#define DOUGHNUT_VERTEX_H

#include "preprocessor.h"
#include "graphics/colors.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
#include <array>

// https://vulkan-tutorial.com/Uniform_buffers/Descriptor_pool_and_sets
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec3 uvw;

    static vk::VertexInputBindingDescription getBindingDescription() {
        return {
                0,
                sizeof(Vertex),
                vk::VertexInputRate::eVertex
        };
    }

    static std::array<vk::VertexInputAttributeDescription, 6> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 6> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[3].offset = offsetof(Vertex, tangent);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[4].offset = offsetof(Vertex, bitangent);

        attributeDescriptions[5].binding = 0;
        attributeDescriptions[5].location = 5;
        attributeDescriptions[5].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[5].offset = offsetof(Vertex, uvw);

        return attributeDescriptions;
    }
};

#endif //DOUGHNUT_VERTEX_H
