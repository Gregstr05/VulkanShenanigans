// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>

#include "vk_descriptors.h"

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

#pragma endregion

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {

public:

#pragma region Shaders
	DescriptorAllocator globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

#pragma endregion

#pragma region Pipelines
	VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;
#pragma endregion

#pragma region ImmediateSubmit
	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	void immediate_submit(std::function<void(VkCommandBuffer cmd)> && function);
#pragma endregion

	FrameData _frames[FRAME_OVERLAP];

	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };

	VmaAllocator _allocator;

	// Draw resources
	AllocatedImage _drawImage;
	VkExtent2D _drawExtent;

	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;

	DeletionQueue _mainDeletionQueue;

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool stop_rendering{ false };
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

private:
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void init_descriptors();
	void init_pipelines();

	void init_background_pipelines();
	void init_imgui();

	void CreateSwapchain(uint32_t width, uint32_t height);
	void DestroySwapchain();

	void DrawBackground(VkCommandBuffer cmd);
	void DrawImGui(VkCommandBuffer cmd, VkImageView targetImageView);
};
