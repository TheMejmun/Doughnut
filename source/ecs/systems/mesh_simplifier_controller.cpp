//
// Created by Saman on 27.08.23.
//

//#include "graphics/v1/vulkan/vulkan_swapchain.h"
//#include "ecs/systems/mesh_simplifier_controller.h"
//#include "ecs/entities/camera.h"
//#include "util/timer.h"
//#include "util/performance_logging.h"
//#include "util/os.h"
//#include "io/logger.h"
//
//#include <thread>
//#include <limits>
//#include <unordered_set>
//#include <algorithm>
//#include <execution>
//
////#define OUTPUT_MAPPINGS
//
//// This seems to universally be slower when enabled
////#define CACHE_LOCALITY_MODE
//
//constexpr uint32_t MAX_PIXELS_PER_VERTEX = 1;
//constexpr uint32_t MAX_INDEX = std::numeric_limits<uint32_t>::max();
//
//std::thread thread;
//uint32_t simplifiedMeshCalculationThreadFrameCounter = 0;
//dn::Time simplifiedMeshCalculationThreadStartedTime{};
//bool meshCalculationDone = false;
//
//struct SVO { // Simplification Vertex Object
//    bool set = false;
//    uint32_t index = 0;
//    uint32_t rasterIndex = 0;
//    float depth = 0.0f;
//};
//
//inline float distance2(const glm::vec3 &a, const glm::vec3 &b) {
//    return (a.x - b.x) * (a.x - b.x) +
//           (a.y - b.y) * (a.y - b.y) +
//           (a.z - b.z) * (a.z - b.z);
//}
//
//struct Triangle {
//    uint32_t id1;
//    uint32_t id2;
//    uint32_t id3;
//
//    // This function is used by unordered_set to compare
//    bool operator==(const Triangle &other) const {
//        return (this->id1 == other.id1 &&
//                this->id2 == other.id2 &&
//                this->id3 == other.id3);
//    }
//
//    // This function is used by unordered_set to hash
//    // If the IDs get too high, we may get collisions
//    size_t operator()(const Triangle &triangle) const noexcept {
//        uint32_t maxBytes = sizeof(size_t);
//        uint32_t allowedBytesPerId = maxBytes / 3;
//        uint32_t allowedBitsPerId = CHAR_BIT * allowedBytesPerId;
//
//        size_t hash = (((triangle.id1
//                << allowedBitsPerId) + triangle.id2)
//                << allowedBitsPerId) + triangle.id3;
//        return hash;
//    };
//
//    // compare for order.
//    bool operator<(const Triangle &rhs) const {
//        uint32_t maxBytes = sizeof(size_t);
//        uint32_t allowedBytesPerId = maxBytes / 3;
//        uint32_t allowedBitsPerId = CHAR_BIT * allowedBytesPerId;
//
//        size_t lhsHash = (((this->id1
//                << allowedBitsPerId) + this->id2)
//                << allowedBitsPerId) + this->id3;
//        size_t rhsHash = (((rhs.id1
//                << allowedBitsPerId) + rhs.id2)
//                << allowedBitsPerId) + rhs.id3;
//
//        return lhsHash < rhsHash;
//    }
//
//};
//
//// Change the triangle to start with the lowest index, but retain the face direction
//Triangle makeOrientedTriangle(const uint32_t id1, const uint32_t id2, const uint32_t id3) {
//    if (id1 < id2 && id1 < id3) {
//        // id1 is smallest
//        return {id1, id2, id3};
//    } else if (id2 < id1 && id2 < id3) {
//        // id2 is smallest
//        return {id2, id3, id1};
//    } else {
//        // id3 is smallest
//        return {id3, id1, id2};
//    }
//}
//
//class IndexLut {
//public:
//    uint32_t getMapping(uint32_t forIndex) {
//        uint32_t found = forIndex;
//        while (found != MAX_INDEX && this->indexMappings[found] != 0) {
//            // because the stored mappings are +1
//            found = this->indexMappings[found] == MAX_INDEX ? MAX_INDEX : this->indexMappings[found] - 1;
//        }
//        return found;
//    }
//
//    void insertMapping(uint32_t from, uint32_t to) {
//        this->indexMappings[from] = to == MAX_INDEX ? MAX_INDEX : to + 1;
//    }
//
//    bool isSet(uint32_t index) {
//        return this->indexMappings[index] != 0;
//    }
//
//    void resize(size_t size) {
//        this->indexMappings.resize(size);
//    }
//
//private:
//    // Treat 0 as not set -> all ids += 1
//    std::vector<uint32_t> indexMappings{};
//};
//
//std::vector<uint32_t> makeIndexRange(uint32_t untilInclusive) {
//    std::vector<uint32_t> out{};
//    out.reserve(untilInclusive + 1);
//    for (int i = 0; i <= untilInclusive; ++i) {
//        out.emplace_back(i);
//    }
//    return out;
//}
//
//#define insertOutput(id) \
//    if (usedVertexIndexMappings[id] == MAX_INDEX) { \
//        usedVertexIndexMappings[id] = to.vertices.size(); \
//        to.indices.push_back(to.vertices.size()); \
//        to.vertices.emplace_back(from.vertices[id]); \
//    } else { \
//        to.indices.push_back(usedVertexIndexMappings[id]); \
//    }
//
//
//void simplify(const Projector &cameraProjector,
//              const dn::Transform &cameraTransform,
//              const RenderMesh &from,
//              RenderMeshSimplifiable &to,
//              const dn::Transform &transform) {
//    // Init
//    const auto model = transform.forward;
//    const auto normalModel = glm::transpose(transform.inverse);
//    const auto cameraPos = cameraTransform.getPosition();
//    const auto view = cameraProjector.getView(cameraTransform);
//    const auto proj = cameraProjector.getProjection(dn::vulkan::Swapchain::aspectRatio);
//    const auto viewProj = proj * view;
//
//    to.vertices.clear();
//    to.indices.clear();
//
//    const uint32_t rasterWidth = dn::vulkan::Swapchain::framebufferWidth / MAX_PIXELS_PER_VERTEX;
//    const uint32_t rasterHeight = dn::vulkan::Swapchain::framebufferHeight / MAX_PIXELS_PER_VERTEX;
//
//    std::vector<SVO> svos{};
//    svos.resize(from.vertices.size());
//
//    // Pre-compute raster positions
//    const auto positionCalcLambda = [&](const uint32_t index) {
//        const Vertex &vertex = from.vertices[index];
//        const glm::vec4 worldPos = model * glm::vec4(vertex.pos, 1.0f);
//
//        // Is facing away from camera
//        if (glm::dot(glm::vec4(cameraPos, 1.0f) - worldPos,
//                     normalModel * glm::vec4(vertex.normal, 1.0f)) < 0) {
//            return;
//        }
//
//        const glm::vec4 projectedPos = viewProj * worldPos;
//        const long x = lroundf((projectedPos.x * 0.5f / projectedPos.w + 0.5f) * static_cast<float>(rasterWidth));
//        const long y = lroundf((projectedPos.y * 0.5f / projectedPos.w + 0.5f) * static_cast<float>(rasterHeight));
//
//        if (x < 0 || x >= rasterWidth || y < 0 || y >= rasterHeight) {
//            return;
//        }
//
//        const float depth = projectedPos.z / projectedPos.w;
//        const uint32_t rasterIndex = y * rasterWidth + x;
//
//        svos[index] = {
//                .set = true,
//                .index = index,
//                .rasterIndex = rasterIndex,
//                .depth = depth
//        };
//    };
//
//    // PARALLELLLLLL
//    const auto indices = makeIndexRange(from.vertices.size() - 1);
//    // TODO restore parallelism
//    // std::execution::par not available on macOS
//    std::for_each(
//#ifndef OS_MAC
//            std::execution::par,
//#endif
//            indices.begin(),
//            indices.end(),
//            positionCalcLambda);
//
//    std::vector<SVO> indicesRaster{};
//    indicesRaster.resize(rasterWidth * rasterHeight);
//    dn::log::d("Using raster", rasterWidth, "*", rasterHeight, "for mesh simplification");
//    IndexLut lut{};
//    lut.resize(from.vertices.size());
//    uint32_t newVertexCount = 0;
//
//    // Insert into raster
//    for (uint32_t i = 0; i < svos.size(); ++i) {
//        const auto &svo = svos[i];
//        if (!svo.set) {
//            lut.insertMapping(i, MAX_INDEX); // Map previously stored to the current vertex
//            continue;
//        }
//
//        if (!indicesRaster[svo.rasterIndex].set) {
//            ++newVertexCount;
//            indicesRaster[svo.rasterIndex] = svo;
//        } else if (indicesRaster[svo.rasterIndex].depth > svo.depth) {
//            // stored vertex is farther away
//            lut.insertMapping(indicesRaster[svo.rasterIndex].index, i); // Map previously stored to the current vertex
//            indicesRaster[svo.rasterIndex] = svo;
//        } else {
//            // stored vertex is closer to the camera than the current
//            lut.insertMapping(i, indicesRaster[svo.rasterIndex].index); // Map previously stored to the current vertex
//        }
//    }
//
//    // Filter triangles
//    std::unordered_set<Triangle, Triangle> triangles{}; // Ordered set
//    for (uint32_t i = 0; i < from.indices.size(); i += 3) {
//        const uint32_t id1 = lut.getMapping(from.indices[i]);
//        const uint32_t id2 = lut.getMapping(from.indices[i + 1]);
//        const uint32_t id3 = lut.getMapping(from.indices[i + 2]);
//
//        if (id1 == MAX_INDEX || id2 == MAX_INDEX || id3 == MAX_INDEX ||
//            id1 == id2 || id1 == id3 || id2 == id3)
//            continue;
//
//        triangles.insert(makeOrientedTriangle(id1, id2, id3));
//    }
//
//    // Push
//#ifdef CACHE_LOCALITY_MODE
//    to.indices.reserve(triangles.size() * 3);
//    to.vertices.reserve(to.indices.size()); // Duplicate vertices for good cache locality
//
//    for (const auto &[id1, id2, id3]: triangles) {
//        to.indices.push_back(to.vertices.size());
//        to.vertices.push_back(from.vertices[id1]);
//        to.indices.push_back(to.vertices.size());
//        to.vertices.push_back(from.vertices[id2]);
//        to.indices.push_back(to.vertices.size());
//        to.vertices.push_back(from.vertices[id3]);
//    }
//
//#else
//    std::vector<uint32_t> usedVertexIndexMappings;
//    usedVertexIndexMappings.resize(from.vertices.size(), MAX_INDEX);
//
//    to.indices.reserve(triangles.size() * 3);
//    to.vertices.reserve(newVertexCount);
//
//    for (const auto &[id1, id2, id3]: triangles) {
//        insertOutput(id1)
//        insertOutput(id2)
//        insertOutput(id3)
//    }
//#endif
//}
//
//void MeshSimplifierController::update(double delta, EntityManagerSpec &entityManager) {
//   const auto uiState = entityManager.template getArchetype<UiState>()[0].components;
//
//    if (thread.joinable()) {
//        simplifiedMeshCalculationThreadFrameCounter++;
//        if (meshCalculationDone) {
//            dn::log::d("Mesh calculation thread took", simplifiedMeshCalculationThreadFrameCounter, "frames");
//            thread.join();
//            uiState->meshSimplifierTimeTaken = dn::duration(simplifiedMeshCalculationThreadStartedTime, dn::now());
//            uiState->meshSimplifierFramesTaken = simplifiedMeshCalculationThreadFrameCounter;
//        }
//    } else if (uiState->runMeshSimplifier) {
//        auto entities = entityManager.getArchetype<RenderMesh, RenderMeshSimplifiable, dn::Transform>();
//        auto cameras = entityManager.getArchetype<Projector, dn::Transform>();
//        uint32_t mainCameraIndex;
//        for (uint32_t i = 0; i < cameras.size(); ++i) {
//            if (cameras[i].get<Projector>()->isMainCamera) {
//                mainCameraIndex = i;
//                break;
//            }
//        }
//
//        if (!entities.empty()) {
//            meshCalculationDone = false;
//            simplifiedMeshCalculationThreadFrameCounter = 0;
//            simplifiedMeshCalculationThreadStartedTime = dn::now();
//
//            auto function = [=](bool &done) {
//                for (const auto & entity : entities) {
//                    const auto renderMesh = entity.get<RenderMesh>();
//                    const auto renderMeshSimplifiable =entity.get<RenderMeshSimplifiable>();
//                    const auto transformer = entity.get<dn::Transform>();
//                    if (renderMeshSimplifiable->simplifiedMeshMutex->try_lock()) {
//                        PerformanceLogging::meshCalculationStarted();
//                        simplify(
//                                *cameras[mainCameraIndex].get<Projector>(),
//                                *cameras[mainCameraIndex].get<dn::Transform>(),
//                                *renderMesh,
//                                *renderMeshSimplifiable,
//                                *transformer
//                        );
//                        renderMeshSimplifiable->updateSimplifiedMesh = true;
//                        PerformanceLogging::meshCalculationFinished();
//                        renderMeshSimplifiable->simplifiedMeshMutex->unlock();
//                    }
//                }
//                done = true;
//            };
//
//            thread = std::thread(function, std::ref(meshCalculationDone));
//        }
//    }
//}
//
//MeshSimplifierController::~MeshSimplifierController() noexcept {
//    if (thread.joinable())
//        thread.join();
//}