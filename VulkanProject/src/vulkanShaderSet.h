#pragma once

#include "vulkanBase.h"

class VulkanComp;

class VulkanShaderSet{
public:
    VulkanShaderSet() = default;
    VulkanShaderSet(VulkanComp* comp);
    ~VulkanShaderSet();

    void LoadVertShaderStageInfo(const std::string vertFilePath);
    void LoadFragShaderStageInfo(const std::string fragFilePath);

    void CreateAndLoadLayoutBinding(uint32_t id, VkDescriptorType type, VkShaderStageFlagBits flag, VkSampler sampler);

    size_t ShaderStageCreateInfoCount() { return shaderStageCreateInfos.size(); }
    VkPipelineShaderStageCreateInfo* ShaderStageCreateInfoData() { return shaderStageCreateInfos.data(); }

    size_t DescriptorSetLayoutBindingsCount() { return descriptorSetLayoutBindings.size(); }
    VkDescriptorSetLayoutBinding* DescriptorSetLayoutBindingsData() { return descriptorSetLayoutBindings.data(); }

private:
    static std::vector<char> readFile(const std::string& filename);

    VulkanComp* vkCompPtr;

    VkSampler mTextureSampler;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;

    //Pipeline Layout
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    
};
