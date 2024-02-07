//
// Created by Sam on 2023-03-28.
//

#ifndef DOUGHNUT_RENDERER_H
#define DOUGHNUT_RENDERER_H

#include "preprocessor.h"
#include "typedefs.h"
#include "ecs/components/ui_state.h"
#include "render_state.h"
#include "io/resource_pool.h"

#include <thread>

//#define WIREFRAME_MODE
//#define INSTANCED_RENDERING

// TODO https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight

namespace dn {
    class Renderer {
    public:
        Renderer(const std::string &title, GLFWwindow *window);

        ~Renderer();

        double draw(const double &delta, EntityManagerSpec &ecs);

        void resetMesh();

    private:

        void createGraphicsPipeline();

        void createDescriptorSetLayout();

        // TODO Take out delta time
        void updateUniformBuffer(const double &delta, EntityManagerSpec &ecs);

        void createDescriptorPool();

        void createDescriptorSets();

        void initVulkan();

        void destroyVulkan();

        VkShaderModule createShaderModule(const std::vector<char> &code);

        void createCommandPool();

        // TODO void createTextureImage();

        void createSyncObjects();

        void recordCommandBuffer(EntityManagerSpec &ecs, VkCommandBuffer buffer, uint32_t imageIndex);

        void uploadRenderables(EntityManagerSpec &ecs);

        // return buffer to use
        void uploadSimplifiedMeshes(EntityManagerSpec &ecs);

        void destroyRenderables(EntityManagerSpec &ecs);

        void drawUi(EntityManagerSpec &ecs);

        RenderState state{};

        dn::Time lastTimestamp = now();
        std::thread simplifiedMeshAllocationThread{};

        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkDescriptorPool descriptorPool = nullptr;
        std::vector<VkDescriptorSet> descriptorSets{}; // Will be cleaned up with pool
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipeline graphicsPipeline = nullptr;
        VkCommandPool commandPool = nullptr;

        VkSemaphore imageAvailableSemaphore = nullptr;
        VkSemaphore renderFinishedSemaphore = nullptr;
        VkFence inFlightFence = nullptr;

        dn::ResourcePool<dn::Texture> mTexturePool{};
    };
};

#endif //DOUGHNUT_RENDERER_H
