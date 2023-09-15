#include "vulkanImageLoader.h"
#include "vulkanComp.h"

VulkanImageLoader::VulkanImageLoader(VulkanComp* vkComp) : mVkCompPtr(vkComp) {
}

VulkanImageLoader::~VulkanImageLoader() {
}

void VulkanImageLoader::LoadImageTexture(short imageWidth, short imageHeight, const std::string imageFilePath) {
    mImageFilePath = imageFilePath;
    int bpp = BitPerPixel();
    VkDeviceSize size = (imageWidth * imageHeight * bpp) >> 3;
    loadImageWidth = imageWidth;
    loadImageHeight = imageHeight;
    imageDataBuffer = new unsigned char[size];

    int pixelPlaneCnt = CurrentPixelPlaneCnt();
    imageDesInfos.clear();
    regions.clear();

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

    if (rawDataFile) {
        fclose(rawDataFile);
        rawDataFile = nullptr;
    }

    rawDataFile = fopen(imageFilePath.c_str(), "rb");
    fread(imageDataBuffer, size, 1, rawDataFile);

    void *mapData;
    vkMapMemory(mVkCompPtr->LogicalDevice(), stagingMemory, 0, size, 0, &mapData);
    memcpy(mapData, imageDataBuffer, (imageWidth * imageHeight * bpp) >> 3);
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
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(mVkCompPtr->LogicalDevice(), &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate imageview failed!");
    }

    VkDescriptorImageInfo imageDesInfoY;
    imageDesInfoY.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageDesInfoY.imageView = textureImageView;
    imageDesInfos.push_back(imageDesInfoY);

    if (pixelPlaneCnt >= 2) {
        VkImageViewCreateInfo viewInfoU{};
        viewInfoU.pNext = nullptr;
        viewInfoU.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfoU.image = mImage;
        viewInfoU.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfoU.format = mPixelFormat;
        viewInfoU.components.r = VK_COMPONENT_SWIZZLE_R;
        viewInfoU.components.g = VK_COMPONENT_SWIZZLE_G;
        viewInfoU.components.b = VK_COMPONENT_SWIZZLE_B;
        viewInfoU.components.a = VK_COMPONENT_SWIZZLE_A;
        viewInfoU.subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT;
        viewInfoU.subresourceRange.baseMipLevel = 0;
        viewInfoU.subresourceRange.levelCount = 1;
        viewInfoU.subresourceRange.baseArrayLayer = 0;
        viewInfoU.subresourceRange.layerCount = 1;

        if (vkCreateImageView(mVkCompPtr->LogicalDevice(), &viewInfoU, nullptr, &textureImageViewU) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate imageview failed!");
        }

        VkDescriptorImageInfo imageDesInfoU;
        imageDesInfoU.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageDesInfoU.imageView = textureImageViewU;
        imageDesInfos.push_back(imageDesInfoU);
    }

    if (pixelPlaneCnt >= 3) {
        VkImageViewCreateInfo viewInfoV{};
        viewInfoV.pNext = nullptr;
        viewInfoV.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfoV.image = mImage;
        viewInfoV.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfoV.format = mPixelFormat;
        viewInfoV.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfoV.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfoV.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfoV.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfoV.subresourceRange.aspectMask = VK_IMAGE_ASPECT_PLANE_2_BIT;
        viewInfoV.subresourceRange.baseMipLevel = 0;
        viewInfoV.subresourceRange.levelCount = 1;
        viewInfoV.subresourceRange.baseArrayLayer = 0;
        viewInfoV.subresourceRange.layerCount = 1;

        if (vkCreateImageView(mVkCompPtr->LogicalDevice(), &viewInfoV, nullptr, &textureImageViewV) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate imageview failed!");
        }

        VkDescriptorImageInfo imageDesInfoV;
        imageDesInfoV.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageDesInfoV.imageView = textureImageViewV;
        imageDesInfos.push_back(imageDesInfoV);
    }

    //VkImageMemoryBarrier barrier = {};
    //barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    //barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    //barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //barrier.image = mImage;
    //barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //barrier.subresourceRange.baseMipLevel = 0;
    //barrier.subresourceRange.levelCount = 1;
    //barrier.subresourceRange.baseArrayLayer = 0;
    //barrier.subresourceRange.layerCount = 1;

    //barrier.srcAccessMask = 0;
    //barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    //barriers.push_back(barrier);

    VkBufferImageCopy region = {};

    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_PLANE_0_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        (unsigned int)imageWidth,
        (unsigned int)imageHeight,
        1
    };
    regions.push_back(region);

    if (pixelPlaneCnt >= 2) {
        VkBufferImageCopy regionU = {};

        regionU.bufferOffset = imageWidth * imageHeight;
        regionU.bufferRowLength = 0;
        regionU.bufferImageHeight = 0;
        regionU.imageSubresource.aspectMask = VK_IMAGE_ASPECT_PLANE_1_BIT;
        regionU.imageSubresource.mipLevel = 0;
        regionU.imageSubresource.baseArrayLayer = 0;
        regionU.imageSubresource.layerCount = 1;
        regionU.imageOffset = { 0, 0, 0 };
        regionU.imageExtent = {
            (unsigned int)imageWidth / 2,
            (unsigned int)imageHeight / 2,
            1
        };
        regions.push_back(regionU);
    }

    if (pixelPlaneCnt >= 3) {
        VkBufferImageCopy regionV = {};

        regionV.bufferOffset = imageWidth * imageHeight + imageWidth * imageHeight / 4;
        regionV.bufferRowLength = 0;
        regionV.bufferImageHeight = 0;
        regionV.imageSubresource.aspectMask = VK_IMAGE_ASPECT_PLANE_2_BIT;
        regionV.imageSubresource.mipLevel = 0;
        regionV.imageSubresource.baseArrayLayer = 0;
        regionV.imageSubresource.layerCount = 1;
        regionV.imageOffset = { 0, 0, 0 };
        regionV.imageExtent = {
            (unsigned int)imageWidth / 2,
            (unsigned int)imageHeight / 2,
            1
        };
        regions.push_back(regionV);
    }

    //vkCmdCopyBufferToImage(imageCopyCommand, stagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    //mVkCompPtr->EndSingleTimeCommands(imageCopyCommand);

    //vkDestroyBuffer(mVkCompPtr->LogicalDevice(), stagingBuffer, nullptr);
    //vkFreeMemory(mVkCompPtr->LogicalDevice(), stagingMemory, nullptr);
}

