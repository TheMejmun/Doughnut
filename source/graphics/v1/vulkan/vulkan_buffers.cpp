//
// Created by Sam on 2023-04-12.
//

#include "graphics/v1/vulkan/vulkan_buffers.h"
#include "graphics/v1/uniform_buffer_object.h"
#include "graphics/v1/vulkan/vulkan_memory.h"
#include "graphics/v1/vulkan/vulkan_devices.h"
#include "util/timer.h"
#include "io/logger.h"

using namespace Doughnut;
using namespace Doughnut::Graphics::Vk;

uint32_t Buffers::maxAllocations = 0, Buffers::currentAllocations = 0;

VkCommandBuffer Buffers::commandBuffer = nullptr; // Cleaned automatically by command pool clean.
VkBuffer Buffers::vertexBuffer[] = {nullptr, nullptr, nullptr};
uint32_t Buffers::vertexCount[] = {0, 0, 0};
VkBuffer Buffers::indexBuffer[] = {nullptr, nullptr, nullptr};
uint32_t Buffers::indexCount[] = {0, 0, 0};
uint32_t Buffers::meshBufferToUse = 0;

extern const uint32_t Buffers::UBO_BUFFER_COUNT = 2;
extern const uint32_t Buffers::DEFAULT_ALLOCATION_SIZE = FROM_MB(256); // 128MB is not enough

uint32_t Buffers::uniformBufferIndex = UBO_BUFFER_COUNT;

VkPhysicalDeviceMemoryProperties Buffers::memProperties{};

VkDeviceMemory Buffers::vertexBufferMemory[] = {nullptr, nullptr, nullptr};
VkDeviceMemory Buffers::indexBufferMemory[] = {nullptr, nullptr, nullptr};
std::vector<VkBuffer> Buffers::uniformBuffers{};
std::vector<VkDeviceMemory> Buffers::uniformBuffersMemory{};
std::vector<void *> Buffers::uniformBuffersMapped{};

VkQueue Buffers::transferQueue = nullptr;
VkCommandPool Buffers::transferCommandPool = nullptr;
VkCommandBuffer Buffers::transferCommandBuffer = nullptr; // Cleaned automatically by command pool clean.

VkFence Buffers::uploadFence = nullptr;
bool Buffers::waitingForFence = false;
std::vector<VkBuffer> stagingBuffersToDestroy{};
std::vector<VkDeviceMemory> stagingBufferMemoriesToDestroy{};
uint32_t indexCountToSet, vertexCountToSet;
uint32_t meshBufferIndexToSet;

void Buffers::create() {
    Log::i("Creating Buffers");

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(Devices::physical, &deviceProperties);

    Buffers::maxAllocations = deviceProperties.limits.maxMemoryAllocationCount;
    Log::v("Maximum memory allocation count:", Buffers::maxAllocations);

    vkGetPhysicalDeviceMemoryProperties(Devices::physical, &Buffers::memProperties);

    vkGetDeviceQueue(Devices::logical, Devices::queueFamilyIndices.transferFamily.value(), 0,
                     &Buffers::transferQueue);

    createTransferCommandPool();
    createVertexBuffer();
    createIndexBuffer();
    createUniformBuffers();
    createUploadFence();
}

void destroyStagingBuffers() {
    for (auto stagingBuffer: stagingBuffersToDestroy) {
        vkDestroyBuffer(Devices::logical, stagingBuffer, nullptr);
    }
    stagingBuffersToDestroy.clear();

    for (auto stagingBufferMemory: stagingBufferMemoriesToDestroy) {
        vkFreeMemory(Devices::logical, stagingBufferMemory, nullptr);
    }
    stagingBufferMemoriesToDestroy.clear();
}

