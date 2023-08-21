#pragma once
#include "vulkanBase.h"

class VulkanComp;

class VulkanImageLoader {
public:
	VulkanImageLoader() = default;
	VulkanImageLoader(VulkanComp* vkComp);
	~VulkanImageLoader();

	void LoadImageTexture(short imageWidth, short imageHeight, const std::string imageFilePath);
	void LoadImageTexture(short imageWidth, short imageHeight, unsigned char* imageData, int pixelFormat);
	void BindTextureSampler(VkSampler textureSampler);

	VkDescriptorImageInfo ImageDesInfo() {
		return imageDesInfo;
	}

private:
	VulkanComp* mVkCompPtr;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	VkImage mImage;
	VkImageView textureImageView;
	VkDeviceMemory imageMemory;
	unsigned char* imageDataBuffer = nullptr;

	VkDescriptorImageInfo imageDesInfo;
};