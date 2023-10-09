#pragma once
#include "vulkanBase.h"

class VulkanComp;

class VulkanImageLoader {
public:
	VulkanImageLoader(VulkanComp* vkComp);
	~VulkanImageLoader();

	void LoadImageTexture(short imageWidth, short imageHeight, const std::string imageFilePath);
	void LoadImageTexture(short imageWidth, short imageHeight, unsigned char* imageData, int pixelFormat);
	void BindTextureSampler(int index, VkSampler textureSampler);

	void ReadOneFrame();
	void RecordCommand(VkCommandBuffer commandBuffer);

	void SetImagePixelFormat(VkFormat imageFmt);

	int CurrentPixelPlaneCnt();
	int BitPerPixel();
	std::string CurrentShaderSuffix();

	std::vector<VkDescriptorImageInfo> ImageDesInfos() {
		return imageDesInfos;
	}

private:
	VulkanComp* mVkCompPtr;

	int loadImageWidth;
	int loadImageHeight;
	VkFormat mPixelFormat = VK_FORMAT_R8G8B8A8_UNORM;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	VkImage mImage;
	VkImageView textureImageView;
	VkImageView textureImageViewU;
	VkImageView textureImageViewV;

	VkDeviceMemory imageMemory;
	unsigned char* imageDataBuffer = nullptr;
	std::string mImageFilePath = "";
	FILE* rawDataFile = nullptr;

	VkImageMemoryBarrier barrier{};
	std::vector <VkImageMemoryBarrier> barriers;
	std::vector <VkBufferImageCopy> regions;
	//VkBufferImageCopy region{};

	std::vector<VkDescriptorImageInfo> imageDesInfos;

	//Ê±Ðò¿ØÖÆ
	std::chrono::high_resolution_clock::time_point lastReadTime;
	int fps = 24;

	int mLoadFrameCnt = 0;
};