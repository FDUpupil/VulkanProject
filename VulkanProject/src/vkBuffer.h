#pragma once

#include "vulkanBase.h"

class VkBufferMgr {
public:
    VkBufferMgr();
    ~VkBufferMgr();

    void InitBuffer(VkDevice device,
                    VkPhysicalDevice physicaldevice,
                    VkDeviceSize size,
                    VkBufferUsageFlags usageFlags,
                    VkMemoryPropertyFlags memoryPropertyFlags);
    void Map(VkDevice device, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void WriteToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    VkBuffer &GetBuffer() {return buffer;}
    int GetCount() { return count; }

private:
    uint32_t findMemoryType(VkPhysicalDevice physicaldevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    VkDeviceSize bufferSize;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    int count;
    void* mapped = nullptr;
};