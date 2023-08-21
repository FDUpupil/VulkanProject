#pragma once

#include "vulkanBase.h"
#include "vulkanComp.h"
#include "vkBuffer.h"
#include "vulkanShaderSet.h"
#include "vulkanRenderSet.h"
#include "vulkanImageLoader.h"

class VulkanMgr {
public:
    VulkanMgr();
    ~VulkanMgr();

    void StartVulkanContext(VulkanInfo vkInfo);
    void SetVulkanShader();
    void PrepareTexture();
    void ReadyToRender();

    void PresentLoop();

    void StopAndCleanVulkan();

private:
    void DrawFrame();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    VulkanInfo mVkInfo;
    VulkanComp mVkComp;
    VulkanShaderSet mVkShaderSet;
    VulkanRenderSet mVkRenderSet;
    VulkanImageLoader mVkImageLoader;

    std::vector<VkCommandBuffer> tComBuffers;
    uint32_t currentFrame = 0;
    uint32_t maxFrameCnt = 0;
};