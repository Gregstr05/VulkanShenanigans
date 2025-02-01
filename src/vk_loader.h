#pragma once

#include <vk_types.h>
#include <unordered_map>
#include <filesystem>

struct GltfMaterial
{
    MaterialInstance material;
};

struct GeoSurface {
    uint32_t startIndex;
    uint32_t count;
    std::shared_ptr<GltfMaterial> material;
};

struct MeshAsset {
    std::string name;

    std::vector<GeoSurface> surfaces;
    GpuMeshBuffers meshBuffers;
};

//forward declaration
class VulkanEngine;

std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(VulkanEngine* engine, std::filesystem::path filePath);