void Buffers::destroy() {
    Log::i("Destroying Buffers");

    vkQueueWaitIdle(Buffers::transferQueue); // In case we are still uploading
    destroyStagingBuffers();

    vkDestroyFence(Devices::logical, Buffers::uploadFence, nullptr);

    for (size_t i = 0; i < UBO_BUFFER_COUNT; i++) {
        vkDestroyBuffer(Devices::logical, Buffers::uniformBuffers[i], nullptr);
        vkFreeMemory(Devices::logical, Buffers::uniformBuffersMemory[i], nullptr);
    }

    for (auto buffer: Buffers::vertexBuffer)
        vkDestroyBuffer(Devices::logical, buffer, nullptr);
    for (auto bufferMemory: Buffers::vertexBufferMemory)
        vkFreeMemory(Devices::logical, bufferMemory, nullptr);

    for (auto buffer: Buffers::indexBuffer)
        vkDestroyBuffer(Devices::logical, buffer, nullptr);
    for (auto bufferMemory: Buffers::indexBufferMemory)
        vkFreeMemory(Devices::logical, bufferMemory, nullptr);

    vkDestroyCommandPool(Devices::logical, Buffers::transferCommandPool, nullptr);
//    vkFreeCommandBuffers(Devices::logical, Buffers::transferCommandPool, 1, &Buffers::transferCommandBuffer);
}

void Buffers::uploadVertices(const std::vector<Vertex> &vertices, uint32_t bufferIndex) {
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                 &stagingBufferMemory);

    void *data;
    vkMapMemory(Devices::logical, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(Devices::logical, stagingBufferMemory);

    // +1 because -1 means that we are accessing a non-simplified buffer
    auto dstBuffer = Buffers::vertexBuffer[bufferIndex];
    copyBuffer(stagingBuffer, dstBuffer, bufferSize);

    // Cleanup
    vkDestroyBuffer(Devices::logical, stagingBuffer, nullptr);
    vkFreeMemory(Devices::logical, stagingBufferMemory, nullptr);

    // TODO += -> = revert
    Buffers::vertexCount[bufferIndex] = vertices.size();

    // TODO for now assume that initial buffer is only copied to once
//    Buffers::simplifiedMeshBuffersIndex = simplifiedIndex; TODO
}

void Buffers::uploadIndices(const std::vector<uint32_t> &indices, uint32_t bufferIndex) {
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                 &stagingBufferMemory);

    void *data;
    vkMapMemory(Devices::logical, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(Devices::logical, stagingBufferMemory);

    // +1 because -1 means that we are accessing a non-simplified buffer
    copyBuffer(stagingBuffer, Buffers::indexBuffer[bufferIndex], bufferSize);

    vkDestroyBuffer(Devices::logical, stagingBuffer, nullptr);
    vkFreeMemory(Devices::logical, stagingBufferMemory, nullptr);

    // TODO += -> = revert
    Buffers::indexCount[bufferIndex] = indices.size();

    // TODO for now assume that initial buffer is only copied to once
//    Buffers::simplifiedMeshBuffersIndex = simplifiedIndex; TODO
}