void VulkanImageLoader::LoadImageTexture(short imageWidth, short imageHeight, unsigned char* imageData, int pixelFormat) {

}

void VulkanImageLoader::ReadOneFrame() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - lastReadTime).count();
    if (elapsedTime < 1000000.0 / fps) {
        return;
    }

    lastReadTime = currentTime;

    int bpp = BitPerPixel();
    if (rawDataFile) {
        int size = (loadImageWidth * loadImageHeight * bpp) >> 3 ;
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
    //vkCmdPipelineBarrier(
    //    commandBuffer,
    //    sourceStage, destinationStage,
    //    0,
    //    0, nullptr,
    //    0, nullptr,
    //    1, barriers.data()
    //);

    vkCmdCopyBufferToImage(commandBuffer, stagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());
}

void VulkanImageLoader::BindTextureSampler(int index, VkSampler textureSampler) {
    imageDesInfos[index].sampler = textureSampler;
}

void VulkanImageLoader::SetImagePixelFormat(VkFormat imageFmt) {
    mPixelFormat = imageFmt;
}

int VulkanImageLoader::CurrentPixelPlaneCnt() {
    int pixelPlaneCnt = 1;
    switch (mPixelFormat) {
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM: {
            pixelPlaneCnt = 3;
            break;
        }
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM: {
            pixelPlaneCnt = 2;
            break;
        }
        default: {
            pixelPlaneCnt = 1;
            break;
        }
    }
    return pixelPlaneCnt;
}

std::string VulkanImageLoader::CurrentShaderSuffix() {
    std::string suffix = "";
    switch (mPixelFormat) {
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM: {
            suffix = "yuv420p";
            break;
        }
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM: {
            suffix = "yuvnv12";
            break;
        }
        default: {
            suffix = "rgb";
            break;
        }
    }
    return suffix;
}

int VulkanImageLoader::BitPerPixel() {
    int bitPerPiexl = 1;
    switch (mPixelFormat) {
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM: {
            bitPerPiexl = 12;
            break;
        }
        case VK_FORMAT_R16G16B16A16_UNORM: {
            bitPerPiexl = 64;
            break;
        }
        case VK_FORMAT_B8G8R8_UNORM:
        case VK_FORMAT_R8G8B8_UNORM: {
            bitPerPiexl = 24;
            break;
        }
        default: {
            bitPerPiexl = 32;
            break;
        }
    }
    return bitPerPiexl;
}