#include "vulkanImageLoader.h"
#include "vulkanComp.h"

VulkanImageLoader::VulkanImageLoader(VulkanComp* vkComp) : mVkCompPtr(vkComp) {
}

VulkanImageLoader::~VulkanImageLoader() {
}

void VulkanImageLoader::LoadImageTexture(short imageWidth, short imageHeight, const std::string imageFilePath) {
    mImageFilePath = imageFilePath;
    VkDeviceSize size = imageWidth * imageHeight * 4;
    loadImageWidth = imageWidth;
    loadImageHeight = imageHeight;
    imageDataBuffer = new unsigned char[size];

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(mVkCompPtr->LogicalDevice(), &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(mVkCompPtr->LogicalDevice(), stagingBuffer, &memRequirements);

    mVkCompPtr->AllocateMemory(&stagingMemory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memRequirements);

    vkBindBufferMemory(mVkCompPtr->LogicalDevice(), stagingBuffer, stagingMemory, 0);

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.pNext = NULL;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = mPixelFormat;
    imageInfo.extent.width = imageWidth;
    imageInfo.extent.height = imageHeight;
    imageInfo.extent.depth = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.mipLevels = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.queueFamilyIndexCount = 0;
    imageInfo.pQueueFamilyIndices = nullptr;

    if (vkCreateImage(mVkCompPtr->LogicalDevice(), &imageInfo, NULL, &mImage) != VK_SUCCESS) {
        throw std::runtime_error("failed to create VKImage!");
    }

    VkMemoryRequirements memRequirementsImage;
    vkGetImageMemoryRequirements(mVkCompPtr->LogicalDevice(), mImage, &memRequirementsImage);

    mVkCompPtr->AllocateMemory(&imageMemory, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memRequirementsImage);

    vkBindImageMemory(mVkCompPtr->LogicalDevice(), mImage, imageMemory, 0);

    rawDataFile = fopen(imageFilePath.c_str(), "rb");
    fread(imageDataBuffer, size, 1, rawDataFile);

    void *mapData;
    vkMapMemory(mVkCompPtr->LogicalDevice(), stagingMemory, 0, size, 0, &mapData);
    memcpy(mapData, imageDataBuffer, imageWidth * imageHeight * 4);
    vkUnmapMemory(mVkCompPtr->LogicalDevice(), stagingMemory);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.pNext = nullptr;
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = mImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = mPixelFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(mVkCompPtr->LogicalDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate imageview failed!");
    }

    imageDesInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageDesInfo.imageView = textureImageView;

    //VkCommandBuffer transLayoutCommand = mVkCompPtr->CreatebeginSingleTimeCommands();

    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = mImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    //vkCmdPipelineBarrier(
    //    transLayoutCommand,
    //    sourceStage, destinationStage,
    //    0,
    //    0, nullptr,
    //    0, nullptr,
    //    1, &barrier
    //);

    //mVkCompPtr->EndSingleTimeCommands(transLayoutCommand);

    //VkCommandBuffer imageCopyCommand = mVkCompPtr->CreatebeginSingleTimeCommands();

    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        (unsigned int)imageWidth,
        (unsigned int)imageHeight,
        1
    };

    //vkCmdCopyBufferToImage(imageCopyCommand, stagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    //mVkCompPtr->EndSingleTimeCommands(imageCopyCommand);

    //vkDestroyBuffer(mVkCompPtr->LogicalDevice(), stagingBuffer, nullptr);
    //vkFreeMemory(mVkCompPtr->LogicalDevice(), stagingMemory, nullptr);
}

void VulkanImageLoader::LoadImageTexture(short imageWidth, short imageHeight, unsigned char* imageData, int pixelFormat) {

}

void VulkanImageLoader::ReadOneFrame() {
    if (rawDataFile) {
        int size = loadImageWidth * loadImageHeight * 4;
        auto read = fread(imageDataBuffer, size, 1, rawDataFile);
        if (read != 1) {
            fclose(rawDataFile);
            rawDataFile = fopen(mImageFilePath.c_str(), "rb");
        }

        void* mapData;
        vkMapMemory(mVkCompPtr->LogicalDevice(), stagingMemory, 0, size, 0, &mapData);
        memcpy(mapData, imageDataBuffer, size);
        vkUnmapMemory(mVkCompPtr->LogicalDevice(), stagingMemory);
    }
}

void VulkanImageLoader::RecordCommand(VkCommandBuffer commandBuffer) {
    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void VulkanImageLoader::BindTextureSampler(VkSampler textureSampler) {
    imageDesInfo.sampler = textureSampler;
}

void VulkanImageLoader::SetImagePixelFormat(VkFormat imageFmt) {
    mPixelFormat = imageFmt;
}