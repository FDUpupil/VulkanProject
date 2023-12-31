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

    void RunOrStop();
    void Reset();
    void SetFmt(VkFormat imageFmt);
    void SetRes(int imageWidth, int imageHeight);
    void CloseWnd();

    void SetInputFile(std::string imageFilePath);

private:
    void DrawFrame();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    std::string mInputSeqFile = "";

    VulkanInfo mVkInfo{};
    VulkanComp* pVkComp = nullptr;
    VulkanShaderSet* pVkShaderSet;
    VulkanRenderSet* pVkRenderSet;
    VulkanImageLoader* pVkImageLoader;

    std::vector<VkCommandBuffer> tComBuffers;
    uint32_t currentFrame = 0;
    uint32_t maxFrameCnt = 0;

    int mVideoImageWidth = 1280;
    int mVideoImageHeight = 720;

    bool mPlay = false;
    bool mHoldLoop = false;
};