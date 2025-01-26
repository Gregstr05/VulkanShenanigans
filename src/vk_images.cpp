#include <vk_images.h>
#include <vk_initializers.h>

void vkutil::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout,
        VkImageLayout newLayout)
{
        VkImageMemoryBarrier2 imgBarrier {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2};
        imgBarrier.pNext = nullptr;

        imgBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imgBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        imgBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imgBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        imgBarrier.oldLayout = currentLayout;
        imgBarrier.newLayout = newLayout;

        VkImageAspectFlags aspectMask = (newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        imgBarrier.subresourceRange = vkinit::image_subresource_range(aspectMask);
        imgBarrier.image = image;

        VkDependencyInfo dependencyInfo {};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependencyInfo.pNext = nullptr;

        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imgBarrier;

        vkCmdPipelineBarrier2(cmd, &dependencyInfo);

}