void Buffers::uploadMesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
                         bool parallel, uint32_t bufferIndex) {

    size_t vertexBufferSize = sizeof(Vertex) * vertices.size();
    size_t indexBufferSize = sizeof(uint32_t) * indices.size();
    VkDeviceSize bufferSize = vertexBufferSize + indexBufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer,
                 &stagingBufferMemory);

    // To staging buffer

    void *data;
    // Upload vertices
    vkMapMemory(Devices::logical, stagingBufferMemory, 0, vertexBufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t) vertexBufferSize);
    vkUnmapMemory(Devices::logical, stagingBufferMemory);
    // Upload indices
    vkMapMemory(Devices::logical, stagingBufferMemory, vertexBufferSize, indexBufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t) indexBufferSize);
    vkUnmapMemory(Devices::logical, stagingBufferMemory);

    // To final buffer

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(Buffers::transferCommandBuffer, &beginInfo);

    // Upload vertices
    auto dstBufferVertices = Buffers::vertexBuffer[bufferIndex];
    VkBufferCopy copyRegionVertices{};
    copyRegionVertices.srcOffset = 0; // Optional
    copyRegionVertices.dstOffset = 0; // Optional
    copyRegionVertices.size = vertexBufferSize; // VK_WHOLE_SIZE  not allowed here!
    vkCmdCopyBuffer(Buffers::transferCommandBuffer, stagingBuffer, dstBufferVertices, 1, &copyRegionVertices);

    // Upload indices
    auto dstBufferIndices = Buffers::indexBuffer[bufferIndex];
    VkBufferCopy copyRegionIndices{};
    copyRegionIndices.srcOffset = vertexBufferSize; // Optional
    copyRegionIndices.dstOffset = 0; // Optional
    copyRegionIndices.size = indexBufferSize; // VK_WHOLE_SIZE  not allowed here!
    vkCmdCopyBuffer(Buffers::transferCommandBuffer, stagingBuffer, dstBufferIndices, 1, &copyRegionIndices);

    vkEndCommandBuffer(Buffers::transferCommandBuffer);

    // Submit

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &Buffers::transferCommandBuffer;

    vkQueueSubmit(Buffers::transferQueue, 1, &submitInfo, uploadFence);

    // End
    stagingBuffersToDestroy.push_back(stagingBuffer);
    stagingBufferMemoriesToDestroy.push_back(stagingBufferMemory);
    waitingForFence = true;

    meshBufferIndexToSet = bufferIndex;
    indexCountToSet = indices.size();
    vertexCountToSet = vertices.size();
}

void Buffers::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, bool parallel) {
//    VkCommandBufferAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandPool = Buffers::transferCommandPool;
//    allocInfo.commandBufferCount = 1;
//    VkCommandBuffer transferBuffer;
//    vkAllocateCommandBuffers(logical, &allocInfo, &transferBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(Buffers::transferCommandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size; // VK_WHOLE_SIZE  not allowed here!
    vkCmdCopyBuffer(Buffers::transferCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(Buffers::transferCommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &Buffers::transferCommandBuffer;

    vkQueueSubmit(Buffers::transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(Buffers::transferQueue);

//    vkFreeCommandBuffers(logical, Buffers::transferCommandPool, 1, &Buffers::transferCommandBuffer);
}

void Buffers::waitForTransfer() {
    VkResult result = vkWaitForFences(Devices::logical, 1, &Buffers::uploadFence, true,
                                      30'000'000'000); // Wait for 30s max
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Waiting for the upload fence was unsuccessful");
    }
    finishTransfer();
}

void Buffers::finishTransfer() {
    vkResetFences(Devices::logical, 1, &Buffers::uploadFence);
    destroyStagingBuffers();
    waitingForFence = false;
    Buffers::meshBufferToUse = meshBufferIndexToSet;
    Buffers::indexCount[meshBufferIndexToSet] = indexCountToSet;
    Buffers::vertexCount[meshBufferIndexToSet] = vertexCountToSet;
}

bool Buffers::isTransferQueueReady() {
    if (!waitingForFence) return true;

    VkResult result = vkGetFenceStatus(Devices::logical, Buffers::uploadFence);
    if (result == VK_SUCCESS) {
        return true;
    } else if (result == VK_ERROR_DEVICE_LOST) {
        throw std::runtime_error("Device lost when checking fence");
    } else {
        return false;
    }
}

void Buffers::createUploadFence() {
    VkFenceCreateInfo createInfo{
            .sType=VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags=0
    };
    if (vkCreateFence(Devices::logical, &createInfo, nullptr, &Buffers::uploadFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create upload fence");
    }
}

void Buffers::destroyCommandBuffer(VkCommandPool commandPool) {
    vkFreeCommandBuffers(Devices::logical, commandPool, 1, &Buffers::commandBuffer);
}

void Buffers::nextUniformBuffer() {
    Buffers::uniformBufferIndex = (Buffers::uniformBufferIndex + 1) % UBO_BUFFER_COUNT;
}

void *Buffers::getCurrentUniformBufferMapping() {
    return Buffers::uniformBuffersMapped[Buffers::uniformBufferIndex];
}

VkBuffer Buffers::getCurrentUniformBuffer() {
    return Buffers::uniformBuffers[Buffers::uniformBufferIndex];
}

void Buffers::createVertexBuffer() {
    VkDeviceSize bufferSize = DEFAULT_ALLOCATION_SIZE;

    for (int i = 0; i < 3; ++i)
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffers::vertexBuffer + i,
                     Buffers::vertexBufferMemory + i);
}

void Buffers::createIndexBuffer() {
    VkDeviceSize bufferSize = DEFAULT_ALLOCATION_SIZE;

    for (int i = 0; i < 3; ++i)
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffers::indexBuffer + i,
                     Buffers::indexBufferMemory + i);
}

void Buffers::createUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    // One to be written, one to be read!
    Buffers::uniformBuffers.resize(UBO_BUFFER_COUNT);
    Buffers::uniformBuffersMemory.resize(UBO_BUFFER_COUNT);
    Buffers::uniformBuffersMapped.resize(UBO_BUFFER_COUNT);

    for (size_t i = 0; i < UBO_BUFFER_COUNT; i++) {
        // Instead of memcopy, because it is written to every frame!
        createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     &Buffers::uniformBuffers[i], &Buffers::uniformBuffersMemory[i]);

        // Persistent mapping:
        vkMapMemory(Devices::logical, Buffers::uniformBuffersMemory[i], 0, bufferSize, 0,
                    &Buffers::uniformBuffersMapped[i]);
    }
}

