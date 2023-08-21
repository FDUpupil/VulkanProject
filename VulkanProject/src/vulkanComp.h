#pragma once

#include "vulkanBase.h"

class VulkanComp {
public:
    VulkanComp();
    ~VulkanComp();

    void PrepareVulkan(bool needDebug);
    void CreateWindow(int width, int height, bool resize);
    void InitVulkanDevice();
    void CleanComponet();
    // ��Ⱦ
    void PrepareRenderOnScreen();

    //��������
    void InitInstance();
    void CreateSurface();
    void PickPhysicDevice();
    void CreateLogicalDevice();
    void CreateCommandPool();
    void CreateDescriptorPool();
    void PrepareSyncObjects();
 
    void CreateSwapchain();
    void CreateSwapChainImageViews();

    VkRenderPass CreateRenderPass(VkRenderPassCreateInfo renderPassInfo);
    VkPipeline CreateRnderPipeline(VkGraphicsPipelineCreateInfo pipelineInfo);
    VkShaderModule LoadShaderModule(const std::vector<char>& code);
    
    //�����������
    std::vector<VkCommandBuffer> CreateCommandBuffers();
    VkCommandBuffer CreatebeginSingleTimeCommands(uint32_t beginFlag = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

    VkSampler CreateSampler(uint32_t mipLevels);
    std::vector<VkFramebuffer> CreateRenderFrameBuffer(VkRenderPass renderPass);
    std::vector<VkDescriptorSet> CreateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout);
    void AllocateMemory(VkDeviceMemory* vkMemory, VkMemoryPropertyFlags flag, VkMemoryRequirements requirements);

    void DestoryInstance();

    VkDevice& LogicalDevice() { return mDevice; }
    VkFormat ScreenImageFormat() { return mSwapChainImageFormat; }
    VkExtent2D SwapChainExtent() { return mSwapChainExtent; }
    //ѭ����ʾ
    void ShowLoop(VlukanLoopFunc loopFunc);

    int GetSyncFrameIndex(int renderOptCnt);
    void SubmitCommand(VkCommandBuffer comBuffer, int renderOptCnt);
    
private:
    std::vector<const char*> getRequiredExtensions();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);
    
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    uint32_t findMemoryType(VkPhysicalDevice physicaldevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    
private:
    // Flag
    bool mDebug;
    //��Ⱦ�Ĵ���
    int windowWidth = 0;
    int windowHeight = 0;
    GLFWwindow* mWindow;
    //����vk���
    VkInstance mInstance;
    VkSurfaceKHR mSurface;

    std::vector<VkPhysicalDevice> mDevices;
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures mDeviceFeatures{};
    VkPhysicalDeviceProperties mDeviceProperties;

    QueueFamilyIndices mIndices;
    SwapChainSupportDetails mDetails;

    VkDevice mDevice;

    VkQueue mGraphicsQueue;
    VkQueue mPresentQueue;

    VkCommandPool mCommandPool;
    VkDescriptorPool mDescriptorPool;
    std::vector<VkCommandBuffer> mComBuffers;
    //������
    VkSwapchainKHR mSwapChain;
    std::vector<VkImage> mSwapChainImages;
    std::vector<VkImageView> mSwapChainImageViews;
    std::vector<VkFramebuffer> mSwapChainFramebuffers;
    VkFormat mSwapChainImageFormat;
    VkExtent2D mSwapChainExtent;
    //�������е�ͬ�����
    uint32_t imageIndex;
    std::vector<VkSemaphore> mImageOkSemaphores;
    std::vector<VkSemaphore> mRenderOkSemaphores;
    std::vector<VkFence> mInFlightFences;
    
    std::vector<const char*> deviceExtensions;

    VkSampler textureSampler = 0;
};