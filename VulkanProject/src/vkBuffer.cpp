#include "vkBuffer.h"

VkBufferMgr::VkBufferMgr() {

}

VkBufferMgr::~VkBufferMgr() {

}

uint32_t VkBufferMgr::findMemoryType(VkPhysicalDevice physicaldevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(physicaldevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
        (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

void VkBufferMgr::InitBuffer(  VkDevice device,
                            VkPhysicalDevice physicaldevice,
                            VkDeviceSize size,
                            VkBufferUsageFlags usageFlags,
                            VkMemoryPropertyFlags memoryPropertyFlags) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicaldevice, memRequirements.memoryTypeBits, memoryPropertyFlags);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate vertex buffer memory!");
    }

    vkBindBufferMemory(device, buffer, memory, 0);
    bufferSize = size;
}

void VkBufferMgr::Map(VkDevice device, VkDeviceSize size, VkDeviceSize offset) {
    if(vkMapMemory(device, memory, offset, size, 0, &mapped) != VK_SUCCESS) {

    }
}

void VkBufferMgr::WriteToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
    if (size == VK_WHOLE_SIZE) {
        memcpy(mapped, data, bufferSize);
    } else {
        char *memOffset = (char *)mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}