void Buffers::createCommandBuffer(VkCommandPool commandPool) {
    createCommandBuffer(commandPool, &Buffers::commandBuffer);
}

void Buffers::createCommandBuffer(VkCommandPool commandPool, VkCommandBuffer *pBuffer) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCommandPoolCreateFlagBits.html
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Can be submitted for execution, but not called from other command buffers
    // VK_COMMAND_BUFFER_LEVEL_SECONDARY can not be submitted, but called from other command buffers
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(Devices::logical, &allocInfo, pBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers!");
    }
}

void Buffers::createTransferCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // for vkResetCommandBuffer
    // Use VK_COMMAND_POOL_CREATE_TRANSIENT_BIT if buffer is very short-lived
    poolInfo.queueFamilyIndex = Devices::queueFamilyIndices.transferFamily.value();

    if (vkCreateCommandPool(Devices::logical, &poolInfo, nullptr, &Buffers::transferCommandPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }

    createCommandBuffer(Buffers::transferCommandPool, &Buffers::transferCommandBuffer);
}

void Buffers::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                           VkBuffer *pBuffer, VkDeviceMemory *pBufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage; // Can be and-ed with other use cases
    if (Devices::queueFamilyIndices.hasUniqueTransferQueue()) {
        bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT; // TODO Switch to memory barriers
        bufferInfo.queueFamilyIndexCount = 2;
        uint32_t queueIndices[] = {Devices::queueFamilyIndices.graphicsFamily.value(),
                                   Devices::queueFamilyIndices.transferFamily.value()};
        bufferInfo.pQueueFamilyIndices = queueIndices;
    } else {
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Like swap chain images
    }

    if (vkCreateBuffer(Devices::logical, &bufferInfo, nullptr, pBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(Devices::logical, *pBuffer, &memRequirements);

    // Malloc
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    // Is visible and coherent when viewing from host
    allocInfo.memoryTypeIndex = Memory::findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(Devices::logical, &allocInfo, nullptr, pBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate vertex buffer memory!");
    }

    // offset % memRequirements.alignment == 0
    vkBindBufferMemory(Devices::logical, *pBuffer, *pBufferMemory, 0);
}

void Buffers::resetMeshBufferToUse() {
    Buffers::meshBufferToUse = 0;
}