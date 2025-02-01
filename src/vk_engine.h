// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>

#include "vk_descriptors.h"
#include "vk_loader.h"

#pragma region Structs
struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()> && function)
	{
		deletors.push_back(function);
	}

	void flush()
	{
		// iterate in reverse to first destroy the newly created thing
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
		{
			// call the functions
			(*it)();
		}

		deletors.clear();
	}
};

struct FrameData
{
	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	VkSemaphore _swapchainSemaphore, _renderSemaphore;
	VkFence _renderFence;

	DeletionQueue _deletionQueue;
	DescriptorAllocatorGrowable _frameDescriptors;
};

struct ComputePushConstants
{
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect
{
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;

	ComputePushConstants pushConstants;
};

struct GpuSceneData
{
	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 viewProjection;
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection; // W for sun power (not in Watts)
	glm::vec4 sunlightColor;
};

struct GltfMetallic_Roughness
{
	MaterialPipeline opaquePipeline;
	MaterialPipeline transparentPipeline;

	VkDescriptorSetLayout materialLayout;

	struct MaterialConstants
	{
		glm::vec4 colorFactors;
		glm::vec4 metalRoughnessFactors;
		//padding, we need it anyway for uniform buffers
		glm::vec4 extra[14];
	};

	struct MaterialResources
	{
		AllocatedImage colorImage;
		VkSampler colorSampler;
		AllocatedImage metalRoughImage;
		VkSampler metalRoughSampler;
		VkBuffer dataBuffer;
		uint32_t dataBufferOffset;
	};

	DescriptorWriter writer;

	void build_pipelines(VulkanEngine* engine);
	void clear_resources(VkDevice device);

	MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator);
};

struct RenderObject
{
	uint32_t indexCount;
	uint32_t firstIndex;
	VkBuffer indexBuffer;

	MaterialInstance* material;

	glm::mat4 transform;
	VkDeviceAddress vertexBufferAddress;
};

#pragma endregion

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {

public:

#pragma region Shaders
	DescriptorAllocatorGrowable globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;
	
	VkDescriptorSetLayout _singleImageDescriptorLayout;

	GpuSceneData sceneData;
	VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

#pragma endregion

#pragma region Pipelines
	VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;

	VkPipelineLayout _meshPipelineLayout;
	VkPipeline _meshPipeline;
#pragma endregion

#pragma region ImmediateSubmit
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	void immediate_submit(std::function<void(VkCommandBuffer cmd)> && function);
#pragma endregion

#pragma region TestData
	std::vector<std::shared_ptr<MeshAsset>> testMeshes;

	AllocatedImage _whiteImage;
	AllocatedImage _blackImage;
	AllocatedImage _greyImage;
	AllocatedImage _errorCheckerboardImage;

	VkSampler _defaultSamplerLinear;
	VkSampler _defaultSamplerNearest;

	MaterialInstance defaultData;
	GltfMetallic_Roughness metalRoughMaterial;

#pragma endregion

	FrameData _frames[FRAME_OVERLAP];

	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	VmaAllocator _allocator;

	// Draw resources
	AllocatedImage _drawImage;
	AllocatedImage _depthImage;
	VkExtent2D _drawExtent;

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;

	DeletionQueue _mainDeletionQueue;

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
	bool resize_requested{ false };
	float renderScale{ 1.0f };
	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };

	static VulkanEngine& Get();

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();

#pragma region VulkanHandles
	VkInstance _instance;// Vulkan library handle
	VkDebugUtilsMessengerEXT _debug_messenger;// Vulkan debug output handle
	VkPhysicalDevice _chosenGPU;// GPU chosen as the default device
	VkDevice _device; // Vulkan device for commands
	VkSurfaceKHR _surface;// Vulkan window surface
#pragma endregion
#pragma region SwapChainHandles
	VkSwapchainKHR _swapChain;
	VkFormat _swapChainImageFormat;

	std::vector<VkImage> _swapChainImages;
	std::vector<VkImageView> _swapChainImageViews;
	VkExtent2D _swapChainExtent;
#pragma endregion

private:
	std::vector<ComputeEffect> backgroundsEffects;
	int currentBackgroundEffect{0};

	float rotation{0.f};
	float viewScale{1.f};

private:
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void init_descriptors();
	void init_pipelines();
	void init_default_data();

	void init_background_pipelines();
	void init_mesh_pipeline();
	void init_imgui();


	void CreateSwapchain(uint32_t width, uint32_t height);
	void ResizeSwapchain();
	void DestroySwapchain();

	AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	void DestroyBuffer(const AllocatedBuffer &buffer);

	AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	AllocatedImage CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	void DestroyImage(const AllocatedImage &img);

	void DrawBackground(VkCommandBuffer cmd);
	void DrawGeometry(VkCommandBuffer cmd);
	void DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView);

public:
	GpuMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);
};
