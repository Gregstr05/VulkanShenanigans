#include <vk_pipelines.h>

#include <fstream>
#include <vk_initializers.h>

bool vkutil::load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule)
{
    // Open the file and seek to the end
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        return false;

    // calculate the size (in bytes) of the file by looking at the cursor position
    size_t fileSize = (size_t)file.tellg();

    // spirv expects the buffer to be on uint32, so make sure to reserve a int
    // vector big enough for the entire file
    std::vector<uint32_t> buffer(fileSize/sizeof(uint32_t));

    // seek to file start
    file.seekg(0);

    // load file into the created buffer
    file.read((char*)buffer.data(), fileSize);

    file.close();

    // Create a shader module
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.pNext = nullptr;

    // Multiply by the size of uint32_t to get size in bytes
    shaderModuleCreateInfo.codeSize = buffer.size() * sizeof(uint32_t);
    shaderModuleCreateInfo.pCode = buffer.data();

    // Sanity check
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        return false;
    }
    *outShaderModule = shaderModule;
    return true;
}