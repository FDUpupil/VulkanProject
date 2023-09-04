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

	void ReadOneFrame();
	void RecordCommand(VkCommandBuffer commandBuffer);

	void SetImagePixelFormat(VkFormat imageFmt);

	VkDescriptorImageInfo ImageDesInfo() {
		return imageDesInfo;
	}

private:
	VulkanComp* mVkCompPtr;

	int loadImageWidth;
	int loadImageHeight;
	VkFormat mPixelFormat = VK_FORMAT_B8G8R8A8_SRGB;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	VkImage mImage;
	VkImageView textureImageView;
	VkDeviceMemory imageMemory;
	unsigned char* imageDataBuffer = nullptr;
	std::string mImageFilePath = "";
	FILE* rawDataFile = nullptr;

	VkImageMemoryBarrier barrier{};
	VkBufferImageCopy region{};

	VkDescriptorImageInfo imageDesInfo;
};